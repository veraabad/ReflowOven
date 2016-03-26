/* Test application for the SPI-via-USI-driver. */

#include "spi_via_usi_driver.c"



#define SPIMODE 0	// Sample on leading _rising_ edge, setup on trailing _falling_ edge.
//#define SPIMODE 1	// Sample on leading _falling_ edge, setup on trailing _rising_ edge.



void main()
{
	unsigned char val = 0;		// Temp value to send.
	DDRB = 0xFF;			// Set PORTB to all output.
//	spiX_initmaster(SPIMODE);	// Init SPI driver as master.
	spiX_initslave(SPIMODE);	// Init SPI driver as slave.
	__enable_interrupt();		// Must do this to make driver work.
	do {
		spiX_put( val++ );	// Send temp value to SPI and increment,
		spiX_wait();		// wait for transmission to finish
		PORTB = spiX_get();	// and finally put result on PORTB.
	} while(1);			// Loop forever...
}
