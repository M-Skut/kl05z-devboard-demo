#include "gpio.h"
#include "MKL05Z4.h"
#include <stdbool.h>
#include <stdint.h>

static const uint32_t port_mask = PORT_PCR_MUX_MASK | PORT_PCR_DSE_MASK | PORT_PCR_PFE_MASK | PORT_PCR_SRE_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
static const uint32_t port_irq_mask = PORT_PCR_ISF_MASK | PORT_PCR_IRQC_MASK;
static gpio_port_irq_t porta_irq = 0;
static gpio_port_irq_t portb_irq = 0;

void gpio_pin_init(gpio_pin_t pin, gpio_direction_t direction, gpio_option_t options)
{
    // Enable clock and set direction for the pin
    if (pin.port == PORTA) {
        SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    } else {
        SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    }
    // set options in PCR register and direction PDDR register
    pin.gpio->PDDR = (pin.gpio->PDDR & ~(1 << pin.pin_number)) | (direction << pin.pin_number);
    pin.port->PCR[pin.pin_number] = (pin.port->PCR[pin.pin_number] & ~port_mask) | options;
}

void gpio_set_options(gpio_pin_t pin, gpio_option_t options)
{
    pin.port->PCR[pin.pin_number] = (pin.port->PCR[pin.pin_number] & ~port_mask) | options;
}

inline void gpio_toggle(gpio_pin_t pin)
{
    pin.gpio->PTOR |= (1 << pin.pin_number);
}

void gpio_set(gpio_pin_t pin, bool enabled)
{
    if (enabled)
        pin.gpio->PSOR |= (1 << pin.pin_number);
    else
        pin.gpio->PCOR |= (1 << pin.pin_number);
}

inline void gpio_fast_on(gpio_pin_t pin) {
    pin.gpio->PSOR |= (1 << pin.pin_number);
}

inline void gpio_fast_off(gpio_pin_t pin) {
    pin.gpio->PCOR |= (1 << pin.pin_number);
}

bool gpio_get(gpio_pin_t pin)
{
    return (bool)pin.gpio->PDIR & (1 << pin.pin_number);
}

void gpio_attach_interrupt(gpio_pin_t pin, gpio_irq_option_t irq_type, gpio_port_irq_t callback)
{
    pin.port->PCR[pin.pin_number] = (pin.port->PCR[pin.pin_number] & ~port_irq_mask) | irq_type;
    pin.port->PCR[pin.pin_number] |= PORT_PCR_ISF_MASK;
    if (pin.port == PORTA) {
        porta_irq = callback;
        NVIC_EnableIRQ(PORTA_IRQn);
    } else {
        portb_irq = callback;
        NVIC_EnableIRQ(PORTB_IRQn);
    }
}

void PORTA_IRQHandler(void)
{
    if (porta_irq != 0)
        porta_irq();
}

void PORTB_IRQHandler(void)
{
    if (portb_irq != 0)
        portb_irq();
}