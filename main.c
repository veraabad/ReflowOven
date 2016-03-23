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

