//
//  PwmATTiny84.c
//  ReflowOven
//
//  Created by Abad Vera on 2/01/17.
//  Copyright © 2017 Abad Vera. All rights reserved.
//

/* 
	This File contains PWM functions to be used with 
	ATTiny24/44/84
*/

// OC1A/B will not be used due to these pins being used for SPI communication

#include "PwmATTiny84.h"

void setupPWM(PWM_MODE pwmMode, uint8_t dutyCycle) {
	switch (pwmMode) {
		case FAST:
			// Setup for Fast PWM mode
			TCCR1A &= (1<<WGM11) | (1<<WGM10);
			TCCR1B &= (1<<WGM13) | (1<<WGM12);
			break;

		case PHASE_CORRECT:
			break;

		case PHASE_FREQ_CORRECT:
			break;

		default:
			break;
	}
	
	// All PWM modes need to have Output Compare A Interrupt Enabled
	// and also Overflow Interrupt Enabled
	TIMSK1 &= (1<<OCIE1A) | (1<<TOIE1);

	setDutyCycle(pwmMode, dutyCycle);
}

void setDutyCycle(PWM_MODE pwmMode, uint8_t dutyCycle) {
	unsigned int topCount;
	if (dutyCycle == 100) {
		topCount = MaxTopValue;
	} else {
		topCount = dutyCycle * PercentRatio;
	}
	
	unsigned char sreg;

	sreg = SREG; // Saving global interrupt flag
	_CLI(); // Now disabling interrupts in order to update the registers

	switch (pwmMode) {
		case FAST || PHASE_CORRECT:
			OCR1A = topCount;
			break;

		case PHASE_FREQ_CORRECT:
			break;

		default:
			break;
	}

	SREG = sreg; // Restoring global interrupt flag
}

// Set prescaler to start up clock
void startPWMClock() {
	TCCR1B |= PreScale256;
}

void stopPWMClock() {
	TCCR1B &= ~(PWM_Off);
}

/* Interrupt functions */

// Interrupt called when there is a compare match with OCR1A
ISR(TIM1_COMPA_vect) {

}

// Interrupt called when there is a timer overflow
ISR_Two(TIM1_OVF_vect) {

}