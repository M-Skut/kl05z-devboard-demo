#include <stdbool.h>
#include <stdint.h>

#include "MKL05Z4.h"
#include "gpio.h"
#include "pwm.h"

typedef struct gpio_to_tpm_s {
    gpio_pin_t pin;
    TPM_Type* tpm;
    uint8_t tpm_channel;
} gpio_to_tpm_t;

static const uint8_t ps_to_val[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

static uint8_t tpm0_prescaler = 64;
static uint8_t tpm1_prescaler = 64;

static const gpio_to_tpm_t tpm_pin_map[10] = {
    { { PORTB, GPIOB, 11 }, TPM0, 0 },
    { { PORTB, GPIOB, 10 }, TPM0, 1 },
    { { PORTB, GPIOB, 9 }, TPM0, 2 },
    { { PORTB, GPIOB, 8 }, TPM0, 3 },
    { { PORTA, GPIOA, 6 }, TPM0, 4 },
    { { PORTA, GPIOA, 5 }, TPM0, 5 },
    { { PORTA, GPIOA, 12 }, TPM1, 0 },
    { { PORTB, GPIOB, 5 }, TPM1, 1 },
    { { PORTA, GPIOA, 0 }, TPM1, 0 },
    { { PORTB, GPIOB, 13 }, TPM1, 1 },
};

static uint32_t tpm_time_to_val(uint32_t time_us, TPM_Type* tpm)
{
    uint32_t tpm_clk;
    if (tpm == TPM0) {
        tpm_clk = SystemCoreClock / tpm0_prescaler;
    } else {
        tpm_clk = SystemCoreClock / tpm1_prescaler;
    }
    return (uint32_t)((uint64_t)time_us * tpm_clk / 1000000);
}

pwm_pin_t pwm_from_gpio(gpio_pin_t pin)
{
    pwm_pin_t pwm_pin = { (TPM_Type*)0, 0 };
    for (uint8_t i = 0; i < 10; i++) {
        if (pin.gpio == tpm_pin_map[i].pin.gpio
            && pin.pin_number == tpm_pin_map[i].pin.pin_number
            && pin.port == tpm_pin_map[i].pin.port) {
            pwm_pin.tpm = tpm_pin_map[i].tpm;
            pwm_pin.tpm_channel = tpm_pin_map[i].tpm_channel;
        }
    }
    // If pin not found in table, return zeroed struct
    return pwm_pin;
}

void pwm_set_prescaler(TPM_Type* tpm, pwm_prescaler_t prescaler)
{
    tpm->SC = TPM_SC_CMOD(0);
    tpm->SC = TPM_SC_CMOD(1) | TPM_SC_PS(prescaler);
    if (tpm == TPM0) {
        tpm0_prescaler = ps_to_val[prescaler];
    } else {
        tpm1_prescaler = ps_to_val[prescaler];
    }
}

pwm_status_t pwm_init(gpio_pin_t pin_gpio, pwm_pin_t pin_tpm)
{
    if (pin_tpm.tpm == 0)
        return PWM_ERR_CONFIG;
    // Enable clock for the pin and TPM
    if (pin_gpio.port == PORTA) {
        SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    } else {
        SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    }
    if (pin_tpm.tpm == TPM0) {
        SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    } else {
        SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
    }
    // Set default 64 prescaler
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    pin_tpm.tpm->SC = TPM_SC_CMOD(1) | TPM_SC_PS(6);
    // No Interrupts and High True pulses on Edge Aligned PWM
    pin_tpm.tpm->CONTROLS[pin_tpm.tpm_channel].CnSC = (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);
    // 20 ms default period for TPM
    pwm_period_us(pin_tpm, 20000);
    pin_tpm.tpm->CONTROLS[pin_tpm.tpm_channel].CnV = 0;
    // Set pin for output
    pin_gpio.port->PCR[pin_gpio.pin_number] |= PORT_PCR_MUX(2);

    return PWM_OK;
}

void pwm_fast_write_duty(pwm_pin_t pin, uint16_t value)
{
    pin.tpm->CONTROLS[pin.tpm_channel].CnV = value;
}

void pwm_fast_write_period(pwm_pin_t pin, uint16_t value)
{
    pin.tpm->MOD = value;
}

void pwm_period_us(pwm_pin_t pin, uint32_t period_us)
{
    pin.tpm->MOD = tpm_time_to_val(period_us, pin.tpm);
}

void pwm_pulsewidth_us(pwm_pin_t pin, uint32_t pulsewidth_us)
{
    pin.tpm->CONTROLS[pin.tpm_channel].CnV = tpm_time_to_val(pulsewidth_us, pin.tpm);
}