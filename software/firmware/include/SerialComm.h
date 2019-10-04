//
//  F: SerialComm.h
//  P: ReflowOven
//
//  Created by Abad Vera on 9/22/19.
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 09/30/2019
//

#ifndef SerialComm_h
#define SerialComm_h

#include <stdint.h>

#define HEADER_SIZE		3

typedef enum SERIAL_PACKETS_ENUM {
	RO_GET_TEMP = 1,
	RO_SET_TEMP, RO_GET_TIME, RO_SET_TIME, RO_RX_PROFILE,
	RO_START_OVEN, RO_STOP_OVEN, RO_ACK, RO_NACK,
	RO_START_PACKET = 0xF0
} SERIAL_PACKETS;

typedef enum ERROR_RESPONSE_ENUM {
	TEMP_ERROR, THERMOCOUPLE_ERROR, SHORT_ERROR, UNDEFINED_REQUEST_ERROR
} ERROR_RESPONSE;

uint8_t rxBuffer[255];
uint8_t txBuffer[50];

void serialProcessPackets(void);
void writeUART(const uint8_t *data, uint16_t length);
void packetHeader(uint8_t c[], uint8_t len, uint8_t cmd, uint8_t *counter);
void breakdownLong(long value, uint8_t array[], uint8_t *counter);
void breakdownUINT16(uint16_t value, uint8_t array[], uint8_t *counter);

#endif