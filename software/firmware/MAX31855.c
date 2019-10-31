//
//  F: MAX31855.c
//  P: ReflowOven
//
//	Created by Abad Vera on 03/23/2016.
//	Copyright © 2016 Abad Vera. All rights reserved.
//	Last Modified: 10/12/2019
//

#include "MAX31855.h"
#include <stdbool.h>
#include <avr/interrupt.h>
#include "Timer.h"

#define USI_OUT_REG     PORTA			// USI port output register
#define USI_DIR_REG     DDRA			// USI port direction register
#define CLK_FREQ		2500UL			// 2.5KHz
#define SPI_MASTER      (1<<USIWM0)		// Set USI to 3-wire mode
#define SPI_MODE0       (1<<USICS1) 	// Sample on negative edge

#define SHIFT_BITS		8
#define PACKET_BITS 	32
#define PACKET_BYTES	(PACKET_BITS/8)
#define IDENTIFIER_MASK	0x20008UL
#define FAULT_BIT_MASK	0x10000UL
#define ERROR_BIT_MASK	0x07
#define BIT_MASK_14		0x3FFFU
#define BIT_MASK_12		0x00000FFFUL
#define THERM_SIGN_BIT	0x2000U
#define JUNCT_SIGN_BIT	0x800U
#define THERM_LSB		0x1FFFU
#define JUNCT_LSB		0x7FFU

const static uint8_t	NON_DATA_BITS 		= 18;
const static float 		THERM_TO_CELSIUS	= 0.25;
const static float 		JUNCT_TO_CELSIUS	= 0.0625;

static uint32_t data = 0; // Hold raw data
volatile static uint8_t numberOfBytes = 0; // Holds value of number of bytes to fetch
volatile static char done = 0; // True if fetch is done
static uint8_t dataArray[4];

void Setup_MAX31855(uint8_t csActiveLow) {
	USI_DIR_REG |= (1<<SPI_SCK) | (1<<SPI_CS); // Set outputs
	USI_DIR_REG &= ~(1<<SPI_MISO); // Set input
	USI_OUT_REG |= (1<<SPI_MISO); // Needed only if no external pullup

	// Configure USI for 3-wire mode (SPI mode), set to SPI master mode 0. Also configured to use
	// software clock strobe
	USICR = (1<<USIOIE) | SPI_MASTER | SPI_MODE0 | (1<<USICLK);

	// Clear any flags and counter
	// USISR = 0;

	// Set how often clk should trigger
	setSpiClkValue((uint16_t)(CLK_FREQ * 2));

	// Set CS pin in idle mode
	if (csActiveLow) {
		USI_OUT_REG |= (1<<SPI_CS);
	}
	else {
		USI_OUT_REG &= ~(1<<SPI_CS);
	}
}

// Gets called from TIM1_COMPA vector
void toggleSpiCLK() {
	USICR |= (1<<USITC);
}

// USI overflow interrupt.
// Stop software clock strobe and collect data on USIBR
ISR(USI_OVF_vect) {
	// Disable SPI clock meanwhile we copy data
	stopSpiClk();
	// Clear overflow flag
	USISR = (1<<USIOIF);

	// Obtain data from USI data buffer
	dataArray[numberOfBytes] = USIDR;
	data <<= 8;
	data |= USIDR;

	// Re-start SPI CLK if we haven't finished fetching enough bytes
	if (numberOfBytes > 0) {
		numberOfBytes--;
		startSpiClk();
	}
	else {
		stopSpiClk();
		done = 1;
	}
}

long getNegativeTemp(long tempData, long lsb) {
	tempData = ~tempData;
	tempData &= lsb; // Re-obtain 13 or 11 LSB
	tempData += 1;
	return tempData * -1; // Return negative temp value
}

uint8_t* getData() {
	return dataArray;
}

// Parses data and sends back info
Mx31855_Data readData_MAX31855() {
	// Holds info that will be relayed to main program
	Mx31855_Data response;
	response.error = NO_ERROR;
	uint32_t dataBuffer = 0;

	// Activate CS
	USI_OUT_REG ^= (1<<SPI_CS);


	// We need 4 Bytes of data
	numberOfBytes = (PACKET_BYTES - 1);
	// TODO: remove following line
	// numberOfBytes = 1;
	data = 0;
	startSpiClk();

	do {} while(done == 0); // Wait until transfer is complete
	// De-activate CS
	USI_OUT_REG ^= (1<<SPI_CS);
	dataBuffer = data;

	// Check to make sure we are communicating with a MAX31855
	// Bits 3 and 17 must be zero and there should be at least one bit high
	uint8_t bitCount = 0;
	for (uint8_t i = 0; i < PACKET_BITS; i++) {
		if (dataBuffer & (1<<(i)))
			bitCount++;
	}

	if ((bitCount <= 0) || (dataBuffer & IDENTIFIER_MASK))
		response.error = NO_COMM;

	// Check for faults
	// If fault bit found then find out what type of fault
	if (dataBuffer & FAULT_BIT_MASK) {
		switch (dataBuffer & ERROR_BIT_MASK) {
			case 0x01:
				response.error = THERM_OPEN;
				break;

			case 0x02:
				response.error = THERM_SHORT_GND;
				break;

			case 0x04:
				response.error = THERM_SHORT_VCC;
				break;

			default:
				break;
		}
	}

	// Strip away other bits and leave only thermocouple temp data
	long thTemp = dataBuffer >> NON_DATA_BITS;
	thTemp &= BIT_MASK_14; // Make sure its only thermocouple temp data
	long jctTemp = dataBuffer >> 4; // Strip away fault bits
	jctTemp &= BIT_MASK_12; // Remove thermocouple temp data

	// Sign bit for thermocouple temp data is Bit14
	// Check if its a negative temp. If so then get the negative value
	if (thTemp & THERM_SIGN_BIT) {
		thTemp = getNegativeTemp(thTemp, THERM_LSB);
	}

	response.thermTemp = (long)(thTemp * THERM_TO_CELSIUS); // Convert to Celsius

	// Sign bit for junction temp data is Bit12
	// Check if its a negative temp. If so then get the negative value
	if (jctTemp & JUNCT_SIGN_BIT) {
		jctTemp = getNegativeTemp(jctTemp, JUNCT_LSB);
	}

	response.junctionTemp = (long)(jctTemp * JUNCT_TO_CELSIUS); // Convert to Celsius

	// Reset done to 0
	done = 0;
	return response;
}

