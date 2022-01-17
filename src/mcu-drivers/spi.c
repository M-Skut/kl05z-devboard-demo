#include "spi.h"
#include "MKL05Z4.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum spi_data_status_e {
    SPI_NODATA = 0,
    SPI_DATA_RECIEVED = 1,
    SPI_DATA_SENT = 2,
} spi_data_status_t;

spi_status_t spi_init(spi_mode_t mode, uint32_t speed, spi_side_t side) {
    uint32_t diff = 0;
    uint32_t speed_diff = UINT32_MAX;
    uint32_t calculated_speed = 0;
    uint8_t  ref_spr = 0;
    uint8_t  ref_prescaler = 0;

    if (mode > SPI_MODE_3 || side > SPI_SIDE_MASTER)
        return SPI_CONFIGERR_PERIPH;

    // Configure pins and clocks
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK);
    SIM->SCGC4 |= SIM_SCGC4_SPI0_MASK;
    // MISO on PTA6
    PORTA->PCR[6] = PORT_PCR_MUX(3);
    // MOSI on PTA7
    PORTA->PCR[7] = PORT_PCR_MUX(3);
    // SCK on PTB0
    PORTB->PCR[0] = PORT_PCR_MUX(3);
    // SS on PTA5 for slave mode
    if (side == SPI_SIDE_SLAVE) {
        PORTA->PCR[5] = PORT_PCR_MUX(3);
    }

    uint8_t polarity = (mode & 0x02) ? 1 : 0;
    uint8_t phase = (mode & 0x01) ? 1 : 0;
    uint8_t c1_reg = ((side) << 4) | (polarity << 3) | (phase << 2);
    // clear MSTR, CPOL and CPHA bits
    SPI0->C1 &= ~(0x07 << 2);
    SPI0->C1 |= c1_reg;

    // Calculate and set speed by testing all possible prescaler
    // and divisor combinations
    uint32_t spi_clock = SystemCoreClock / 2;
    uint8_t prescaler, divisor;
    for (prescaler = 1; prescaler <= 8; prescaler++) {
        divisor = 2;
        for (uint8_t spr = 0; spr <= 8; spr++) {
            calculated_speed = spi_clock / (prescaler*divisor);
            diff = (calculated_speed > speed) ? calculated_speed - speed : speed - calculated_speed;
            if (diff < speed_diff) {
                ref_spr = spr;
                ref_prescaler = prescaler - 1;
                speed_diff = diff;
            }
            divisor *= 2;
        }
    }

    SPI0->BR = ((ref_prescaler & 0x7) << 4) | (ref_spr & 0xf);
    // Enable SPI and return
    SPI0->C1 |= SPI_C1_SPE_MASK;
    return SPI_OK;
}

static inline int spi_writeable() {
    return (SPI0->S & SPI_S_SPTEF_MASK) ? SPI_DATA_SENT : SPI_NODATA;
}

static inline int spi_readable() {
    return (SPI0->S & SPI_S_SPRF_MASK) ? SPI_DATA_RECIEVED : SPI_NODATA;
}

uint8_t spi_master_write(uint8_t data) {
    // wait until TX buffer is empty
    while(!spi_writeable());

    SPI0->D = data;

    // wait until RX buffer is full
    while (!spi_readable());
    return SPI0->D;
}

uint8_t spi_slave_receive() {
    return spi_readable();
}

uint8_t spi_slave_read(void) {
    return SPI0->D;
}

void spi_slave_write(uint8_t data) {
    while (!spi_writeable());
    SPI0->D = data;
}
