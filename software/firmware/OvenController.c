//
//  F: OvenController.c
//  P: ReflowOven
//
//  Created by Abad Vera on 09/26/2019
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 09/29/2019
//

#include "OvenController.h"
#include "MAX31855.h"
#include "SerialComm.h"

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

void receiveTempProfile(uint8_t profileData[], uint8_t length) {
	// TODO: Save temperature profile
}

uint8_t ovenStartStop(bool on) {
	// TODO: Start oven and timers
	// TODO: Turn off oven and timers
	return 0;
}

void getValue(TEMP_TIME valueType) {
	switch (valueType) {
		case TIME:
		// TODO: get time
		break;
		case TEMP:
			sendTemp();
		break;
		default:
			// Value Type is not defined
		break;
	}
}

uint8_t setValue(TEMP_TIME value, uint8_t data) {
	// TODO: Set value
	return 0;
}