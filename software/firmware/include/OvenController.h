//
//  F: OvenController.h
//  P: ReflowOven
//
//  Created by Abad Vera on 09/25/2019
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 09/29/2019
//

#ifndef OvenController_h
#define OvenController_h

#include <stdint.h>
#include <stdbool.h>

typedef enum TEMP_TIME_Enum {
    TIME, TEMP
}TEMP_TIME;

void receiveTempProfile(uint8_t profileData[], uint8_t length);
uint8_t ovenStartStop(bool on);
void getValue(TEMP_TIME value);
uint8_t setValue(TEMP_TIME value, uint8_t data);

#endif