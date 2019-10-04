//
//  F: ReflowOvenFirmware.c
//  P: ReflowOven
//
//  Created by Abad Vera on 3/4/16.
//  Copyright © 2016 Abad Vera. All rights reserved.
//  Last Modified: 09/26/2019
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
// TC0 - SW UART & SPI
// TC1 - Timer & PWM

/**** CLK Speed = 8MHz ****/

// Will be using SCPI command syntax in order to communicate with computer

#include "ReflowOvenFirmware.h"
#include "MAX31855.h"
#include "avr-softuart/softuart.h"
#include "Timer.h"
// #include "Commands.h"
#include "SerialComm.h"

// #define PWM_PIN     PA1

uint64_t startTime = 0;

void testing() {
    if ((millis() - startTime) > 1500) {
        softuart_puts_P("\r\n***Test***\r\n");
        PORTA ^= (1 << PA0);
        startTime = millis();
    }
}

void loop() {
    // serialProcessPackets();
    testing();
}

int main() {
    // Init Timer
    timerInit();

    // Initialize the UART
    softuart_init();

    // Setup to recognize interrupts
    sei();
    DDRA |= (1 << PA0);
    // PORTA |= (1 << PA0);
    softuart_puts_P( "\r\nAVR ATtiny84 -- Demo Application\r\n");

    startTime = millis();
    while(1) {
        loop();
    }
}
