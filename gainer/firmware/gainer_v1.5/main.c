//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

/*
 * UART Clock
 * case 1: 57600bps
 * 24000000 / (8 * 57600) = 52.083
 * VC1 = SysClk / 4
 * VC2 = VC1 / 13
 * 24000000 / (8 * 4 * 13) = 57692.307
 * error < 0.2%
 * 
 * case 2: 38400bps
 * 24000000 / (8 * 38400) = 78.125
 * VC1 = SysClk / 6
 * VC2 = VC1 / 13
 * 24000000 / (8 * 6 * 13) = 38461.538
 * error < 0.2%
 * 
 * PWM Clock
 * 24000000 / (6 * 13 * 256) = 1201.923Hz
 */
#include <m8c.h>        // part specific constants and macros
#include <string.h>

#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "psocdynamic.h"
#include "gainer_api.h"
#include "custom.h"		// custom user code

void main()
{
	// NOTE: Just for testing
	BOOL led = FALSE;

	Initialize();

	// set port mode for all pins
	// configurations will be loaded automatically to place and init user modules
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
	SetPortMode(12, AOUT_SERVO);
	SetPortMode(13, AOUT_SERVO);
	SetPortMode(14, AOUT_SERVO);
	SetPortMode(15, AOUT_SERVO);
	SetPortMode(PORT_BUTTON, DIN_PULL_DOWN);
	SetPortMode(PORT_LED, DOUT_STRONG);

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

	PWM16_1_WritePeriod(2550);
	PWM16_1_WritePulseWidth(2294);
	PWM16_1_EnableInt();
	PWM16_1_Start();

    // Custom initization code.
	CustomInit();
    // End Initialize Project

	while (1) {
		// Sync loop sample rate
		SyncWait();

#if 1
		UART_PutChar('>');

		// NOTE: Just for testing
		if (led) {
			DigitalWrite(PORT_LED, 1);
			led = FALSE;
		} else {
			DigitalWrite(PORT_LED, 0);
			led = TRUE;
		}
#endif

		// update input variables
		ScanInputs();

		// Custom Post Input function
		CustomPostInputUpdate();

		// run transfer function and update output variables
		ProcessCommands();

		// CustomPreOutputUpdate();
		CustomPreOutputUpdate();
	}
}
