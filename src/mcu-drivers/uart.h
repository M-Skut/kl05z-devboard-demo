#ifndef UART_H_INCLUDE
#define UART_H_INCLUDE
#include <stdbool.h>
#include <stdint.h>
/** @file uart.h
 *  @brief UART driver for MKL05 microcontrollers in 32-pin packages.
 *  Supports only UART0 without DMA. Features:
 * - Different pinout configuration
 * - Automatic calculation for baudrate with ability to re-init if CPU speed changes
 * - Error if configuration baudrate or pins cannot be set
 * - Simple print, putchar and getchar API
 * - RX IRQ callback setting and checking RX overrun
 */

/**
 * @enum UART driver status
 */
typedef enum uart_status_e {
    UART_OK = 0, /**< UART function success */
    UART_CONFIGERR_GPIO = 1, /**< UART configuration error, invalid GPIO used */
    UART_CONFIGERR_BAUD = 2 /**< UART configuration error, desired baud cannot be set with current system clock*/
} uart_status_t;

/**
 * @enum UART TX pin
 */
typedef enum uart_tx_pin_e {
    UART_PTB1_TX = 1, /**< PTB1 Tx Pin */
    UART_PTB2_TX = 2, /**< PTB2 Tx Pin */
    UART_PTB3_TX = 3, /**< PTB3 Tx Pin */
} uart_tx_pin_t;

/**
 * @enum UART RX pin
 */
typedef enum uart_rx_pin_e {
    UART_PTB1_RX = 4, /**< PTB1 Rx Pin */
    UART_PTB2_RX = 5, /**< PTB2 Rx Pin */
    UART_PTB4_RX = 6, /**< PTB4 Rx Pin */
} uart_rx_pin_t;

/**
 * @brief Function pointer for UART RX interrupt callback
 * callback function should not take any arguments or return any values.
 */
typedef void (*uart_irq)(void);

/**
 * @brief UART Initialization function
 * This function calculates clock registers and returns
 * error when the desired baudrate cannot be set with
 * current system clock.
 * 
 * @param baudrate UART baud rate (eg. 9600, 38400, 115200)
 * 
 * @param rx RX pin choice from uart_pin_t type. Available pins:
 * UART_PTB1_RX, UART_PTB2_RX, UART_PTB4_RX
 * 
 *  * @param tx TX pin choice from uart_pin_t type. Available pins:
 * UART_PTB1_TX, UART_PTB2_TX, UART_PTB3_TX
 * 
 * @return uart_status_t enum value. Possible states:
 * - UART_OK - UART0 was initialised properly.
 * - UART_CONFIGERR - UART0 cannot be initialized because the
 * desired baudrate cannot be set with current system clock.
 */
uart_status_t uart_init(uint32_t baudrate, uart_rx_pin_t rx, uart_tx_pin_t tx);

/**
 * @brief Print string to UART
 * UART needs to be configured using `uart_init` before
 * this function can be used.
 * @param str string to print
 */
void uart_print(const char* str);

/**
 * @brief Print single character to UART
 * UART needs to be configured using `uart_init` before
 * this function can be used.
 * @param data 8-bit character to print
 */
void uart_putchar(uint8_t data);

/**
 * @brief Get byte from RX buffer.
 * UART needs to be configured using `uart_init` before
 * this function can be used.
 * @return uint8_t byte from UART RX buffer
 */
uint8_t uart_getchar(void);

/**
 * @brief Check if new byte is in RX buffer
 * UART needs to be configured using `uart_init` before
 * this function can be used.
 * @return uint8_t Non-zero value if new byte is in RX buffer, zero if buffer empty
 */
uint8_t uart_available(void);

/**
 * @brief Check if UART RX buffer is full and new data was lost.
 * This fuction reads UART0_S1_OR_MASK and clears it if it's set.
 * UART needs to be configured using `uart_init` before
 * this function can be used.
 * @return true RX buffer is full, new data was lost
 * @return false RX buffer not full or no new data arrived yet
 */
bool rx_overrun(void);

/**
 * @brief Set RX IRQ callback for UART
 * To set callback function, pass function pointer to this function and
 * to disable, pass NULL pointer or zero.
 * 
 * @param callback uart_irq function pointer (function with no parameters or return value)
 */
void uart_set_irq(uart_irq callback);

#endif
