//
//  MAX31855.h
//  ReflowOven
//
//  Created by Abad Vera on 3/23/16.
//  Copyright © 2016 Abad Vera. All rights reserved.
//

// Code tested on ATtiny84
// Uses hardware SPI and clock at 4MHz which is clkCPU/2

#ifndef MAX31855_h
#define MAX31855_h

#include <avr/io.h>

#define CS_PIN      PA7
#define SCK_PIN         PA4
#define MISO_PIN        PA5         // MOSI is not used for communication with MAX31855
#define CLOCK_SEL       (1<<CS00)   // No need for prescaling
#define TC0_COMPARE_VAL 1           // Need a value of 1 to be 4MHz
#define SPI_MASTER      (1<<USIWM0) // Set USI to 3-wire mode
#define SPI_MODE0       (1<<USICS1) // Sample on positive edge

enum ErrorCode {
    THERM_OPEN, THERM_SHORT_GND, THERM_SHORT_VCC, NO_COMM, NONE
};

typedef struct Mx31855Data_struct {
    double thermTemp;
    double junctionTemp;
    enum ErrorCode error;
}Mx31855_Data;

void Setup_MAX31855(uint8_t CS); // Using hardware SPI, only need to define CS or slave select
Mx31855_Data readData(); // Returns all info provided by the MAX31855 including any error codes


#endif /* MAX31855_h */
