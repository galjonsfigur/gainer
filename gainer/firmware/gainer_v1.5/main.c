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
	Initialize();

    // Custom initization code
	CustomInit();

	SetPortMode(PORT_BUTTON, DIN_PULL_DOWN);
	SetPortMode(PORT_LED, DOUT_STRONG);
	DigitalWrite(PORT_LED, 0);

	PWM16_1_WritePeriod(2550);
	PWM16_1_WritePulseWidth(2294);
	PWM16_1_EnableInt();
	PWM16_1_Start();

	while (1) {
#if 0
		// Sync loop sample rate
		SyncWait();
#endif

		// Update input variables
		ScanInputs();

		// Custom post input function
		CustomPostInputUpdate();

		// Process commands from the host
		ProcessCommands();
		
		// Report values to the host port if needed
		ReportToHost();

		// Custom pre output update
		CustomPreOutputUpdate();
	}
}
