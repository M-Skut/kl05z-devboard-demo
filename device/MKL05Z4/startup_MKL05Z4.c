/******************************************************************************
 * @file     startup_MKL05Z4.c
 * @brief    CMSIS-Core(M) Device Startup File for
 *           Device MKL05Z4
 * @version  V1.0.1
 * @date     27. October 2021
 ******************************************************************************/
/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MKL05Z4.h"

/*---------------------------------------------------------------------------
  External References
 *---------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

extern __NO_RETURN void __PROGRAM_START(void);

/*---------------------------------------------------------------------------
  Internal References
 *---------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
void Default_Handler(void);

/*---------------------------------------------------------------------------
  Exception / Interrupt Handler
 *---------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak));
void MemManage_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void SecureFault_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void FTFA_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void LVD_LVW_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void LLW_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void CMP0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void TPM0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void TPM1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Seconds_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void PIT_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void DAC0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void TSI0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void MCG_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTimer_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void PORTA_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PORTB_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));


/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE[48];
       const VECTOR_TABLE_Type __VECTOR_TABLE[48] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP),  /*     Initial Stack Pointer */
  Reset_Handler,                       /*     Reset Handler */
  NMI_Handler,                         /* -14 NMI Handler */
  HardFault_Handler,                   /* -13 Hard Fault Handler */
  MemManage_Handler,                   /* -12 MPU Fault Handler */
  BusFault_Handler,                    /* -11 Bus Fault Handler */
  UsageFault_Handler,                  /* -10 Usage Fault Handler */
  SecureFault_Handler,                 /*  -9 Secure Fault Handler */
  0,                                   /*     Reserved */
  0,                                   /*     Reserved */
  0,                                   /*     Reserved */
  SVC_Handler,                         /*  -5 SVCall Handler */
  DebugMon_Handler,                    /*  -4 Debug Monitor Handler */
  0,                                   /*     Reserved */
  PendSV_Handler,                      /*  -2 PendSV Handler */
  SysTick_Handler,                     /*  -1 SysTick Handler */
  DMA0_IRQHandler,                     /*   0 DMA channel 0 transfer complete and error */
  DMA1_IRQHandler,                     /*   1 DMA channel 1 transfer complete and error */
  DMA2_IRQHandler,                     /*   2 DMA channel 2 transfer complete and error */
  DMA3_IRQHandler,                     /*   3 DMA channel 3 transfer complete and error */
  0,                                   /*     Reserved */
  FTFA_IRQHandler,                     /*   5 Command complete and read collision */
  LVD_LVW_IRQHandler,                  /*   6 Low-voltage detect, low-voltage warning */
  LLW_IRQHandler,                      /*   7 Low Leakage Wakeup */
  I2C0_IRQHandler,                     /*   8 I2C0 Interrupt */
  0,                                   /*     Reserved */
  SPI0_IRQHandler,                     /*  10 SPI Single interrupt vector for all sources */
  0,                                   /*     Reserved */
  UART0_IRQHandler,                    /*  12 UART0 Status and error */
  0,                                   /*     Reserved */
  0,                                   /*     Reserved */
  ADC0_IRQHandler,                     /*  15 ADC0 Interrupt */
  CMP0_IRQHandler,                     /*  16 CMP0 Interrupt */
  TPM0_IRQHandler,                     /*  17 TPM0 Interrupt */
  TPM1_IRQHandler,                     /*  18 TPM1 Interrupt */
  0,                                   /*     Reserved */
  RTC_IRQHandler,                      /*  20 RTC Alarm interrupt */
  RTC_Seconds_IRQHandler,              /*  21 RTC Seconds interrupt */
  PIT_IRQHandler,                      /*  22 PIT Single interrupt vector for all channels */
  0,                                   /*     Reserved */
  0,                                   /*     Reserved */
  DAC0_IRQHandler,                     /*  25 DAC0 Interrupt */
  TSI0_IRQHandler,                     /*  26 TSI0 Interrupt */
  MCG_IRQHandler,                      /*  27 MCG Interrupt */
  LPTimer_IRQHandler,                  /*  28 LPTMR0 Interrupt */
  0,                                   /*     Reserved */
  PORTA_IRQHandler,                    /*  30 Pin detect (Port A) */
  PORTB_IRQHandler                     /*  31 Pin detect (Port B) */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*---------------------------------------------------------------------------
  Reset Handler called on controller reset
 *---------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void)
{
  __set_PSP((uint32_t)(&__INITIAL_SP));
  SystemInit();                    /* CMSIS System Initialization */
  __PROGRAM_START();               /* Enter PreMain (C library entry point) */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*---------------------------------------------------------------------------
  Hard Fault Handler
 *---------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
  while(1);
}

/*---------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *---------------------------------------------------------------------------*/
void Default_Handler(void)
{
  while(1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif
