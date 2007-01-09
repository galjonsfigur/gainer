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

//global_parameters _gainer;

/**
 * private functions of CONFIG_START
 */
const char cConfigCommandPrefix[] = {'K','O','N','F','I','G','U','R','A','T','I','O','N','_'};
const char cVersionString[] = {'1','.','0','.','1','.','0','2'};
/*
void handle_commands_config_start(void);
BYTE handle_config_command(char *pCommand);
BYTE command_reboot(void);
BYTE command_verbose(char *pCommand);
BYTE command_version(char *pCommand);
void init_global_parameters(void);
void change_configuration(BYTE from, BYTE to);
*/

void main()
{
/*
	_gainer.bCurrentConfig = CONFIG_START;
	_gainer.bRequestedConfig = CONFIG_START;

	Enter_Config_Start();	// enter CONFIG_START

	while (TRUE) {
		while (!_gainer.bQuitRequested) {
			_gainer.pMainFunction();
		}

		// Change configuration if needed.
		if (CONFIG_START == _gainer.bCurrentConfig) {
			// We don't have to do reboot, so just clear the quit-requested flag.
			_gainer.bQuitRequested = FALSE;
		} else {
			_gainer.bRequestedConfig = CONFIG_START;
			change_configuration(_gainer.bCurrentConfig, _gainer.bRequestedConfig);
		}
	}
*/
/*
	// set port modes for all pins
	// configurations will be loaded automatically to place and init user modules
	SetPortMode(PORT_00, AIN_NORMAL);
	SetPortMode(PORT_01, AIN_NORMAL);
	SetPortMode(PORT_02, AIN_NORMAL);
	SetPortMode(PORT_03, AIN_NORMAL);
	SetPortMode(PORT_04, DIN_PULL_DOWN);
	SetPortMode(PORT_05, DIN_PULL_DOWN);
	SetPortMode(PORT_06, DIN_PULL_DOWN);
	SetPortMode(PORT_07, DIN_PULL_DOWN);
	SetPortMode(PORT_08, AOUT_PRSPWM);
	SetPortMode(PORT_09, AOUT_PRSPWM);
	SetPortMode(PORT_10, AOUT_PRSPWM);
	SetPortMode(PORT_11, AOUT_PRSPWM);
	SetPortMode(PORT_12, DOUT_STRONG);
	SetPortMode(PORT_13, DOUT_STRONG);
	SetPortMode(PORT_14, DOUT_STRONG);
	SetPortMode(PORT_15, DOUT_STRONG);
	SetPortMode(PORT_BUTTON, DIN_PULL_DOWN);
	SetPortMode(PORT_LED, DOUT_STRONG);

    // Custom initization code.
    CustomInit();
    // End Initialize Project

	while (TRUE) {
		// Sync loop sample rate
		SyncWait();

		// update input variables
		UpdateInputs();

		// Custom Post Input function
		CustomPostInputUpdate();

		// run transfer function and update output variables
		ProcessCommands();

		// CustomPreOutputUpdate();
		CustomPreOutputUpdate();

		// set outputs
		UpdateOutputs();
	}
*/
}
