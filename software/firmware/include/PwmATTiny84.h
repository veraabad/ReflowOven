//
//  PwmATTiny84.h
//  ReflowOven
//
//  Created by Abad Vera on 2/01/17.
//  Copyright © 2017 Abad Vera. All rights reserved.
//

#ifndef PwmATTiny84_h
#define PwmATTiny84_h

//#include <avr/io.h>

#define PreScaleNone	(1<<CS10)
#define PreScale8		(1<<CS11)
#define PreScale64		((1<<CS11) | (1<<CS10))
#define PreScale256		(1<<CS12)
#define PreScale1024	((1<<CS12) | (1<<CS10))
#define PWM_Off			((1<<CS12) | (1<<CS11) | (1<<CS10))

// If using Timer/Counter 1
#define MaxTopValue	0xFFFF
// If using Timer/Counter 0
//#define MaxTopValue	0xFF

// Use this for Timer/Counter 1 in order to 
// avoid division when calculating duty cycle
#define PercentRatio 	655

typedef enum PWM_MODE_ENUM {
	FAST, PHASE_CORRECT, PASE_FREQ_CORRECT
} PWM_MODE;

// Take an array of ascii characters and turn them into their integer equivalent
void setupPWM(PWM_MODE pwmMode, uint8_t dutyCycle);
void setDutyCycle(PWM_MODE pwmMode, uint8_t dutyCycle);

#endif /* Helper_h */
