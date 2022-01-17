#ifndef DELAY_H_INCLUDE
#define DELAY_H_INCLUDE

#include <stdint.h>

/**
 * @brief Init SysTick timer for delay functions.
 * This function is only needed if SysTickConfig is
 * never called in the application. 
 */
void delay_init(void);

/**
 * @brief  Delay by the provided number of system ticks.
 * Any values between 0 and 0x0ffffffff are allowed.
 */
void delay_system_ticks(uint32_t sys_ticks);

/**
 * @brief Delay by the provided number of micro seconds.
 * Limitation: "us" * System-Freq in MHz must now overflow in 32 bit.
 * Values between 0 and 1.000.000 (1 second) are ok.
 * Important: Call SystemCoreClockUpdate() before calling this function.
 */
void delay_micro_seconds(uint32_t us);

#endif
