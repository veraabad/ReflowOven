//
//  main.c
//  ReflowOven
//
//  Created by Abad Vera on 3/4/16.
//  Copyright © 2016 Abad Vera. All rights reserved.
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

/**** CLK Speed = 8MHz ****/

#include <avr/io.h>
#include "main.h"
#include "MAX31855.h"

#define PWM_PIN     PA1
#define RX1_PIN     PA2
#define TX1_PIN     PA3

void setupPortsCLK() {
    // Set Outputs
    DDRA = (1<<CS_PIN) | (1<<SCK_PIN) | (1<<TX1_PIN) | (1<<PWM_PIN);
    // Need pull up for RX1 and Set and CS High
    PORTA = (1<<CS_PIN) | (1<<RX1_PIN);
    
    // Set prescaler for SPI
    
}

// Initialize SPI Master Device
void spiInitMaster (void) {
    
    
    // Enable SPI, Set as Master
}

void main() {
    // Quick Setup
    setupPortsCLK();
}
