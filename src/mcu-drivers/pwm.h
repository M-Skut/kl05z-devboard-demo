#ifndef PWM_H_INCLUDE
#define PWM_H_INCLUDE

#include <stdbool.h>
#include <stdint.h>

#include "MKL05Z4.h"
#include "gpio.h"

typedef enum pwm_status_e {
    PWM_OK = 0,
    PWM_ERR_CONFIG = 1,
} pwm_status_t;

typedef enum pwm_prescaler_e
{
    PRESCALER_1   = 0,
    PRESCALER_2   = 1,
    PRESCALER_4   = 2,
    PRESCALER_8   = 3,
    PRESCALER_16  = 4,
    PRESCALER_32  = 5,
    PRESCALER_64  = 6,
    PRESCALER_128 = 7,
} pwm_prescaler_t;

typedef struct pwm_pin_s
{
    TPM_Type* tpm;
    uint8_t tpm_channel;
} pwm_pin_t;

void pwm_set_prescaler(TPM_Type* tpm, pwm_prescaler_t prescaler);
pwm_pin_t pwm_from_gpio(gpio_pin_t pin);
pwm_status_t pwm_init(gpio_pin_t pin_gpio, pwm_pin_t pin_tpm);
void pwm_fast_write_duty(pwm_pin_t pin, uint16_t value);
void pwm_fast_write_period(pwm_pin_t pin, uint16_t value);
void pwm_period_us(pwm_pin_t pin, uint32_t period_us);
void pwm_pulsewidth_us(pwm_pin_t pin, uint32_t pulsewidth_us);

#endif