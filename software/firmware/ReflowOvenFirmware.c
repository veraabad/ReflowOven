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
//
// TC0 - SW UART & SPI
// TC1 - Timer & PWM

/**** CLK Speed = 8MHz ****/

// Will be using SCPI command syntax in order to communicate with computer

#include <string.h>
#include "ReflowOvenFirmware.h"
#include "MAX31855.h"
#include "softuart.h"
#include "Commands.h"
#include "Helper.h"

#define PWM_PIN     PA1

typedef enum TimeSel_Enum {
    MIL_SEC, SEC, MINS
} TimeSel;

typedef enum GET_SET_Enum {
    GET, SET
}GET_SET;

typedef struct Commands_struct {
    char firstCmd[6];
    char secondCmd[6];
    char valueStr[6];
}Cmd_Struct;

Mx31855_Data tempData;
int tempHold = 0;
int timeHold = 0;

// Fetch temperature info or set temperature to be reached
void tempSetGet(GET_SET dirFlow, int temp = 0) {
    if (dirFlow == GET) {
        Setup_MAX31855(0); // Initialize SPI
        tempData = readData_MAX31855();

        softuart_init(); // Restart UART

        char tStr[12] PROGMEM;
        sprintf(tStr, ":TEMP %ld", tempData.thermTemp);
        softuart_puts_P(tStr);
    }
    else if (dirFlow == SET) {
        // Set temperature value
        tempHold = temp;
    }
}

// Set dwell time for temperatuere or get time remaining
void timeSetGet(GET_SET dirFlow, TimeSel timeSel, int timeVal = 0) {
    if (GET) {
        /* TODO */
        // Get timer and print
    }
    else if (SET) {
        // Set timer
        timeHold = timeVal;
    }
}

void checkCommand(Cmd_Struct cmdStruct) {

    if (strcmp (GET_CMD, cmdStruct.firstCmd) == 0) {
        if (strcmp (TEMP_CMD, cmdStruct.secondCmd) == 0)
            tempSetGet(GET); // Get temperature info

        else if (strcmp (TIME_CMD, cmdStruct.secondCmd) == 0)
            timeSetGet(GET, MINS);

        else if (strcmp (TIME_S_CMD, cmdStruct.secondCmd) == 0)
            timeSetGet(GET, SEC);

        else if (strcmp (TIME_MS_CMD, cmdStruct.secondCmd) == 0)
            timeSetGet(GET, MIL_SEC);

        else {
            softuart_puts_P("\r\nERROR:SEC_CMD_NOT_RECOG\r\n");
            return;
        }
    }

    else if (strcmp (SET_CMD, cmdStruct.firstCmd) == 0) {
        if (strcmp (TEMP_CMD, cmdStruct.secondCmd) == 0) {
            if ((sizeof cmdStruct.valueStr) > 0)
                tempSetGet(SET, asciiToInt(cmdStruct.valueStr)); // Set temperature

            else
                softuart_puts_P("\r\nERROR:NO_VALUE\r\n");
        }

        else if (strcmp (TIME_CMD, cmdStruct.secondCmd) == 0) {
            if ((sizeof cmdStruct.valueStr) > 0)
                timeSetGet(SET, MINS, asciiToInt(cmdStruct.valueStr));

            else
                softuart_puts_P("\r\nERROR:NO_VALUE\r\n");
        }

        else if (strcmp (TIME_S_CMD, cmdStruct.secondCmd) == 0) {
            if ((sizeof cmdStruct.valueStr) > 0)
                timeSetGet(SET, SEC, asciiToInt(cmdStruct.valueStr));

            else
                softuart_puts_P("\r\nERROR:NO_VALUE\r\n");
        }

        else if (strcmp (TIME_MS_CMD, cmdStruct.secondCmd) == 0) {
            if ((sizeof cmdStruct.valueStr) > 0)
                timeSetGet(SET, MIL_SEC, asciiToInt(cmdStruct.valueStr));

            else
                softuart_puts_P("\r\nERROR:NO_VALUE\r\n");
        }

        else {
            softuart_puts_P("\r\nERROR:SEC_CMD_NOT_RECOG\r\n");
            return;
        }
    }

    else {
        softuart_puts_P("\r\nERROR:CMD_NOT_RECOGNIZED\r\n");
        return;
    }
}

// Parses whole string received from user and saves only relevant commands and values
void commandParser(char rxCmds[]) {
    Cmd_Struct commands;
    uint8_t cmdSeq = 0;

    for (uint8_t i = 0; i < (sizeof rxCmds); i++) {
        // Make sure the command string starts with a colon
        // If not then it is not a command
        if (rxCmds[i] == ':')
            cmdSeq++;
        else if (rxCmds[i] == ' ')
            cmdSeq = 3;

        // Start saving commands to their respective locations
        switch (cmdSeq) {
            case 1:
                if (!(rxCmds[i] == ':'))
                    commands.firstCmd++ = rxCmds[i];
                break;

            case 2:
                if (!(rxCmds[i] == ':'))
                    commands.secondCmd++ = rxCmds[i];

            case 3:
                if (!(rxCmds[i] == ' '))
                    commands.valueStr++ = rxCmds[i];

            default:
                // Didn't find a colon to start the command string
                softuart_puts_P("\r\nERROR:NOT_A_CMD\r\n");
                return;
                break;
        }
    }
    checkCommand(commands);
}

void main() {
    // First run to make sure that UART works
    char tempChar[8];
    uint8_t charIndex = 0;
    int cmdPresent = 0;
    /*
    static const char pstring[] PROGMEM =
    "adapted for Atmel AVR and this demo by Martin Thomas\r\n"; */
    // Initialize the UART
    softuart_init();

    sei(); // Setup to recognize interrupts

    softuart_puts_P( "\r\nAVR ATtiny84 -- Demo Application\r\n");

    while(1) {
        // Try capturing any incoming characters
        if ( softuart_kbhit() ) {
            char c = softuart_getchar();
            if (c == '\r') {
                char str[8];
                strncpy(str, tempChar, charIndex + 1);
                commandParser(str);
                charIndex = 0;
            }
            else {
                tempChar[charIndex] = c;
                charIndex++;
            }

            if(cmdPresent) {
                softuart_puts_P("Good\r\n");
                cmdPresent = 0;
            }
            /*
            c = softuart_getchar();
            if (c == '\n' || c == '\r')
                softuart_puts_P("\r\n");
            softuart_putchar( '[' );
            softuart_putchar( c );
            softuart_putchar( ']' ); */
        }
    }
}
