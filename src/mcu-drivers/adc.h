#ifndef ADC_H_INCLUDE
#define ADC_H_INCLUDE

#include <stdbool.h>
#include <stdint.h>

#include "MKL05Z4.h"
#include "gpio.h"

typedef enum adc_status_e {
    ADC_OK = 0,
    ADC_ERR_CONFIG = 1,
    ADC_ERR_CAL_FAILED = 2,
} adc_status_t;

typedef enum adc_config_e {
    ADC_CONFIG_LOWPOWER_FAST,
} adc_config_t;

typedef struct adc_pin_s {
    uint8_t channel;
} adc_pin_t;

adc_pin_t adc_from_gpio(gpio_pin_t pin);
adc_status_t adc_init_calibration(void);
adc_status_t adc_init(gpio_pin_t pin_gpio, adc_pin_t pin_adc, adc_config_t config);
uint16_t adc_read(adc_pin_t pin_adc);

#endif