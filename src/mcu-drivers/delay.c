#include "MKL05Z4.h"
#include <stdint.h>

/*  Generic ARM delay procedures, based on the system timer (SysTick) 
    The delay function delay_micro_seconds() will use the global variable
    SystemCoreClock. A call to SystemCoreClockUpdate() is required before
    using delay_micro_seconds().

    Copyright (c) 2016, olikraus@gmail.com
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
    are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
    CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
*/

void delay_init(void)
{
    /* 
      Set reload register to 1ms value (can by any other, set just to start Systick)
      Can be changed by calling SysTickConfig.
    */
    SysTick->LOAD = (uint32_t)(SystemCoreClock / 1000);
    /* Load the SysTick Counter Value */
    SysTick->VAL = 0UL;
    /* Enable SysTick but do not enable interrupts */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

/*
  Delay by the provided number of system ticks.
  The delay must be smaller than the RELOAD value.
  This delay has an imprecision of about +/- 20 system ticks.   
*/
static void delay_system_ticks_sub(uint32_t sys_ticks)
{
    uint32_t start_val, end_val, curr_val;
    uint32_t load;

    start_val = SysTick->VAL;
    start_val &= 0x0ffffffUL;
    end_val = start_val;

    if (end_val < sys_ticks) {
        /* check, if the operation after this if clause would lead to a negative result */
        /* if this would be the case, then add the reload value first */
        load = SysTick->LOAD;
        load &= 0x0ffffffUL;
        end_val += load;
    }
    /* counter goes towards zero, so end_val is below start value */
    end_val -= sys_ticks;

    /* wait until interval is left */
    if (start_val >= end_val) {
        for (;;) {
            curr_val = SysTick->VAL;
            curr_val &= 0x0ffffffUL;
            if (curr_val <= end_val)
                break;
            if (curr_val > start_val)
                break;
        }
    } else {
        for (;;) {
            curr_val = SysTick->VAL;
            curr_val &= 0x0ffffffUL;
            if (curr_val <= end_val && curr_val > start_val)
                break;
        }
    }
}

/*
  Delay by the provided number of system ticks.
  Any values between 0 and 0x0ffffffff are allowed.
*/
void delay_system_ticks(uint32_t sys_ticks)
{
    uint32_t load4;
    load4 = SysTick->LOAD;
    load4 &= 0x0ffffffUL;
    load4 >>= 2;

    while (sys_ticks > load4) {
        sys_ticks -= load4;
        delay_system_ticks_sub(load4);
    }
    delay_system_ticks_sub(sys_ticks);
}

/*
  Delay by the provided number of micro seconds.
  Limitation: "us" * System-Freq in MHz must now overflow in 32 bit.
  Values between 0 and 1.000.000 (1 second) are ok.
*/
void delay_micro_seconds(uint32_t us)
{
    uint32_t sys_ticks = (uint32_t)((uint64_t)us * SystemCoreClock / 1000000);
    delay_system_ticks(sys_ticks);
}
