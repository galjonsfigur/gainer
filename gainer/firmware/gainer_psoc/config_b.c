#include "gainer_common.h"

/**
 * private functions of CONFIG_B
 */
void handle_commands_config_b(void);

void Enter_Config_B(void)
{
	bCurrentConfig = CONFIG_B;

	LoadConfig_config_b();

	M8C_EnableGInt;
	UART_B_IntCntl(UART_B_ENABLE_RX_INT);
	UART_B_Start(UART_B_PARITY_NONE);

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
				case 'C':
					UART_B_Write(cReplyBuffer, HandleConfigCommand(pCommand));
					break;
				
				default:
					break;
			}
		}
		UART_B_CmdReset();					// Reset command buffer
	}
}
