#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdint.h>
#include "MKL05Z4.h"

void display_error() {

    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; // Enable clock for GPIO B
    PORTB->PCR[8] |= PORT_PCR_MUX(1);   // Set Pin 8 MUX as GPIO
    PTB->PDDR |= (1 << 8);              // Set LED pins as output
    PTB->PCOR |= (1 << 8);              // Turn the RED LED on
    while (1)                           // Block program execution
        __NOP();
}

#endif
