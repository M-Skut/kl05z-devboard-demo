#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "MKL05Z4.h"
#include "uart.h"

/**
 * Documentation available in header file  
 */

static uart_irq rx_irq;

typedef enum uart_clock_source_e {
    CLK_DIS = 0x00,
    MCGFLLCLK = 0x01,
    OSCERCLK = 0x02,
    MCGIRCLK = 0x03
} uart_clock_source_t;

uart_status_t uart_init(uint32_t baudrate, uart_rx_pin_t rx, uart_tx_pin_t tx)
{
    uart_status_t result = UART_OK;
    uint8_t osr_val;
    uint16_t sbr;
    uint32_t diff, calculated_baud, uart_clock;
    uint32_t baud_diff = UINT32_MAX;
    // Enable UART clock
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    // Disable UART before configuration
    UART0->C2 &= (uint8_t) ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);

    // UART clock hardwired to system core clock
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(MCGFLLCLK);
    uart_clock = SystemCoreClock;

    // Set UART0 GPIO
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    switch (rx) {
    case UART_PTB1_RX:
        PORTB->PCR[1] = PORT_PCR_MUX(3);
        break;
    case UART_PTB2_RX:
        PORTB->PCR[2] = PORT_PCR_MUX(2);
        break;
    case UART_PTB4_RX:
        PORTB->PCR[4] = PORT_PCR_MUX(3);
        break;
    default:
        result = UART_CONFIGERR_GPIO;
        goto fail;
    }

    switch (tx) {
    case UART_PTB1_TX:
        PORTB->PCR[1] = PORT_PCR_MUX(2);
        break;
    case UART_PTB2_TX:
        PORTB->PCR[2] = PORT_PCR_MUX(3);
        break;
    case UART_PTB3_TX:
        PORTB->PCR[3] = PORT_PCR_MUX(3);
        break;
    default:
        result = UART_CONFIGERR_GPIO;
        goto fail;
    }

    // 8bit, no parity
    UART0->C1 = 0x00;

    // Select the best OSR value
    for (uint8_t i = 4; i <= 32; i++) {
        sbr = (uint16_t)(uart_clock / (baudrate * i));
        calculated_baud = (uart_clock / (i * sbr));
        diff = (calculated_baud > baudrate) ? calculated_baud - baudrate : baudrate - calculated_baud;

        if (diff <= baud_diff) {
            baud_diff = diff;
            osr_val = i;
        }
    }

    // More than 3% baud difference will not alllow for communication
    if (baud_diff > ((baudrate / 100) * 3)) {
        result = UART_CONFIGERR_BAUD;
        goto fail;
    }

    // If the OSR is between 4x and 8x then both edge sampling must be turned on.
    if ((osr_val > 3) && (osr_val < 9))
        UART0->C5 |= UART0_C5_BOTHEDGE_MASK;

    sbr = (uint16_t)(uart_clock / (baudrate * osr_val));

    UART0->C4 = (uint8_t)((UART0->C4 & ~UART0_C4_OSR_MASK) | UART0_C4_OSR(osr_val - 1));
    UART0_BDH = UART0_BDH_SBR(sbr >> 8);
    UART0_BDL = UART0_BDL_SBR(sbr);

    // Enable receiver and set interrupts, left to be enabled by NVIC
    UART0->C2 |= UART0_C2_RIE_MASK;
    UART0->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);
    UART0->C3 |= UART0_C3_ORIE_MASK;
    return result;

fail:
    SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
    SIM->SCGC4 &= ~SIM_SCGC4_UART0_MASK;
    return result;
}

void uart_putchar(uint8_t data)
{
    while (!(UART0->S1 & UART0_S1_TDRE_MASK))
        ;
    UART0->D = data;
}

uint8_t uart_getchar(void)
{
    int data = UART0->D;
    return data;
}

void uart_print(const char* str)
{
    uint8_t str_len = 0;
    while (str[str_len] != '\0') {
        uart_putchar(str[str_len]);
        ++str_len;
    }
}

uint8_t uart_available(void)
{
    return UART0->S1 & UART0_S1_RDRF_MASK;
}

bool rx_overrun(void)
{
    // Clear overrun flag if there is one
    if (UART0->S1 & UART0_S1_OR_MASK) {
        UART0->S1 |= UART0_S1_OR_MASK;
        return true;
    } else {
        return false;
    }
}

void uart_set_irq(uart_irq callback)
{
    rx_irq = callback;
}

void UART0_IRQHandler()
{
    if (rx_irq)
        rx_irq();
}
