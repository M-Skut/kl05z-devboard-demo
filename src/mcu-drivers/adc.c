#include <stdbool.h>
#include <stdint.h>

#include "MKL05Z4.h"
#include "adc.h"
#include "gpio.h"

typedef struct gpio_to_adc_s {
    gpio_pin_t pin;
    uint8_t adc_channel;
} gpio_to_adc_t;

static const gpio_to_adc_t adc_pin_map[14] = {
    { { PORTA, GPIOA, 12 }, 0 },
    { { PORTB, GPIOB, 5 }, 1 },
    { { PORTA, GPIOA, 9 }, 2 },
    { { PORTA, GPIOA, 8 }, 3 },
    { { PORTB, GPIOB, 2 }, 4 },
    { { PORTB, GPIOB, 1 }, 5 },
    { { PORTB, GPIOB, 0 }, 6 },
    { { PORTA, GPIOA, 7 }, 7 },
    { { PORTB, GPIOB, 11 }, 8 },
    { { PORTB, GPIOB, 10 }, 9 },
    { { PORTB, GPIOB, 9 }, 10 },
    { { PORTB, GPIOB, 8 }, 11 },
    { { PORTA, GPIOA, 0 }, 12 },
    { { PORTB, GPIOB, 13 }, 13 },
};

static bool calibration_done = false;

adc_pin_t adc_from_gpio(gpio_pin_t pin)
{
    adc_pin_t adc_pin = { 0 };
    for (uint8_t i = 0; i < 14; i++) {
        if (pin.gpio == adc_pin_map[i].pin.gpio
            && pin.pin_number == adc_pin_map[i].pin.pin_number
            && pin.port == adc_pin_map[i].pin.port) {
            adc_pin.channel = adc_pin_map[i].adc_channel;
        }
    }
    // If pin not found in table, return zeroed struct
    return adc_pin;
}

adc_status_t adc_init_calibration(void)
{
    uint16_t cal_value;
    // enable ADC0 clock
    SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
    // Enable Software Conversion Trigger for Calibration Process
    ADC0->SC2 &= ~ADC_SC2_ADTRG_MASK;
    // Set single conversion, clear avgs bitfield for next writing
    ADC0->SC3 &= (~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK);
    // For best calibration results turn averaging ON and set at max value ( 32 )
    ADC0->SC3 |= (ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3));
    // Start Calibration
    ADC0->SC3 |= ADC_SC3_CAL_MASK;
    // Wait for calibration to end
    while (ADC0->SC3 & ADC_SC3_CAL_MASK)
        ;
    // Check calibration results
    if (ADC0->SC3 & ADC_SC3_CALF_MASK) {
        ADC0->SC3 |= ADC_SC3_CALF_MASK;
        return ADC_ERR_CAL_FAILED;
    }
    // Calculate and set plus-side calibration
    cal_value = 0x00;
    cal_value += ADC0->CLP0;
    cal_value += ADC0->CLP1;
    cal_value += ADC0->CLP2;
    cal_value += ADC0->CLP3;
    cal_value += ADC0->CLP4;
    cal_value += ADC0->CLPS;
    cal_value += ADC0->CLPD;
    cal_value /= 2;
    cal_value |= 0x8000;
    ADC0->PG = ADC_PG_PG(cal_value);

    return ADC_OK;
}

adc_status_t adc_init(gpio_pin_t pin_gpio, adc_pin_t pin_adc, adc_config_t config)
{
    adc_status_t result;
    // Enable port clock
    if (pin_gpio.port == PORTA) {
        SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    } else {
        SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    }

    // Check if calibration was already done. If not, calibrate
    if (!calibration_done) {
        result = adc_init_calibration();
        // If calibration failed, return
        if (result)
            return result;
    }

    if (config == ADC_CONFIG_LOWPOWER_FAST) {
        ADC0->CFG1 = ADC_CFG1_ADLPC_MASK // Low-Power Configuration
            | ADC_CFG1_ADIV(3) // Clock Divide Select: (Input Clock)/8
            | ADC_CFG1_ADLSMP_MASK // Long Sample Time
            | ADC_CFG1_MODE(3) // (16)bits Resolution
            | ADC_CFG1_ADICLK(1); // Input Clock: (Bus Clock)/2

        ADC0->CFG2 = ADC_CFG2_MUXSEL_MASK // ADxxb channels are selected
            | ADC_CFG2_ADACKEN_MASK // Asynchronous Clock Output Enable
            | ADC_CFG2_ADHSC_MASK // High-Speed Configuration
            | ADC_CFG2_ADLSTS(0); // Long Sample Time Select

        ADC0->SC2 = ADC_SC2_REFSEL(0); // Default Voltage Reference

        ADC0->SC3 = ADC_SC3_AVGE_MASK // Hardware Average Enable
            | ADC_SC3_AVGS(0); // 4 Samples Averaged
    } else {
        return ADC_ERR_CONFIG;
    }

    // Reconfigure pin for analog input
    pin_gpio.port->PCR[pin_gpio.pin_number] |= PORT_PCR_MUX(0);

    return ADC_OK;
}

uint16_t adc_read(adc_pin_t pin_adc)
{
    // Start conversion and wait until complete
    ADC0->SC1[0] = ADC_SC1_ADCH(pin_adc.channel);
    while ((ADC0->SC1[0] & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK)
        ;

    return (uint16_t)ADC0->R[0];
}