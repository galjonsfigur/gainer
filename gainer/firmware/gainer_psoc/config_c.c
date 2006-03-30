#include "gainer_common.h"

/**
 * interrupt handler
 */
#pragma interrupt_handler Counter8_C_Service_ISR;
void Counter8_C_Service_ISR(void);

/**
 * private functions of CONFIG_C
 */
void config_c_handle_commands(void);
BYTE config_c_command_set_aout_column(char *pCommand);
BYTE config_c_command_reboot(void);

/**
 * private definitions and variables of CONFIG_C
 */
#define bPOLY_255 0xB8	// Modular Polynomial = [8,6,5,4]
#define	bSEED_255 0xFF	// initial seed value
#define	bSEED_000 0x00	// initial seed value

const BYTE bScanLineSelectionMask[8] = {
	0x7F,	// P0[7]
	0xDF,	// P0[5]
	0xF7,	// P0[3]
	0xFD,	// P0[1]
	0xBF,	// P0[6]
	0xEF,	// P0[4]
	0xFB,	// P0[2]
	0xFE	// P0[0]
};

typedef struct {
	BYTE bData[8][8];	// 8 columns (x) by 8 rows (y)
	BYTE bCurrentScanLine;
	BYTE bType;
} config_c_parameters;

enum {
	COMMON_K_TYPE = 0,
	COMMON_A_TYPE
};

config_c_parameters _c;

void Counter8_C_Service_ISR(void)
{
	PRT0DR = 0xFF;	// disable all scan lines to turn off all LEDs

	switch (_c.bType) {
		case COMMON_K_TYPE:
			// O: ROW (Y)
			// I: COL (X) (i.e. column is scan line)
			PRS8_C_0_WriteSeed(_c.bData[_c.bCurrentScanLine][0]);
			PRS8_C_1_WriteSeed(_c.bData[_c.bCurrentScanLine][1]);
			PRS8_C_2_WriteSeed(_c.bData[_c.bCurrentScanLine][2]);
			PRS8_C_3_WriteSeed(_c.bData[_c.bCurrentScanLine][3]);
			PRS8_C_4_WriteSeed(_c.bData[_c.bCurrentScanLine][4]);
			PRS8_C_5_WriteSeed(_c.bData[_c.bCurrentScanLine][5]);
			PRS8_C_6_WriteSeed(_c.bData[_c.bCurrentScanLine][6]);
			PRS8_C_7_WriteSeed(_c.bData[_c.bCurrentScanLine][7]);
			break;
#if 0
		// WE DON'T CARE ABOUT COMMON ANODE TYPE
		// PLEASE ROTATE YOUR MATRIX LED :P
		case COMMON_A_TYPE:
			// O: COL (X)
			// I: ROW (Y) (i.e. row is scan line)
			PRS8_C_0_WriteSeed(_c.bData[0][_c.bCurrentScanLine]);
			PRS8_C_1_WriteSeed(_c.bData[1][_c.bCurrentScanLine]);
			PRS8_C_2_WriteSeed(_c.bData[2][_c.bCurrentScanLine]);
			PRS8_C_3_WriteSeed(_c.bData[3][_c.bCurrentScanLine]);
			PRS8_C_4_WriteSeed(_c.bData[4][_c.bCurrentScanLine]);
			PRS8_C_5_WriteSeed(_c.bData[5][_c.bCurrentScanLine]);
			PRS8_C_6_WriteSeed(_c.bData[6][_c.bCurrentScanLine]);
			PRS8_C_7_WriteSeed(_c.bData[7][_c.bCurrentScanLine]);
			break;
#endif
		default:
			break;
	}

	// set the current scan line low to turn on LEDs of the scan line
	PRT0DR = bScanLineSelectionMask[_c.bCurrentScanLine];

	_c.bCurrentScanLine = (_c.bCurrentScanLine + 1) & 0x07;	// i.e. 0~7
}

