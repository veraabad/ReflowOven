//
//  F: OvenController.h
//  P: ReflowOven
//
//  Created by Abad Vera on 09/25/2019
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 10/15/2019
//

#ifndef OvenController_h
#define OvenController_h

#include <stdint.h>
#include <stdbool.h>

typedef enum TEMP_TIME_Enum {
    TIME, TEMP
}TEMP_TIME;

typedef struct TEMP_PROFILE_STRUCT {
	uint16_t temp;
	uint32_t time;
} TEMP_PROFILE;

TEMP_PROFILE tempProfile[5];

void checkOven(void);
void receiveTempProfile(uint8_t profileData[], uint8_t length);
uint8_t ovenStartStop(bool on);
void getValue(TEMP_TIME value);
uint8_t setValue(TEMP_TIME value, uint8_t data[], uint8_t length);

#endif