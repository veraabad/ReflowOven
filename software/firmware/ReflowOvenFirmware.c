//
//  F: ReflowOvenFirmware.c
//  P: ReflowOven
//
//  Created by Abad Vera on 3/4/16.
//  Copyright © 2016 Abad Vera. All rights reserved.
//	Last Modified: 10/10/2019
//

// I'm utilizing an ATtiny84 so here's a pinout
//
//       VCC - |1      14| - GND
//       PB0 - |2      13| - PA0
//       PB1 - |3      12| - PA1(PWM)
//       PB3 - |4      11| - PA2(RX1)
//       PB2 - |5      10| - PA3(TX1)
//   (CS)PA7 - |6       9| - PA4(SCK)
// (MOSI)PA6 - |7       8| - PA5(MISO)
//
// TC0 - SW UART
// TC1 - Timer

/**** CLK Speed = 16MHz ****/

#include "ReflowOvenFirmware.h"
#include "MAX31855.h"
#include "avr-softuart/softuart.h"
#include "Timer.h"
#include "SerialComm.h"

uint64_t startTime = 0;

void testing() {
	if ((millis() - startTime) > 5000UL) {
		PORTA ^= (1 << PA0);
		startTime = millis();
		static uint8_t ctr = 0;

		// Start SPI
		Setup_MAX31855(1);
		Mx31855_Data data = readData_MAX31855();
		uint8_t *rawData = getData();

		uint8_t len = sizeof(data.thermTemp) + sizeof(data.junctionTemp)
					+ sizeof(data.error);
		len += HEADER_SIZE;
		txBuffer[ctr++] = (uint8_t)(*"\n");
		txBuffer[ctr++] = (uint8_t)(*"H");
		txBuffer[ctr++] = (uint8_t)(*":");
		packetHeader(txBuffer, len, RO_GET_TEMP, &ctr);
		txBuffer[ctr++] = (uint8_t)(*"\n");
		txBuffer[ctr++] = (uint8_t)(*"D");
		txBuffer[ctr++] = (uint8_t)(*":");
		txBuffer[ctr++] = rawData[0];
		txBuffer[ctr++] = rawData[1];
		txBuffer[ctr++] = rawData[2];
		txBuffer[ctr++] = rawData[3];
		txBuffer[ctr++] = (uint8_t)(*"\n");
		txBuffer[ctr++] = (uint8_t)(*"T");
		txBuffer[ctr++] = (uint8_t)(*":");
		breakdownLong(data.thermTemp, txBuffer, &ctr);
		txBuffer[ctr++] = (uint8_t)(*"\n");
		txBuffer[ctr++] = (uint8_t)(*"J");
		txBuffer[ctr++] = (uint8_t)(*":");
		breakdownLong(data.junctionTemp, txBuffer, &ctr);
		txBuffer[ctr++] = (uint8_t)(*"\n");
		txBuffer[ctr++] = (uint8_t)(*"E");
		txBuffer[ctr++] = (uint8_t)(*":");
		if (sizeof(data.error) > 1) {
			breakdownUINT16(data.error, txBuffer, &ctr);
		} else {
			txBuffer[ctr++] = (uint8_t)(data.error);
		}

		txBuffer[ctr++] = (uint8_t)(*"\n");
		txBuffer[ctr] = ctr;
		ctr += 1;

		writeUART(txBuffer, ctr);
		ctr = 0;
		// softuart_puts_P("\r\n***Test2***\r\n");
	}
}

void loop() {
	// serialProcessPackets();
	testing();
}

void setup() {
	// Init Timer
	timerInit();

	// Initialize the UART
	softuart_init();

	// Setup to recognize interrupts
	sei();

	// Setup PA0 as output
	DDRA |= (1 << PA0);

	softuart_puts_P( "\r\nReflow Oven Project -- Testing\r\n");

	startTime = millis();
}

int main() {
	setup();

	while(1) {
		loop();
	}
}
