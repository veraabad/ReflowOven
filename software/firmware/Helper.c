//
//  Helper.c
//  ReflowOven
//
//  Created by Abad Vera on 1/20/17.
//  Copyright © 2017 Abad Vera. All rights reserved.
//

/* 
	This File contains helper functions that can be useful to this project
	and any other projects I may have in the future
*/

#include <math.h>

int asciiToInt( char ints[] ) {
	uint8_t charAmt = sizeof ints;
	int hightestBase = int(pow (10, charAmt - 1));
	int newNumber = 0;

	for(int i = 0; i < charAmt; i++) {
		uint8_t tempInt = ints[i];
		tempInt &= 0xF;
		newNumber += tempInt * hightestBase;
		hightestBase /= 10;
	}

	return newNumber;
}