#ifndef __AD_HANDLER_H__
#define __AD_HANDLER_H__
#if 0 
#include <MD_AD9833.h>
#include <SPI.h>
#include "AD9833FuncGen.h"

AD9833FuncGen AD(PIN_FSYNC, PIN_CS_DPOT);
#endif
#if 0
// Pins for SPI comm with the AD9833 IC
#define DATA  11	///< SPI Data pin number
#define CLK   13	///< SPI Clock pin number
#define FSYNC 10	///< SPI Load pin number (FSYNC in AD9833 usage)

MD_AD9833	AD(FSYNC);  // Hardware SPI
// MD_AD9833	AD(DATA, CLK, FSYNC); // Arbitrary SPI pins
#endif
#if 0 
void setupAD(void)
{
	AD.begin();
}
#endif
#if 0 
void loopAD(void)
{
	static uint16_t lastv = 0;
	uint16_t v = analogRead(A0);

	if (abs(v-lastv) > 20)
	{
		AD.setFrequency(MD_AD9833::CHAN_0, 1000 + v);
		lastv = v;
	}
}
#endif
#endif // __AD_HANDLER_H__
