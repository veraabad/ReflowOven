//
//  F: MAX31855.h
//  P: ReflowOven
//
//  Created by Abad Vera on 3/23/16.
//  Copyright © 2016 Abad Vera. All rights reserved.
//	Last Modified: 10/10/2019
//

// Code tested on ATtiny84
// Uses hardware SPI and clock at 4MHz which is clkCPU/2

#ifndef MAX31855_h
#define MAX31855_h

#include <avr/io.h>

#define SPI_CS          PA7
#define SPI_SCK         PA4
#define SPI_MISO        PA6         // MOSI is not used for communication with MAX31855

enum ErrorCode {
    THERM_OPEN, THERM_SHORT_GND, THERM_SHORT_VCC, NO_COMM, NO_ERROR
};

typedef struct Mx31855Data_struct {
    long thermTemp;
    long junctionTemp;
    enum ErrorCode error;
}Mx31855_Data;

// Using hardware SPI, only need to define if CS is active low or active high
void Setup_MAX31855(uint8_t csActiveLow);

// Returns all info provided by the MAX31855 including any error codes
Mx31855_Data readData_MAX31855();

// Toggle spi clk
void toggleSpiCLK();

uint8_t* getData();

#endif /* MAX31855_h */
