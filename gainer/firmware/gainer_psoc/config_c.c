#include "gainer_common.h"

/**
 * private functions of CONFIG_C
 */
void config_c_handle_commands(void);
BYTE config_c_command_reboot(void);

/**
 * private variables of CONFIG_C
 */
typedef struct {
	BYTE bDummy;
} config_c_parameters;

config_c_parameters _c;

void Enter_Config_C(void)
{
	_gainer.bCurrentConfig = _gainer.bRequestedConfig;

	_gainer.bContinuousAinRequested = FALSE;
	_gainer.bContinuousAinMask = 0x00;
	_gainer.bContinuousDinRequested = FALSE;

	LoadConfig_config_c();

	M8C_EnableGInt;
	UART_C_IntCntl(UART_C_ENABLE_RX_INT);
	UART_C_Start(UART_C_PARITY_NONE);

	// set drive mode of P2[6] (TxD) to 'Strong'
	// DM[2:0] = '001' (Strong)
	PRT2DM2 &= ~0x40;
	PRT2DM1 &= ~0x40;
	PRT2DM0 |= 0x40;
}

void Exit_Config_C(void)
{
	M8C_DisableGInt;

	UART_C_Stop();

	// set drive mode of P2[6] (TxD) to 'High-Z Analog'
	// DM[2:0] = '110' (High-Z Analog)
	PRT2DM2 |= 0x40;
	PRT2DM1 |= 0x40;
	PRT2DM0 &= ~0x40;

	UnloadConfig_config_c();
}

void Main_Config_C(void)
{
	config_c_handle_commands();
}

void config_c_handle_commands(void)
{
	char * pCommand;						// Parameter pointer
	BYTE bNumBytes = 0;

	// reset Rx buffer if it seems to be broken
	if (UART_C_bErrCheck()) {
		UART_C_CmdReset();
		return;
	}

	if (UART_C_bCmdCheck()) {				// Wait for command    
		if(pCommand = UART_C_szGetParam()) {
			switch (*pCommand) {
				case 'Q':	// reboot (Q)
					bNumBytes = config_c_command_reboot();
					break;

				default:
					// seems to be an invalid command
					_gainer.cReplyBuffer[0] = '!';
					_gainer.cReplyBuffer[1] = '*';
					bNumBytes = 2;
					break;
			}
		}

		if (bNumBytes > 0) {
			UART_C_Write(_gainer.cReplyBuffer, bNumBytes);
		}

		UART_C_CmdReset();					// Reset command buffer
	}
}

BYTE config_c_command_reboot(void)
{
	if (1 != UART_C_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	_gainer.bQuitRequested = TRUE;

	_gainer.cReplyBuffer[0] = 'Q';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}
