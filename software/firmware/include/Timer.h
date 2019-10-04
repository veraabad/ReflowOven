//
//  F: Timer.h
//  P: ReflowOven
//
//  Created by Abad Vera on 09/30/2019
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 09/30/2019
//

#ifndef Timer_h
#define Timer_h

#include <stdint.h>

// Start time function
void timerInit(void);
uint64_t micros(void);
uint64_t millis(void);

#endif