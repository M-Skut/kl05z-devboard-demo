#include <stdint.h>

#include "MKL05Z4.h"
#include "cmdline.h"
#include "error_handler.h"
#include "i2c.h"
#include "microrl.h"
#include "spi.h"
#include "u8x8.h"
#include "u8x8_mkl_hal.h"
#include "uart.h"
#include "delay.h"


microrl_t rl;
microrl_t* prl = &rl;

#define DELAY(x)                               \
    for (uint32_t i = 0; i < (x * 10000); i++) \
        __NOP();

/** Usage of the prompt
 * In putty -> terminal check "implicit Cr in every LF" and "implicit LF in every CF"
 * In picocom use imap for example: picocom -b 9600 /dev/ttyACM0 --imap crcrlf
 */

int main()
{ 
    delay_init();
    if (uart_init(9600, UART_PTB2_RX, UART_PTB1_TX) != UART_OK)
        display_error();
    if (i2c_init(I2C_PTB3_SCL, I2C_PTB4_SDA, 100000) != I2C_OK)
        display_error();
    if (spi_init(SPI_MODE_0, 1000000, SPI_SIDE_MASTER) != SPI_OK)
         display_error();

    cmdline_init(prl);

    while (1)
    {
        process_uart();
    }
}
