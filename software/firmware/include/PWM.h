//
//  F: PWM.h
//  P: ReflowOven
//
//  Created by Abad Vera on 10/16/2019
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 10/16/2019
//

#ifndef PWM_h
#define PWM_h

#include <avr/io.h>
#include <stdbool.h>

#define PWM_PORT 	PORTA
#define PWM_PIN		PA1

void pwmON(bool on);
void setPWMDuty(int8_t dutyCycle);

#endif