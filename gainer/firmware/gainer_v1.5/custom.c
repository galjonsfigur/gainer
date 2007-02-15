/*
 * Custom.c
 * 
 * Custom functions for insertion into Gainer I/O module, including fixed functions
 */

#include <m8c.h>        // part specific constants and macros
#include "gainer_api.h"
#include "custom.h"

void CustomInit()
{
	// Set port mode for all ports
	// Configurations will be loaded automatically to place and init user modules
	SetPortMode(0, AIN_ADC);
	SetPortMode(1, AIN_ADC);
	SetPortMode(2, AIN_ADC);
	SetPortMode(3, AIN_ADC);
	SetPortMode(4, DIN_PULL_DOWN);
	SetPortMode(5, DIN_PULL_DOWN);
	SetPortMode(6, DIN_PULL_DOWN);
	SetPortMode(7, DIN_PULL_DOWN);
	SetPortMode(8, AOUT_PSEUDO_ANALOG);
	SetPortMode(9, AOUT_PSEUDO_ANALOG);
	SetPortMode(10, AOUT_PSEUDO_ANALOG);
	SetPortMode(11, AOUT_PSEUDO_ANALOG);
#if 1
	SetPortMode(12, DOUT_STRONG);
	SetPortMode(13, DOUT_STRONG);
	SetPortMode(14, DOUT_STRONG);
	SetPortMode(15, DOUT_STRONG);
#else
	SetPortMode(12, AOUT_SERVO);
	SetPortMode(13, AOUT_SERVO);
	SetPortMode(14, AOUT_SERVO);
	SetPortMode(15, AOUT_SERVO);
#endif

#if 0
	// set initial values (just for testing)
	AnalogWrite(8, 1);
	AnalogWrite(9, 32);
	AnalogWrite(10, 128);
	AnalogWrite(11, 255);
	AnalogWrite(12, 1);
	AnalogWrite(13, 64);
	AnalogWrite(14, 126);
	AnalogWrite(15, 254);
#else
	AnalogWrite(8, 0);
	AnalogWrite(9, 0);
	AnalogWrite(10, 0);
	AnalogWrite(11, 0);
	DigitalWrite(12, 0);
	DigitalWrite(13, 0);
	DigitalWrite(14, 0);
	DigitalWrite(15, 0);
#endif
}

void CustomPostInputUpdate()
{
}

void CustomPreOutputUpdate()
{
}
