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
#include "gainer_api.h"
#include "custom.h"		// custom user code

/**
 * private functions of CONFIG_START
 */
const char cConfigCommandPrefix[] = {'K','O','N','F','I','G','U','R','A','T','I','O','N','_'};
const char cVersionString[] = {'1','.','5','.','0','.','0','0'};

void main()
{
	M8C_EnableGInt;

	UART_IntCntl(UART_ENABLE_RX_INT);
	UART_Start(UART_PARITY_NONE);

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
	AnalogWrite(8, 0);
	AnalogWrite(9, 64);
	AnalogWrite(10, 128);
	AnalogWrite(11, 255);
	AnalogWrite(12, 0);
	AnalogWrite(13, 64);
	AnalogWrite(14, 128);
	AnalogWrite(15, 255);
#endif

    // Custom initization code.
    CustomInit();
    // End Initialize Project

	while (1) {
		// Sync loop sample rate
		SyncWait();

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
