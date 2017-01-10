//
//  MAX31855.c
//  ReflowOven
//
//  Created by Abad Vera on 3/23/16.
//  Copyright © 2016 Abad Vera. All rights reserved.
//

#include "MAX31855.h"

#define USI_OUT_REG     PORTA       // USI port output register
#define USI_DIR_REG     DDRA        // USI port direction register
#define CLOCK_SEL       (1<<CS00)   // No need for prescaling
#define TC0_COMPARE_VAL 1           // Need a value of 1 to be 4MHz
#define SPI_MASTER      (1<<USIWM0) // Set USI to 3-wire mode
#define SPI_MODE0       (1<<USICS1) // Sample on positive edge
#define CTC_TC0         (1<<WGM01)  // Enable clear timer on compare match on timer/counter 0

unsigned long data = 0; // Hold raw data
uint8_t numberOfBytes = 0; // Holds value of number of bytes to fetch
char done = 0; // True if fetch is done

void Setup_MAX31855(uint8_t csActiveLow) {
    USI_DIR_REG |= (1<<SCK_PIN) | (1<<CS_PIN); // Set outputs
    USI_DIR_REG &= ~(1<<MISO_PIN); // Set input
    //USI_OUT_REG |= (1<<MISO_PIN); // Needed only if no external pullup
    
    // Configure USI for 3-wire mode (SPI mode), set to SPI master mode 0. Also configured to use
    // software clock strobe
    USICR |= (1<<USIOIE) | SPI_MASTER | SPI_MODE0 | (1<<USICLK);
    
    // Enable CTC and initialize prescaler
    TCCR0A = CTC_TC0;
    TCCR0B = CLOCK_SEL;
    
    // Set Output Compare Reg 0A
    OCR0A = TC0_COMPARE_VAL;
    
    // Set CS pin in idle mode
    if (csActiveLow) {
        USI_OUT_REG |= (1<<CS_PIN);
    }
    else {
        USI_OUT_REG &= ~(1<<CS_PIN);
    }
}

void startSpi() {
    // Clear CTC flag
    TIFR0 |= (1<<OCF0A);
    // Enable compare match A
    TIMSK0 |= (1<<OCIE0A);
}

// TC0 compare match A interrupt
// Use interrupt to use USITC in software clock strobe mode
//#pragma vector=TIM0_COMPA_vect
ISR(TIM0_COMPA_vect) {
    USICR |= (1<<USITC);
}

// USI overflow interrupt.
// Stop software clock strobe and collect data on USIDR
//#pragma vector=USI_OVF_vect
ISR_Two(USI_OVF_vect) {
    // Disable CTC interrupt to stop SPI clock
    TIMSK0 &= ~(1<<OCIE0A);
    
    // Clear overflow flag
    USISR = (1<<USIOIF);
    
    // Store data here or it will be overwitten on next read
    data <<= USIDR;
    
    // Re-start SCK if we haven't finished fetching enough bytes
    if (numberOfBytes > 0) {
        numberOfBytes--;
        startSpi();
    }
    else {
        done = 1;
    }
}

long getNegativeTemp(long tempData, long lsb) {
    tempData = ~tempData;
    tempData &= lsb; // Re-obtain 13 or 11 LSB
    tempData += 1;
    return tempData * -1; // Return negative temp value
}

// Parses data and sends back info
Mx31855_Data readData() {
    // Holds info that will be relayed to main program
    Mx31855_Data response;
    response.error = NO_ERROR;
    
    // Activate CS
    USI_OUT_REG ^= (1<<CS_PIN);
    
    // We need 4 Bytes of data
    numberOfBytes = 4;
    startSpi();
    
    do {} while(done == 0); // Wait until transfer is complete
    
    // De-activate CS
    USI_OUT_REG ^= (1<<CS_PIN);
    
    // Check to make sure we are communicating with a MAX31855
    // Bits 3 and 17 must be zero and there should be at least one bit high
    uint8_t bitCount = 0;
    for (uint8_t i = 0; i <= 31; i++) {
        if (data & (1<<(i)))
            bitCount++;
    }
    
    if ((bitCount <= 0) || (data & 0x2008))
        response.error = NO_COMM;
    
    // Check for faults
    // If fault bit found then find out what type of fault
    if (data & 0x10000) {
        switch (data & 0x07) {
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
    
    long thTemp = data >> 18; // Strip away other bits and leave only thermocouple temp data
    thTemp &= 0x3FF; // Make sure its only thermocouple temp data
    long jctTemp = data >> 4; // Strip away fault bits
    jctTemp &= 0x000007FF; // Remove thermocouple temp data
    
    // Sign bit for thermocouple temp data is Bit14
    // Check if its a negative temp. If so then get the negative value
    if (thTemp & 0x2000) {
        thTemp = getNegativeTemp(thTemp, 0x1FFF);
    }
    
    response.thermTemp = thTemp * 0.25; // Convert to Celsius
    
    // Sign bit for junction temp data is Bit12
    // Check if its a negative temp. If so then get the negative value
    if (jctTemp & 0x800) {
        jctTemp = getNegativeTemp(jctTemp, 0x7FF);
    }
    
    response.junctionTemp = jctTemp * 0.0625; // Convert to Celsius
    
    return response;
}

