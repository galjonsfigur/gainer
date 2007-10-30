//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

#include <m8c.h>        // part specific constants and macros
#include <string.h>

#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "gainer_common.h"

global_parameters _gainer;

/**
 * private functions of CONFIG_START
 */
const char cConfigCommandPrefix[] = {'K','O','N','F','I','G','U','R','A','T','I','O','N','_'};
const char cVersionString[] = {'1','.','1','.','0','b','0','0'};

void handle_commands_config_start(void);
BYTE handle_config_command(char *pCommand);
BYTE command_reboot(void);
BYTE command_verbose(char *pCommand);
BYTE command_version(char *pCommand);
void init_global_parameters(void);
void change_configuration(BYTE from, BYTE to);

void main()
{
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
}

void Enter_Config_Start()
{
	_gainer.bCurrentConfig = CONFIG_START;
	_gainer.pMainFunction = Main_Config_Start;

	init_global_parameters();

	// load the base configuration if not loaded
	if (!IsgainerLoaded()) {
		LoadConfig_gainer();
	}

	M8C_EnableGInt;

	UART_IntCntl(UART_ENABLE_RX_INT);
	UART_Start(UART_PARITY_NONE);

	// make sure to set drive mode of P2[6] (TxD) to 'Strong'
	// DM[2:0] = '001' (Strong)
	PRT2DM2 &= ~0x40;
	PRT2DM1 &= ~0x40;
	PRT2DM0 |= 0x40;

	// make sure to turn the LED off (for the first reboot after a firmware update)
	SET_LED_L();
}

void Exit_Config_Start()
{
	M8C_DisableGInt;

	// NOTE: Since the 'gainer' configuration is the base configuration, never unload it!!!
}

void Main_Config_Start()
{
	handle_commands_config_start();

	if (_gainer.bCurrentConfig == _gainer.bRequestedConfig) {
		return;	// have nothing to do :)
	}

	change_configuration(_gainer.bCurrentConfig, _gainer.bRequestedConfig);
}

void handle_commands_config_start()
{
	char * pCommand;						// Parameter pointer

	// reset Rx buffer if it seems to be broken
	if (UART_bErrCheck()) {
		UART_CmdReset();
		return;
	}

	if (UART_bCmdCheck()) {					// Wait for command    
		if(pCommand = UART_szGetParam()) {
			switch (*pCommand) {
				case 'K':	// configuration (KONFIGURATION_n)
					UART_Write(_gainer.cReplyBuffer, handle_config_command(pCommand));
					WaitForBriefSpells();
					break;
				
				case 'Q':	// reboot (Q)
					UART_Write(_gainer.cReplyBuffer, command_reboot());
					WaitForBriefSpells();
					break;
				
				case 'V':	// verbose (V)
					UART_Write(_gainer.cReplyBuffer, command_verbose(pCommand));
					break;
				
				case '?':	// version (?)
					UART_Write(_gainer.cReplyBuffer, command_version(pCommand));
					break;
				
				default:
					// seems to be an invalid command
					PutErrorStringToReplyBuffer();
					UART_Write(_gainer.cReplyBuffer, 2);
					break;
			}
		}
		UART_CmdReset();					// Reset command buffer
	}
}

BYTE handle_config_command(char *pCommand)
{
	char * p = pCommand;
	BYTE bNumBytes = 0;
	BYTE i = 0;

	// check command length
	if ((sizeof(cConfigCommandPrefix) + 1) != UART_bCmdLength()) {
		// seems to be an invalid command
		PutErrorStringToReplyBuffer();
		bNumBytes = 2;
		return bNumBytes;
	}

	// check command prefix
	for (i = 0; i < sizeof(cConfigCommandPrefix); i++) {
		if (cConfigCommandPrefix[i] != (*p)) {
			PutErrorStringToReplyBuffer();
			bNumBytes = 2;
			return bNumBytes;
		}

		p++;
	}

	for (i = 0; i < sizeof(cConfigCommandPrefix); i++) {
		_gainer.cReplyBuffer[i] = cConfigCommandPrefix[i];
	}

	_gainer.cReplyBuffer[i] = (*p);
	i++;
	_gainer.cReplyBuffer[i] = '*';

	bNumBytes = sizeof(cConfigCommandPrefix) + 2;

	switch (*p) {
		case '0':	// i.e. 'KONFIGURATION_0'
			_gainer.bRequestedConfig = CONFIG_START;
			break;

		case '1':	// i.e. 'KONFIGURATION_1'
			_gainer.bRequestedConfig = CONFIG_1;
			break;

		case '2':	// i.e. 'KONFIGURATION_2'
			_gainer.bRequestedConfig = CONFIG_2;
			break;

		case '3':	// i.e. 'KONFIGURATION_3'
			_gainer.bRequestedConfig = CONFIG_3;
			break;

		case '4':	// i.e. 'KONFIGURATION_4'
			_gainer.bRequestedConfig = CONFIG_4;
			break;

		case '5':	// i.e. 'KONFIGURATION_5'
			_gainer.bRequestedConfig = CONFIG_5;
			break;

		case '6':	// i.e. 'KONFIGURATION_6'
			_gainer.bRequestedConfig = CONFIG_6;
			break;

		case '7':	// i.e. 'KONFIGURATION_7'
			_gainer.bRequestedConfig = CONFIG_7;
			break;

		case '8':	// i.e. 'KONFIGURATION_8'
			_gainer.bRequestedConfig = CONFIG_8;
			break;

		default:
			// seems to be an invalid command
			// replace the reply buffer with error string
			PutErrorStringToReplyBuffer();
			bNumBytes = 2;
			break;
	}

	return bNumBytes;
}

