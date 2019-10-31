//
//  F: PWM.c
//  P: ReflowOven
//
//  Created by Abad Vera on 10/16/2019
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 10/25/2019
//

#include "PWM.h"

const static uint16_t 		pwmPeriod 	= 10 * 1000; // 10 seconds
const static uint16_t 		SCALING_F	= 128;
const static float 	 		K_P 		= 1.0;
const static float 			K_I 		= 0.0;
const static float 			K_D 		= 0.0;
const static int16_t 		MAX_INT 	= INT16_MAX;
const static int32_t 		MAX_I_T		= (INT32_MAX / 2);

volatile static uint32_t 	pwmOnTime 	= 0;
volatile static uint32_t 	pwmOffTime	= 0;
volatile static bool 		pwmON 		= false;

void setupPWM() {
	// Set as output
	PWM_DIR_REG |= (1 << PWM_PIN);

	// Set pin low
	PWM_PORT &= ~(1 << PWM_PIN);
}

void setPwmON(bool on) {
	if (on) {
		pwmON = true;
	} else {
		pwmON = false;
	}
}

void setPWMDuty(bool pwm, uint16_t currTemp, uint16_t setTemp) {
	if (!pwm) {
		// Keep the Triac on constantly
		pwmOnTime = pwmPeriod;
	} else {
		// TODO: Add PID control
	}
}

void togglePWM() {
	static uint32_t counter = 0;
	static uint32_t pwmTimer = 0;
	static bool on = false;

	if (counter < pwmTimer) {
		counter++;
	} else if (on) {
		pwmTimer = pwmOffTime;
		PWM_PORT &= ~(1 << PWM_PIN);
		counter = 0;
		on = false;
	} else {
		pwmTimer = pwmOnTime;
		PWM_PORT |= ~(1 << PWM_PIN);
		counter = 0;
		on = true;
	}
}