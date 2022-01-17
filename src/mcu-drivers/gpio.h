#ifndef GPIO_H_INCLUDE
#define GPIO_H_INCLUDE

#include "MKL05Z4.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct gpio_pin_s {
    PORT_Type* port;
    GPIO_Type* gpio;
    uint8_t pin_number;
} gpio_pin_t;

typedef enum gpio_direction_e {
    GPIO_INPUT = 0,
    GPIO_OUTPUT = 1,
} gpio_direction_t;

typedef void (*gpio_port_irq_t)(void);

typedef enum gpio_option_e {
    GPIO_PULL_DOWN = PORT_PCR_PS(0),
    GPIO_PULL_UP = PORT_PCR_PS(1),
    GPIO_PULL_DISABLE = PORT_PCR_PE(0),
    GPIO_PULL_ENABLE = PORT_PCR_PE(1),
    GPIO_SLEW_FAST = PORT_PCR_SRE(0),
    GPIO_SLEW_SLOW = PORT_PCR_SRE(1),
    GPIO_FILTER_OFF = PORT_PCR_SRE(0),
    GPIO_FILTER_ON = PORT_PCR_PFE(1),
    GPIO_DRIVE_WEAK = PORT_PCR_DSE(0),
    GPIO_DRIVE_STRONG = PORT_PCR_DSE(1),
    GPIO_MUX_ALT0 = PORT_PCR_MUX(0),
    GPIO_MUX_ALT1 = PORT_PCR_MUX(1),
    GPIO_MUX_ALT2 = PORT_PCR_MUX(2),
    GPIO_MUX_ALT3 = PORT_PCR_MUX(3),
    GPIO_DMA_DISABLED = PORT_PCR_IRQC(0),
    GPIO_DMA_RISING_EGDE = PORT_PCR_IRQC(1),
    GPIO_DMA_FALLING_EGDE = PORT_PCR_IRQC(2),
    GPIO_DMA_BOTH_EDGE = PORT_PCR_IRQC(3),
} gpio_option_t;

typedef enum gpio_irq_option_e {
    GPIO_IRQ_ON_LOW = PORT_PCR_IRQC(8),
    GPIO_IRQ_RISING_EGDE = PORT_PCR_IRQC(9),
    GPIO_IRQ_FALLING_EGDE = PORT_PCR_IRQC(10),
    GPIO_IRQ_BOTH_EDGE = PORT_PCR_IRQC(11),
    GPIO_IRQ_ON_HIGH = PORT_PCR_IRQC(12),
} gpio_irq_option_t;

void gpio_pin_init(gpio_pin_t pin, gpio_direction_t direction, gpio_option_t options);
void gpio_set_options(gpio_pin_t pin, gpio_option_t options);
void gpio_toggle(gpio_pin_t pin);
void gpio_set(gpio_pin_t, bool enabled);
void gpio_fast_on(gpio_pin_t pin);
void gpio_fast_off(gpio_pin_t pin);
bool gpio_get(gpio_pin_t);
void gpio_attach_interrupt(gpio_pin_t pin, gpio_irq_option_t, gpio_port_irq_t callback);

#endif
