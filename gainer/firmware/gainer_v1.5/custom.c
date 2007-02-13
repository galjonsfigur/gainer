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
	// set port mode for all pins
	// configurations will be loaded automatically to place and init user modules
	SetPortMode(0, AIN_ADC);
	SetPortMode(1, AIN_ADC);
	SetPortMode(2, AIN_ADC);
	SetPortMode(3, AIN_ADC);
#if 0
	SetPortMode(4, DIN_PULL_DOWN);
	SetPortMode(5, DIN_PULL_DOWN);
	SetPortMode(6, DIN_PULL_DOWN);
	SetPortMode(7, DIN_PULL_DOWN);
#else
	SetPortMode(4, AIN_ADC);
	SetPortMode(5, AIN_ADC);
	SetPortMode(6, AIN_ADC);
	SetPortMode(7, AIN_ADC);
#endif
	SetPortMode(8, AOUT_PSEUDO_ANALOG);
	SetPortMode(9, AOUT_PSEUDO_ANALOG);
	SetPortMode(10, AOUT_PSEUDO_ANALOG);
	SetPortMode(11, AOUT_PSEUDO_ANALOG);
	SetPortMode(12, AOUT_SERVO);
	SetPortMode(13, AOUT_SERVO);
	SetPortMode(14, AOUT_SERVO);
	SetPortMode(15, AOUT_SERVO);

#if 1
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
