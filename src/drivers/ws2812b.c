#include "ws2812b.h"
#include "gpio.h"
#include <stdint.h>

/**
 * This driver is a simple bit-bang type
 * that has been tested with only one clock configuration
 * and currently is impossible to change it on-the-fly.
 * Only GCC supported
 * Based on: https://github.com/cpldcpu/light_ws2812/blob/master/light_ws2812_ARM/light_ws2812_cortex.c
 */

void ws2812b_init(gpio_pin_t pin)
{
    gpio_pin_init(pin, GPIO_OUTPUT, GPIO_MUX_ALT1 | GPIO_PULL_DOWN | GPIO_PULL_ENABLE);
}

#define F_CPU 47972352u
#define ws2812_ctot (((F_CPU / 1000) * 1250) / 1000000)
#define ws2812_t1 (((F_CPU / 1000) * 375) / 1000000) // floor
#define ws2812_t2 (((F_CPU / 1000) * 625 + 500000) / 1000000) // ceil

#define w1 (ws2812_t1 - 2)
#define w2 (ws2812_t2 - ws2812_t1 - 2)
#define w3 (ws2812_ctot - ws2812_t2 - 5)

#define ws2812_DEL1 "	nop		\n\t"
#define ws2812_DEL2 "	b	.+2	\n\t"
#define ws2812_DEL4 ws2812_DEL2 ws2812_DEL2
#define ws2812_DEL8 ws2812_DEL4 ws2812_DEL4
#define ws2812_DEL16 ws2812_DEL8 ws2812_DEL8

void ws2812b_write(gpio_pin_t pin, uint8_t* data, uint32_t length)
{
    #if defined(__GNUC__) && !defined(__clang__)
    uint32_t pinmask = (1 << pin.pin_number);
    volatile uint32_t* port_set = &pin.gpio->PSOR;
    volatile uint32_t* port_clr = &pin.gpio->PCOR;
    volatile uint32_t  i;
    uint32_t curr_byte;
    __disable_irq();
    // ARM CM0+ Magic
	while (length--) {
		curr_byte=*data++;
    	__asm__ volatile(
			"		lsl %[dat],#24				\n\t"
			"		movs %[ctr],#8				\n\t"
			"ilop%=:							\n\t"

			"		lsl %[dat], #1				\n\t"
			"		str %[pinmask], [%[set]]	\n\t"
#if (w1&1)
			ws2812_DEL1
#endif
#if (w1&2)
			ws2812_DEL2
#endif
#if (w1&4)
			ws2812_DEL4
#endif
#if (w1&8)
			ws2812_DEL8
#endif
#if (w1&16)
			ws2812_DEL16
#endif
			"		bcs one%=					\n\t"
			"		str %[pinmask], [%[clr]]	\n\t"
			"one%=:								\n\t"
#if (w2&1)
			ws2812_DEL1
#endif
#if (w2&2)
			ws2812_DEL2
#endif
#if (w2&4)
			ws2812_DEL4
#endif
#if (w2&8)
			ws2812_DEL8
#endif
#if (w2&16)
			ws2812_DEL16
#endif

			"		sub %[ctr], #1				\n\t"
			"		str %[pinmask], [%[clr]]	\n\t"
			"		beq	end%=					\n\t"
#if (w3&1)
			ws2812_DEL1
#endif
#if (w3&2)
			ws2812_DEL2
#endif
#if (w3&4)
			ws2812_DEL4
#endif
#if (w3&8)
			ws2812_DEL8
#endif
#if (w3&16)
			ws2812_DEL16
#endif
			"		b 	ilop%=					\n\t"
			"end%=:								\n\t"
			:	[ctr] "+r" (i)
			:	[dat] "r" (curr_byte), [set] "r" (port_set), [clr] "r" (port_clr), [pinmask] "r" (pinmask)
			);
    }
    __enable_irq();
    // Issure reset
    for (uint8_t r_time = 0; r_time < 192; r_time++)
        __NOP();
#endif
}