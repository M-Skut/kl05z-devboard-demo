#include <stdint.h>
#include "u8x8_mkl_hal.h"
#include "MKL05Z4.h"
#include "u8x8.h"
#include "spi.h"
#include "delay.h"
#include "gpio.h"

static gpio_pin_t sw_spi_clk = {PORTB, GPIOB, 0};
static gpio_pin_t sw_spi_mosi = {PORTA, GPIOA, 7};
static gpio_pin_t sw_spi_cs = {PORTB, GPIOB, 9};
static gpio_pin_t sw_spi_rst = {PORTA, GPIOA, 12};
static gpio_pin_t sw_spi_dc = {PORTA, GPIOA, 11};


uint8_t u8x8_gpio_and_delay_mkl05(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
      //gpio_pin_init(sw_spi_clk, GPIO_OUTPUT, GPIO_MUX_ALT1);
      //gpio_pin_init(sw_spi_mosi, GPIO_OUTPUT, GPIO_MUX_ALT1);
      gpio_pin_init(sw_spi_cs, GPIO_OUTPUT, GPIO_MUX_ALT1);
      gpio_pin_init(sw_spi_rst, GPIO_OUTPUT, GPIO_MUX_ALT1);

      // DC for nokia
      gpio_pin_init(sw_spi_dc, GPIO_OUTPUT, GPIO_MUX_ALT1);

      break;
    case U8X8_MSG_DELAY_NANO:			// delay arg_int * 1 nano second
      break;    
    case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
      break;
    case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
      delay_micro_seconds(arg_int*10UL);
      break;
    case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
      delay_micro_seconds(arg_int*1000UL);
      break;
    case U8X8_MSG_DELAY_I2C:				// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
      delay_micro_seconds(arg_int<=2?5:1);
      break;							
    case U8X8_MSG_GPIO_D0:				// D0 or SPI clock pin: Output level in arg_int
    //case U8X8_MSG_GPIO_SPI_CLOCK:
    gpio_set(sw_spi_clk, arg_int);
    break;
    case U8X8_MSG_GPIO_D1:				// D1 or SPI data pin: Output level in arg_int
    //case U8X8_MSG_GPIO_SPI_DATA:
      gpio_set(sw_spi_mosi, arg_int);
      break;    
    case U8X8_MSG_GPIO_D2:				// D2 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D3:				// D3 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D4:				// D4 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D5:				// D5 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D6:				// D6 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D7:				// D7 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_E:				// E/WR pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_CS:				// CS (chip select) pin: Output level in arg_int
      gpio_set(sw_spi_cs, arg_int);
      break;
    case U8X8_MSG_GPIO_DC:				// DC (data/cmd, A0, register select) pin: Output level in arg_int
      gpio_set(sw_spi_dc, arg_int);
      break;
    case U8X8_MSG_GPIO_RESET:			// Reset pin: Output level in arg_int;
      gpio_set(sw_spi_rst, arg_int);
      break;
    case U8X8_MSG_GPIO_CS1:				// CS1 (chip select) pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_CS2:				// CS2 (chip select) pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_I2C_CLOCK:		// arg_int=0: Output low at I2C clock pin
      break;							// arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:			// arg_int=0: Output low at I2C data pin
      break;							// arg_int=1: Input dir with pullup high for I2C data pin
    case U8X8_MSG_GPIO_MENU_SELECT:
      u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_NEXT:
      u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_PREV:
      u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_HOME:
      u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
      break;
    default:
      u8x8_SetGPIOResult(u8x8, 1);			// default return value
      break;
  }
  return 1;
}

uint8_t u8x8_byte_mkl05_hw_spi (u8x8_t * u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  uint8_t *data;

  switch (msg) {
    case U8X8_MSG_BYTE_INIT:
      // Optional spi_init call if not called before
      //spi_init(u8x8->display_info->spi_mode, u8x8->display_info->sck_clock_hz, SPI_SIDE_MASTER);
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
      break;
    case U8X8_MSG_BYTE_SET_DC:
      u8x8_gpio_SetDC(u8x8, arg_int);
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
      u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
      break;
    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0) {
            spi_master_write(*data);
            data++;
            arg_int--;
        }
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, NULL);
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
      break;
    default:
      return 0;
  }
  
  return 1;
}