BYTE command_reboot(void)
{
	if (1 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	_gainer.bQuitRequested = TRUE;

	_gainer.cReplyBuffer[0] = 'Q';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_verbose(char *pCommand)
{
	char * p = pCommand;

	if (2 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	p++;
	
	switch (*p) {
		case '0':
			_gainer.bVerboseMode = FALSE;
			break;

		default:
			break;
	}

	_gainer.cReplyBuffer[0] = 'V';
	_gainer.cReplyBuffer[1] = *p;
	_gainer.cReplyBuffer[2] = '*';

	return 3;
}

BYTE command_version(char *pCommand)
{
	char * p = pCommand;
	BYTE i = 0;

	if (1 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	// e.g. "?1.0.0.10*"
	_gainer.cReplyBuffer[0] = '?';

#if 1
	for (i = 0; i < sizeof(cVersionString); i++) {
		_gainer.cReplyBuffer[1 + i] = cVersionString[i];
	}
#else
	// I don't understand why this version won't work as expected...
	memcpy(&_gainer.cReplyBuffer[1], (void *)cVersionString, sizeof(cVersionString));
	i = 8;
#endif

	_gainer.cReplyBuffer[1 + i] = '*';

	return (2 + sizeof(cVersionString));
}

const char cHexString[16] = "0123456789ABCDEF";

/**
 * utility functions
 */
void ByteToHex(BYTE value, char *str)
{
	BYTE v = value;

	v >>= 4;
	*str = cHexString[v & 0x0F];
	str++;
	v = value;
	*str = cHexString[v & 0x0F];
}

void WaitForBriefSpells(void)
{
	INT i;
	float j;

	for (i = 0; i < 100; i++) {
		j = (float)i / 100.0;
	}
}

void PutErrorStringToReplyBuffer(void)
{
	_gainer.cReplyBuffer[0] = '!';
	_gainer.cReplyBuffer[1] = '*';
}

void init_global_parameters(void)
{
	_gainer.bGetAinRequested = AIN_NONE;
	_gainer.bGetAinChannelMask = 0x0000;
	_gainer.bContinuousDinRequested = FALSE;
	_gainer.bQuitRequested = FALSE;
	_gainer.bVerboseMode = TRUE;
	_gainer.bChannels_AIN = 0;
	_gainer.bChannels_AOUT = 0;
	_gainer.bChannels_DIN = 0;
	_gainer.bChannels_DOUT = 0;
}

void change_configuration(BYTE from, BYTE to)
{
	WaitForBriefSpells();

	switch (from) {
		case CONFIG_START:
			Exit_Config_Start();
			break;

		case CONFIG_1:
		case CONFIG_2:
		case CONFIG_3:
		case CONFIG_4:
			Exit_Config_A();
			break;

		case CONFIG_5:
		case CONFIG_6:
			Exit_Config_B();
			break;

		case CONFIG_7:
			Exit_Config_C();
			break;

		case CONFIG_8:
			Exit_Config_D();
			break;

		default:
			break;
	}

	switch (to) {
		case CONFIG_START:
			Enter_Config_Start();
			break;

		case CONFIG_1:
		case CONFIG_2:
		case CONFIG_3:
		case CONFIG_4:
			Enter_Config_A();
			break;

		case CONFIG_5:
		case CONFIG_6:
			Enter_Config_B();
			break;

		case CONFIG_7:
			Enter_Config_C();
			break;

		case CONFIG_8:
			Enter_Config_D();
			break;

		default:
			break;
	}
}
