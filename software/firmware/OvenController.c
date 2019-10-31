//
//  F: OvenController.c
//  P: ReflowOven
//
//  Created by Abad Vera on 09/26/2019
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 10/16/2019
//

#include "OvenController.h"
#include "MAX31855.h"
#include "SerialComm.h"
#include "avr-softuart/softuart.h"
#include "Timer.h"

volatile static bool 		ovenON 			= false;
volatile static bool 		timerON			= false;
volatile static uint8_t 	profileLen 		= 0;
volatile static uint8_t 	profileCtr 		= 0;
volatile static uint16_t 	ovenTemp 		= 0;
volatile static uint32_t 	ovenTimerStart 	= 0;
volatile static uint32_t 	ovenTimer 		= 0;

void mendType(uint8_t *variable, uint8_t varBytes[], uint8_t len) {
	for (uint8_t i = len, x = 0; i > 0; i -= 1, x++) {
		variable += x;
		*variable = varBytes[i - 1];
	}
}

// TODO: Test function
uint16_t mendUINT16(uint8_t value[]) {
	uint16_t temp = 0;
	mendType((uint8_t*)&temp, value, sizeof(temp));
	return temp;
}

// TODO: Test function
uint32_t mendUINT32(uint8_t value[]) {
	uint32_t temp = 0;
	mendType((uint8_t*)&temp, value, sizeof(temp));
	return temp;
}

void sendTemp() {
	uint8_t ctr = 0;

	// Start SPI
	Setup_MAX31855(0);
	Mx31855_Data mData = readData_MAX31855();

	uint8_t len = sizeof(mData.thermTemp) + sizeof(mData.junctionTemp)
					+ sizeof(mData.error);
	len += HEADER_SIZE;
	packetHeader(txBuffer, len, RO_GET_TEMP, &ctr);
	breakdownLong(mData.thermTemp, txBuffer, &ctr);
	breakdownLong(mData.junctionTemp, txBuffer, &ctr);
	breakdownUINT16(mData.error, txBuffer, &ctr);

	writeUART(txBuffer, len);
}

void sendTime() {
	uint8_t ctr = 0;

	packetHeader(txBuffer, 0, RO_GET_TIME, &ctr);
	breakdownLong(ovenTimer, txBuffer, &ctr);

	txBuffer[1] = ctr + 1;
	writeUART(txBuffer, ctr + 1);
}

void receiveTempProfile(uint8_t profileData[], uint8_t length) {
	uint8_t tempProfileSize = sizeof(uint16_t) + sizeof(uint32_t);
	uint8_t profileStep = length / tempProfileSize;
	uint8_t x = 0;
	for (uint8_t i = 0; i < length; i += profileStep) {
		tempProfile[x].temp = mendUINT16(profileData + i);
		tempProfile[x].time = mendUINT32(profileData + i + 2);
		x++;
	}
	profileLen = x;
}

uint8_t ovenStartStop(bool on) {
	if (on) {
		ovenON = true;
	} else {
		ovenON = false;
		timerON = false;
		profileCtr = 0;
	}
	return 0;
}

void getValue(TEMP_TIME valueType) {
	switch (valueType) {
		case TIME:
			sendTime();
		break;
		case TEMP:
			sendTemp();
		break;
		default:
			// Value type not defined
		break;
	}
}

uint8_t setValue(TEMP_TIME value, uint8_t data[], uint8_t length) {
	switch (value) {
		case TIME:
			if (length == (sizeof(uint32_t))) {
				ovenTimer = mendUINT32(data);
			} else
				return 1;
		break;
		case TEMP:
			if (length == (sizeof(uint16_t))) {
				ovenTemp = mendUINT16(data);
			} else
				return 1;
		break;
		default:
			// No value type to be set
			return 1;
		break;
	}
	return 0;
}

void tempCheck() {
	Setup_MAX31855(0);

	Mx31855_Data tempData = readData_MAX31855();
	if (tempData.thermTemp < (ovenTemp - 5)) {
		// TODO: Keep heater on
	} else if (tempData.thermTemp < ovenTemp) {
		// TODO: Increase PWM
	} else if (tempData.thermTemp > ovenTemp) {
		// TODO: Reduce PWM
	}
}

void checkOven() {
	if (ovenON) {
		if (timerON) {
			tempCheck();
			if ((millis() - ovenTimerStart) > ovenTimer) {
				timerON = false;
			}
		} else if ((profileLen > 0) && (profileCtr < profileLen)) {
			ovenTemp = tempProfile[profileCtr].temp;
			ovenTimer = tempProfile[profileCtr].time;
			timerON = true;
			profileCtr++;
		} else if ((profileLen > 0) && !(profileCtr < profileLen) && !timerON) {
			ovenStartStop(false);
		}
	}
}