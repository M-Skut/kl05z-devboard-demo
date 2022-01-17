#ifndef U8X8_MKL_HAL_H_INCLUDE
#define U8X8_MKL_HAL_H_INCLUDE

#include <stdint.h>
#include "u8x8.h"

uint8_t u8x8_gpio_and_delay_mkl05(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_byte_mkl05_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif
