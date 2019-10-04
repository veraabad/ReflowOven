//
//  F: SerialComm.c
//  P: ReflowOven
//
//  Created by Abad Vera on 09/22/19.
//  Copyright © 2019 Abad Vera. All rights reserved.
//	Last Modified: 09/29/2019
//

#include "SerialComm.h"
#include "avr-softuart/softuart.h"
#include "OvenController.h"
#include "Helper.h"

#define CMD_PACKET			2
#define START_POSITION		0
#define LENGTH_PACKET		1
#define ACK_RESP_LEN		4
#define NACK_RESP_LEN		5
#define UINT8_SIZE			sizeof(uint8_t)
#define BIT_MASK_8			0xFF

uint8_t ctr = 0;
uint8_t packetLength = 0;

typedef enum RESPONSE_PACKET_ENUM {
	START_BYTE, LENGTH_BYTE, RESPONSE_TYPE, CMD_BYTE, ERROR_BYTE
} RESPONSE_PACKET;

void writeUART(const uint8_t *data, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		softuart_putchar(data[i]);
	}
}

void sendACK() {
	uint8_t resp[ACK_RESP_LEN];

	resp[START_BYTE] = RO_START_PACKET;
	resp[LENGTH_BYTE] = ACK_RESP_LEN;
	resp[RESPONSE_TYPE] = RO_ACK;
	resp[CMD_BYTE] = rxBuffer[CMD_PACKET];

	writeUART(resp, ACK_RESP_LEN);
}

void sendNACK(uint8_t err) {
	uint8_t resp[NACK_RESP_LEN];

	resp[START_BYTE] = RO_START_PACKET;
	resp[LENGTH_BYTE] = NACK_RESP_LEN;
	resp[RESPONSE_TYPE] = RO_NACK;
	resp[CMD_BYTE] = rxBuffer[CMD_PACKET];
	resp[ERROR_BYTE] = err;

	writeUART(resp, NACK_RESP_LEN);
}

void sendResponse(uint32_t data) {
	// TODO: Send reply
}

void parsePackets() {
	uint8_t response = 0;

	switch(rxBuffer[CMD_PACKET]) {
		case RO_GET_TEMP:
			getValue(TEMP);
		break;
		case RO_SET_TEMP:
			response = setValue(TEMP, rxBuffer[ARRAY_NUMBER(CMD_PACKET)]);
			if (response != 0)
				sendNACK(response);
			else
				sendACK();
		break;
		case RO_GET_TIME:
			getValue(TIME);
		break;
		case RO_SET_TIME:
			response = setValue(TIME, rxBuffer[ARRAY_NUMBER(CMD_PACKET)]);
			if (response != 0)
				sendNACK(response);
			else
				sendACK();
		break;
		case RO_RX_PROFILE:
			receiveTempProfile(rxBuffer + ARRAY_NUMBER(CMD_PACKET),
				packetLength - ARRAY_NUMBER(CMD_PACKET));
			sendACK();
		break;
		case RO_START_OVEN:
			response = ovenStartStop(true);
			if (response != 0)
				sendNACK(response);
			else
				sendACK();
		break;
		case RO_STOP_OVEN:
			response = ovenStartStop(false);
			if (response != 0)
				sendNACK(response);
			else
				sendACK();
		break;
		default:
			sendNACK(UNDEFINED_REQUEST_ERROR);
		break;
	}
}

void serialProcessPackets() {
	while (softuart_kbhit()) {
		uint8_t c = softuart_getchar();
		if ((c == RO_START_PACKET) && (ctr == 0)) {
			// Start of incoming packets
			rxBuffer[0] = c;
			ctr = LENGTH_PACKET;
		} else if (ctr == LENGTH_PACKET) {
			// Receive packetLength
			// This dictates amount of packets to be received
			rxBuffer[ctr] = c;
			packetLength = rxBuffer[ctr];
			ctr = CMD_PACKET;
		} else {
			// Receive the rest of the packets
			rxBuffer[ctr] = c;
			ctr++;
			if (ctr == packetLength) {
				// All packets have been received
				parsePackets();
				ctr = START_POSITION;
				packetLength = START_POSITION;
			}
		}
	}
}

void packetHeader(uint8_t c[], uint8_t len, uint8_t cmd, uint8_t *counter) {
	c[START_BYTE] = RO_START_PACKET;
	c[LENGTH_BYTE] = len;
	c[RESPONSE_TYPE] = cmd;
	*counter += 3;
}

void breakdownLong(long value, uint8_t array[], uint8_t *counter) {
	uint8_t size = sizeof(long) * UINT8_SIZE;
	for (uint8_t i = size; i > 0; i -= UINT8_SIZE) {
		array[*counter++] = (uint8_t)(BIT_MASK_8 & (value >> (size - UINT8_SIZE)));
	}
}

void breakdownUINT16(uint16_t value, uint8_t array[], uint8_t *counter) {
	uint8_t size = sizeof(uint16_t) * UINT8_SIZE;
	for (uint8_t i = size; i > 0; i -= UINT8_SIZE) {
		array[*counter++] = (uint8_t)(BIT_MASK_8 & (value >> (size - UINT8_SIZE)));
	}
}

