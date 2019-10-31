//
//  F: PWM.h
//  P: ReflowOven
//
//  Created by Abad Vera on 10/16/2019
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 10/25/2019
//

#ifndef PWM_h
#define PWM_h

#include <avr/io.h>
#include <stdbool.h>

#define PWM_PORT 	PORTA
#define PWM_PIN		PA1
#define PWM_DIR_REG	DDRA

typedef struct PID_DATA_STRUCT {
	int16_t lastValue;
	int32_t sumError;
	int16_t p;
	int16_t i;
	int16_t d;
	int16_t maxError;
	int32_t maxSumError;
}PID_DATA;

void setupPWM(void);
void setPwmON(bool on);
void setPWMDuty(bool pwm, uint16_t currTemp, uint16_t setTemp);
void togglePWM();

#endif