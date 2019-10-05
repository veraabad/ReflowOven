//
//  F: Timer.c
//  P: ReflowOven
//
//  Created by Abad Vera on 09/28/2019
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 10/04/2019
//

#include "Timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define MICROS 					1000000UL
#define MILLIS 					1000U

#define TIMER_T_CONTR_REGA		TCCR1A
#define TIMER_T_CONTR_REGB		TCCR1B
#define TIMER_T_CONTR_REGC		TCCR1C
#define TIMER_T_COMP_REGL		OCR1AL
#define TIMER_T_COMP_REGH		OCR1AH
#define TIMER_T_CNT_REGL		TCNT1L
#define TIMER_T_CNT_REGH		TCNT1H
#define TIMER_T_INTCTL_REG		TIMSK1
#define TIMER_T_COMP_VECT		TIM1_COMPA_vect
#define TIMER_10_MICROS			(MICROS/10)
#define TIMER_PRESCALE			8
#define TIMER_STEP				(MICROS/TIMER_10_MICROS)
// CLK/8 prescaling
#define TIMER_PRESC_MASK		(1 << CS11)
#define TIMER_CMP_INT_EN_MASK	(1 << OCIE1A)

#define TIMER_MASKA				0
#define TIMER_MASKB				((1 << WGM12) | TIMER_PRESC_MASK)
#define TIMER_MASKC				0
#define TIMER_INIT_CNT_MASK		0
#define BIT_8_MASK				0x00FF
// Define F_CPU in Makefile
#define TIMER_TOP 				(F_CPU/TIMER_PRESCALE/TIMER_10_MICROS - 1)

volatile static uint64_t microsValue = 0;

ISR(TIMER_T_COMP_VECT) {
	// Store SREG
	unsigned char sreg = SREG;

	// Increment by 10 since we are
	// triggering every 10 microseconds
	microsValue += TIMER_STEP;

	// Restore SREG
	SREG = sreg;
}

void timerInit(void) {
	// Store SREG
	unsigned char sreg = SREG;
	// Stop interrupts meanwhile we set the timer registers
	cli();

	// Set TOP of timer
	TIMER_T_COMP_REGL = (uint8_t)(TIMER_TOP & BIT_8_MASK);
	TIMER_T_COMP_REGH = (uint8_t)((TIMER_TOP >> 8) & BIT_8_MASK);

	// Set Timer control registers
	TIMER_T_CONTR_REGA = TIMER_MASKA;
	TIMER_T_CONTR_REGB = (uint8_t)((1 << WGM12) | (1 << CS11));
	TIMER_T_CONTR_REGC = TIMER_MASKC;

	// Enable compare match interrupt
	TIMER_T_INTCTL_REG = 0;
	TIMER_T_INTCTL_REG |= TIMER_CMP_INT_EN_MASK;

	// Clear Timer counter
	TIMER_T_CNT_REGL = TIMER_INIT_CNT_MASK;
	TIMER_T_CNT_REGH = TIMER_INIT_CNT_MASK;

	// Restore SREG
	SREG = sreg;
}

uint64_t micros(void) {
	return microsValue;
}

uint64_t millis(void) {
	return (uint64_t)(microsValue / 1000U);
}