void Enter_Config_C(void)
{
	BYTE column, row;

	_gainer.bCurrentConfig = _gainer.bRequestedConfig;
	_gainer.pMainFunction = Main_Config_C;

	_gainer.bContinuousAinRequested = FALSE;
	_gainer.bContinuousAinMask = 0x00;
	_gainer.bContinuousDinRequested = FALSE;

	for (row = 0; row < 7; row++) {
		for (column = 0; column < 7; column++) {
			_c.bData[column][row] = 0;
		}
	}

#if 0
	// THIS IS A TEST...
	_c.bData[0][0] = 0x02;
	_c.bData[1][1] = 0x04;
	_c.bData[2][2] = 0x08;
	_c.bData[3][3] = 0x10;
	_c.bData[4][4] = 0x20;
	_c.bData[5][5] = 0x40;
	_c.bData[6][6] = 0x80;
	_c.bData[7][7] = 0xFF;

	_c.bData[0][0] = 0x02;
	_c.bData[1][0] = 0x04;
	_c.bData[2][0] = 0x08;
	_c.bData[3][0] = 0x10;
	_c.bData[4][0] = 0x20;
	_c.bData[5][0] = 0x40;
	_c.bData[6][0] = 0x80;
	_c.bData[7][0] = 0xFF;

	_c.bData[0][0] = 0x02;
	_c.bData[0][1] = 0x04;
	_c.bData[0][2] = 0x08;
	_c.bData[0][3] = 0x10;
	_c.bData[0][4] = 0x20;
	_c.bData[0][5] = 0x40;
	_c.bData[0][6] = 0x80;
	_c.bData[0][7] = 0xFF;
#endif

	_c.bCurrentScanLine = 0;
	_c.bType = COMMON_K_TYPE;

	LoadConfig_config_c();

	M8C_EnableGInt;

	// init polynomial values
	PRS8_C_0_WritePolynomial(bPOLY_255);
	PRS8_C_1_WritePolynomial(bPOLY_255);
	PRS8_C_2_WritePolynomial(bPOLY_255);
	PRS8_C_3_WritePolynomial(bPOLY_255);
	PRS8_C_4_WritePolynomial(bPOLY_255);
	PRS8_C_5_WritePolynomial(bPOLY_255);
	PRS8_C_6_WritePolynomial(bPOLY_255);
	PRS8_C_7_WritePolynomial(bPOLY_255);

	// write initial seed values
	PRS8_C_0_WriteSeed(bSEED_000);
	PRS8_C_1_WriteSeed(bSEED_000);
	PRS8_C_2_WriteSeed(bSEED_000);
	PRS8_C_3_WriteSeed(bSEED_000);
	PRS8_C_4_WriteSeed(bSEED_000);
	PRS8_C_5_WriteSeed(bSEED_000);
	PRS8_C_6_WriteSeed(bSEED_000);
	PRS8_C_7_WriteSeed(bSEED_000);

	// start PRS8 modules
	PRS8_C_0_Start();
	PRS8_C_1_Start();
	PRS8_C_2_Start();
	PRS8_C_3_Start();
	PRS8_C_4_Start();
	PRS8_C_5_Start();
	PRS8_C_6_Start();
	PRS8_C_7_Start();

	// start the PWM clock and the dynamic scan service
	Counter16_C_PWMClk_Start();
	Counter8_C_Service_WritePeriod(31);	// 32000 / 32 = 1000Hz
	Counter8_C_Service_EnableInt();
	Counter8_C_Service_Start();
}

void Exit_Config_C(void)
{
	M8C_DisableGInt;

	Counter8_C_Service_DisableInt();
	Counter8_C_Service_Stop();
	Counter16_C_PWMClk_Stop();

	// stop PRS8 modules
	PRS8_C_0_Stop();
	PRS8_C_1_Stop();
	PRS8_C_2_Stop();
	PRS8_C_3_Stop();
	PRS8_C_4_Stop();
	PRS8_C_5_Stop();
	PRS8_C_6_Stop();
	PRS8_C_7_Stop();

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
	if (UART_bErrCheck()) {
		UART_CmdReset();
		return;
	}

	if (UART_bCmdCheck()) {				// Wait for command    
		if(pCommand = UART_szGetParam()) {
			switch (*pCommand) {
				case 'a':	// set the analog output (anxx)
					bNumBytes = config_c_command_set_aout_column(pCommand);
					break;
				
				case 'Q':	// reboot (Q)
					bNumBytes = config_c_command_reboot();
					break;

				default:
					// seems to be an invalid command
					PutErrorStringToReplyBuffer();
					bNumBytes = 2;
					break;
			}
		}

		if (bNumBytes > 0) {
			UART_Write(_gainer.cReplyBuffer, bNumBytes);
		}

		UART_CmdReset();					// Reset command buffer
	}
}

BYTE config_c_command_set_aout_column(char *pCommand)
{
	BYTE row = 0;
	BYTE column = 0;
	BYTE value = 0;

	// {a}{n:0..8}{0:00..FF}...{7:00..FF} = {1} + {1} + {2 * 8} = 18 bytes
	if (18 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	pCommand++;
	column = HEX_TO_BYTE(*pCommand);

	if (column > 7) {
		// specified column number seems to be invalid
		PutErrorStringToReplyBuffer();
		return 2;
	}

	for (row = 0; row < 8; row++) {
		pCommand++;
		value = HEX_TO_BYTE(*pCommand);
		pCommand++;
		value = (value << 4) + HEX_TO_BYTE(*pCommand);
		_c.bData[row][column] = value;
	}

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	// TEMPORAL IMPLEMENTATION!!!
	_gainer.cReplyBuffer[0] = 'a';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}

BYTE config_c_command_reboot(void)
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
