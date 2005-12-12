//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "gainer_common.h"

BOOL bContinuousAinRequested = FALSE;
BYTE bContinuousAinMask = 0x0000;
BOOL bContinuousDinRequested = FALSE;
BOOL bQuitRequested = FALSE;
BYTE bCurrentConfig = CONFIG_START;
BYTE bRequestedConfig = CONFIG_START;

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

const char cConfigCommandPrefix[] = {'K','O','N','F','I','G','U','R','A','T','I','O','N','_'};

WORD wAdcValue[16];
BYTE bAdcChannelNumber;
BYTE bAdcFlags;
BOOL bVerboseMode = TRUE;

BYTE bChannels_AIN = 0;
BYTE bChannels_AOUT = 0;
BYTE bChannels_DIN = 0;
BYTE bChannels_DOUT = 0;

/**
 * private functions of CONFIG_START
 */
void handle_commands_config_start(void);
BYTE handle_config_command(char *pCommand);
BYTE command_reboot(void);
BYTE command_verbose(char *pCommand);

void main()
{
	Enter_Config_Start();	// enter CONFIG_START

	while (TRUE) {
		switch (bCurrentConfig) {
			case CONFIG_START:
				Main_Config_Start();
				break;

			case CONFIG_1:
			case CONFIG_2:
			case CONFIG_3:
			case CONFIG_4:
				Main_Config_A();
				break;

			case CONFIG_5:
			case CONFIG_6:
				Main_Config_B();
				break;

			default:
				break;
		}

		if (bQuitRequested) {
			WaitForBriefSpells();

			switch (bCurrentConfig) {
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

				default:
					break;
			}

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

	// set drive mode of P2[6] (TxD) to 'Strong'
	// DM[2:0] = '001' (Strong)
	PRT2DM2 &= ~0x40;
	PRT2DM1 &= ~0x40;
	PRT2DM0 |= 0x40;

	UART_CPutString("!!!*");

#if SERIAL_DEBUG_ENABLED
	UART_CPutString("\r\nEnter_Config_Start()\r\n");
#endif
}

void Exit_Config_Start()
{
#if SERIAL_DEBUG_ENABLED
	UART_CPutString("\r\nExit_Config_Start()\r\n");
#endif

	UART_Stop();

	// set drive mode of P2[6] (TxD) to 'High-Z Analog'
	// DM[2:0] = '110' (High-Z Analog)
	PRT2DM2 |= 0x40;
	PRT2DM1 |= 0x40;
	PRT2DM0 &= ~0x40;

	M8C_DisableGInt;

	UnloadConfig_gainer();
}

void Main_Config_Start()
{
	handle_commands_config_start();

	if (bCurrentConfig == bRequestedConfig) {
		return;	// have nothing to do :)
	}

	switch (bCurrentConfig) {
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

		default:
			break;
	}

	switch (bRequestedConfig) {
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

		default:
			break;
	}
}

void handle_commands_config_start()
{
	char * pCommand;						// Parameter pointer

	if (UART_bCmdCheck()) {					// Wait for command    
		if(pCommand = UART_szGetParam()) {
			switch (*pCommand) {
				case 'K':	// configuration (KONFIGURATION_n)
					UART_Write(cReplyBuffer, handle_config_command(pCommand));
					WaitForBriefSpells();
					break;
				
				case 'Q':	// reboot (Q)
					UART_Write(cReplyBuffer, command_reboot());
					WaitForBriefSpells();
					break;
				
				case 'V':	// verbose (V)
					UART_Write(cReplyBuffer, command_verbose(pCommand));
					break;
				
				default:
					// seems to be an invalid command
					cReplyBuffer[0] = '!';
					cReplyBuffer[1] = '*';
					UART_Write(cReplyBuffer, 2);
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
		cReplyBuffer[i] = cConfigCommandPrefix[i];
	}

	cReplyBuffer[i] = (*p);
	i++;
	cReplyBuffer[i] = '*';

	bNumBytes = sizeof(cConfigCommandPrefix) + 2;

	switch (*p) {
		case '0':	// i.e. 'KONFIGURATION_0'
			bRequestedConfig = CONFIG_START;
			break;

		case '1':	// i.e. 'KONFIGURATION_1'
			bRequestedConfig = CONFIG_1;
			break;

		case '2':	// i.e. 'KONFIGURATION_2'
			bRequestedConfig = CONFIG_2;
			break;

		case '3':	// i.e. 'KONFIGURATION_3'
			bRequestedConfig = CONFIG_3;
			break;

		case '4':	// i.e. 'KONFIGURATION_4'
			bRequestedConfig = CONFIG_4;
			break;

		case '5':	// i.e. 'KONFIGURATION_5'
			bRequestedConfig = CONFIG_5;
			break;

		case '6':	// i.e. 'KONFIGURATION_6'
			bRequestedConfig = CONFIG_6;
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
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	bQuitRequested = TRUE;

	cReplyBuffer[0] = 'Q';
	cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_verbose(char *pCommand)
{
	char * p = pCommand;

	if (2 != UART_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	p++;
	
	switch (*p) {
		case '0':
			bVerboseMode = FALSE;
			break;

		default:
			break;
	}

	cReplyBuffer[0] = 'V';
	cReplyBuffer[1] = *p;
	cReplyBuffer[2] = '*';

	return 3;
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
	cReplyBuffer[0] = '!';
	cReplyBuffer[1] = '*';
}
