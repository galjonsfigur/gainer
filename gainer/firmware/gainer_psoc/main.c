//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "gainer_common.h"

BOOL bContinuousAinRequested = FALSE;
WORD bContinuousAinMask = 0x0000;
BOOL bContinuousDinRequested = FALSE;
BOOL bQuitRequested = FALSE;
BYTE bCurrentConfig = CONFIG_START;
//BYTE bRequestedConfig = CONFIG_START;
BYTE bRequestedConfig = CONFIG_A;

char cReplyBuffer[32];

const BYTE bGainTable[16] = {
	PGA_A_1_G1_00,	// G0x
	PGA_A_1_G1_14,	// G1x
	PGA_A_1_G1_33,	// G2x
	PGA_A_1_G1_46,	// G3x
	PGA_A_1_G1_60,	// G4x
	PGA_A_1_G1_78,	// G5x
	PGA_A_1_G2_00,	// G6x
	PGA_A_1_G2_27,	// G7x
	PGA_A_1_G2_67,	// G8x
	PGA_A_1_G3_20,	// G9x
	PGA_A_1_G4_00,	// GAx
	PGA_A_1_G5_33,	// GBx
	PGA_A_1_G8_00,	// GCx
	PGA_A_1_G16_0,	// GDx
	PGA_A_1_G24_0,	// GEx
	PGA_A_1_G48_0,	// GFx
};

WORD bAdcValue[16];
BYTE bAdcChannelNumber;
BYTE bAdcFlags;

BYTE bChannels_AIN = 0;
BYTE bChannels_AOUT = 0;
BYTE bChannels_DIN = 0;
BYTE bChannels_DOUT = 0;

/**
 * private functions of CONFIG_START
 */
void handle_commands_config_start(void);

void main()
{
	Enter_Config_Start();	// enter CONFIG_START

	while (TRUE) {
		switch (bCurrentConfig) {
			case CONFIG_START:
				Main_Config_Start();
				break;

			case CONFIG_A:
				Main_Config_A();
				break;

			case CONFIG_B:
				Main_Config_B();
				break;

			default:
				break;
		}

		if (bCurrentConfig != bRequestedConfig) {
			switch (bCurrentConfig) {
				case CONFIG_START:
					Exit_Config_Start();
					break;

				case CONFIG_A:
					Exit_Config_A();
					break;

				case CONFIG_B:
					Exit_Config_B();
					break;

				default:
					break;
			}

			switch (bRequestedConfig) {
				case CONFIG_START:
					Enter_Config_Start();
					break;

				case CONFIG_A:
					Enter_Config_A();
					break;

				case CONFIG_B:
					Enter_Config_B();
					break;

				default:
					break;
			}
		}

		if (bQuitRequested) {
			M8C_Reset;
		}
	}
}

void Enter_Config_Start()
{
	bCurrentConfig = CONFIG_START;

	LoadConfig_gainer();

	M8C_EnableGInt;
	UART_IntCntl(UART_ENABLE_RX_INT);
	UART_Start(UART_PARITY_NONE);

	// SHOULD HANDLE I2C HERE IN THE NEAR FUTURE!?

#if SERIAL_DEBUG_ENABLED
	UART_CPutString("\r\nEnter_Config_Start()\r\n");
#endif
}

void Exit_Config_Start()
{
#if SERIAL_DEBUG_ENABLED
	UART_CPutString("\r\nExit_Config_Start()\r\n");
#endif

	// SHOULD HANDLE I2C HERE IN THE NEAR FUTURE!?

	M8C_DisableGInt;
	UART_Stop();

	UnloadConfig_gainer();
}

void Main_Config_Start()
{
	handle_commands_config_start();
}

BYTE HandleConfigCommand(char *pCommand)
{
	char * p = pCommand;
	BYTE bNumBytes = 0;

	p++;
	switch (*p) {
		case '0':	// i.e. 'C0'
			bRequestedConfig = CONFIG_START;
			cReplyBuffer[0] = 'C';
			cReplyBuffer[1] = '0';
			cReplyBuffer[2] = '*';
			bNumBytes = 3;
			break;

		case '1':	// i.e. 'C1'
			bRequestedConfig = CONFIG_A;
			cReplyBuffer[0] = 'C';
			cReplyBuffer[1] = '1';
			cReplyBuffer[2] = '*';
			bNumBytes = 3;
			break;

		case '2':	// i.e. 'C2'
			bRequestedConfig = CONFIG_B;
			cReplyBuffer[0] = 'C';
			cReplyBuffer[1] = '2';
			cReplyBuffer[2] = '*';
			bNumBytes = 3;
			break;

		default:
			// seems to be an invalid command
			cReplyBuffer[0] = '!';
			cReplyBuffer[1] = '*';
			bNumBytes = 2;
			break;
	}

	return bNumBytes;
}

void handle_commands_config_start()
{
	char * pCommand;						// Parameter pointer

	if (UART_bCmdCheck()) {					// Wait for command    
		if(pCommand = UART_szGetParam()) {
			switch (*pCommand) {
				case 'C':
					UART_Write(cReplyBuffer, HandleConfigCommand(pCommand));
					break;
				
				default:
					break;
			}
		}
		UART_CmdReset();					// Reset command buffer
	}
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
