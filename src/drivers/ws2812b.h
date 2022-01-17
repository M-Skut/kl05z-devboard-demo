#ifndef WS2812B_H_INCLUDE
#define WS2812B_H_INCLUDE
#include "gpio.h"
#include <stdint.h>

void ws2812b_init(gpio_pin_t pin);
//void ws2812b_write(gpio_pin_t pin, uint32_t *data, uint32_t length);
void ws2812b_write(gpio_pin_t pin, uint8_t *data, uint32_t length);
#endif