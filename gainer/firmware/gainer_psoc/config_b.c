#include "gainer_common.h"

/**
 * private functions of CONFIG_B
 */
void handle_commands_config_b(void);

void Enter_Config_B(void)
{
	bCurrentConfig = bRequestedConfig;

	LoadConfig_config_b();

	M8C_EnableGInt;
	UART_B_IntCntl(UART_B_ENABLE_RX_INT);
	UART_B_Start(UART_B_PARITY_NONE);

	// set drive mode of P2[6] (TxD) to 'Strong'
	// DM[2:0] = '001' (Strong)
	PRT2DM2 &= ~0x40;
	PRT2DM1 &= ~0x40;
	PRT2DM0 |= 0x40;

	// SHOULD HANDLE I2C HERE IN THE NEAR FUTURE!?

#if SERIAL_DEBUG_ENABLED
	UART_B_CPutString("\r\nEnter_Config_B()\r\n");
#endif
}

void Exit_Config_B(void)
{
#if SERIAL_DEBUG_ENABLED
	UART_B_CPutString("\r\nExit_Config_B\r\n");
#endif

	// SHOULD HANDLE I2C HERE IN THE NEAR FUTURE!?

	M8C_DisableGInt;
	UART_B_Stop();

	// set drive mode of P2[6] (TxD) to 'High-Z Analog'
	// DM[2:0] = '110' (High-Z Analog)
	PRT2DM2 |= 0x40;
	PRT2DM1 |= 0x40;
	PRT2DM0 &= ~0x40;

	UnloadConfig_config_b();
}

void Main_Config_B(void)
{
	handle_commands_config_b();
}

void handle_commands_config_b(void)
{
	char * pCommand;						// Parameter pointer

	if (UART_B_bCmdCheck()) {				// Wait for command    
		if(pCommand = UART_B_szGetParam()) {
			switch (*pCommand) {
				default:
					break;
			}
		}
		UART_B_CmdReset();					// Reset command buffer
	}
}
