#include "gainer_common.h"

#define B_GET_DIN_01() (PRT0DR&0x80)	// P0[7]
#define B_GET_DIN_02() (PRT0DR&0x20)	// P0[5]
#define B_GET_DIN_03() (PRT0DR&0x08)	// P0[3]
#define B_GET_DIN_04() (PRT0DR&0x02)	// P0[1]
#define B_GET_DIN_05() (PRT0DR&0x40)	// P0[6]
#define B_GET_DIN_06() (PRT0DR&0x10)	// P0[4]
#define B_GET_DIN_07() (PRT0DR&0x04)	// P0[2]
#define B_GET_DIN_08() (PRT0DR&0x01)	// P0[0]

#define B_GET_DIN_09() (PRT2DR&0x80)	// P2[7]
#define B_GET_DIN_10() (PRT2DR&0x20)	// P2[5]
#define B_GET_DIN_11() (PRT2DR&0x08)	// P2[3]
#define B_GET_DIN_12() (PRT2DR&0x02)	// P2[1]
#define B_GET_DIN_13() (PRT2DR&0x01)	// P2[0]
#define B_GET_DIN_14() (PRT1DR&0x40)	// P1[6]
#define B_GET_DIN_15() (PRT1DR&0x10)	// P1[4]
#define B_GET_DIN_16() (PRT1DR&0x04)	// P1[2]

// NOTE: P1[5] is pull-downed, so should be always ZERO!!!
#define B_SET_DOUT_01_H() (PRT0DR|=0x80)				// P0[7]
#define B_SET_DOUT_02_H() (PRT0DR|=0x20)				// P0[5]
#define B_SET_DOUT_03_H() (PRT0DR|=0x08)				// P0[3]
#define B_SET_DOUT_04_H() (PRT0DR|=0x02)				// P0[1]
#define B_SET_DOUT_05_H() (PRT0DR|=0x40)				// P0[6]
#define B_SET_DOUT_06_H() (PRT0DR|=0x10)				// P0[4]
#define B_SET_DOUT_07_H() (PRT0DR|=0x04)				// P0[2]
#define B_SET_DOUT_08_H() (PRT0DR|=0x01)				// P0[0]

#define B_SET_DOUT_09_H() (PRT2DR|=0x80)				// P2[7]
#define B_SET_DOUT_10_H() (PRT2DR|=0x20)				// P2[5]
#define B_SET_DOUT_11_H() (PRT2DR|=0x08)				// P2[3]
#define B_SET_DOUT_12_H() (PRT2DR|=0x02)				// P2[1]
#define B_SET_DOUT_13_H() (PRT2DR|=0x01)				// P2[0]
#define B_SET_DOUT_14_H() (PRT1DR=(PRT1DR&0xDF)|0x40)	// P1[6]
#define B_SET_DOUT_15_H() (PRT1DR=(PRT1DR&0xDF)|0x10)	// P1[4]
#define B_SET_DOUT_16_H() (PRT1DR=(PRT1DR&0xDF)|0x04)	// P1[2]

// NOTE: P1[5]  is pull-downed, so should be always ZERO!!!
#define B_SET_DOUT_01_L() (PRT0DR&=~0x80)	// P0[7]
#define B_SET_DOUT_02_L() (PRT0DR&=~0x20)	// P0[5]
#define B_SET_DOUT_03_L() (PRT0DR&=~0x08)	// P0[3]
#define B_SET_DOUT_04_L() (PRT0DR&=~0x02)	// P0[1]
#define B_SET_DOUT_05_L() (PRT0DR&=~0x40)	// P0[6]
#define B_SET_DOUT_06_L() (PRT0DR&=~0x10)	// P0[4]
#define B_SET_DOUT_07_L() (PRT0DR&=~0x04)	// P0[2]
#define B_SET_DOUT_08_L() (PRT0DR&=~0x01)	// P0[0]

#define B_SET_DOUT_09_L() (PRT2DR&=0x7F)	// P2[7]
#define B_SET_DOUT_10_L() (PRT2DR&=0xDF)	// P2[5]
#define B_SET_DOUT_11_L() (PRT2DR&=0xF7)	// P2[3]
#define B_SET_DOUT_12_L() (PRT2DR&=0xFD)	// P2[1]
#define B_SET_DOUT_13_L() (PRT2DR&=0xFE)	// P2[0]
#define B_SET_DOUT_14_L() (PRT1DR&=0x9F)	// P1[6]
#define B_SET_DOUT_15_L() (PRT1DR&=0xCF)	// P1[4]
#define B_SET_DOUT_16_L() (PRT1DR&=0xDB)	// P1[2]

/**
 * private functions of CONFIG_B
 */
void config_b_handle_commands(void);

BYTE config_b_command_get_din_all(char *pCommand, BOOL bContinuous);
BYTE config_b_command_set_dout_all(char *pCommand);
BYTE config_b_command_set_dout_ch_h(char *pCommand);
BYTE config_b_command_set_dout_ch_l(char *pCommand);
BYTE config_b_command_stop_cont(void);

BYTE config_b_command_reboot(void);

BOOL config_b_set_dout(BYTE channel, WORD value);
void config_b_send_din_values(BOOL bContinuous);

void config_b_init_output_ports(void);

/**
 * interrupt handler
 */
#pragma interrupt_handler Counter8_B_Din_ISR
void Counter8_B_Din_ISR(void);

/**
 * private variables of CONFIG_B
 */
typedef struct {
	BYTE bDinTimerFlags;
} config_b_parameters;

config_b_parameters _b;

// this is the interrupt handler (will be called at 200Hz)
void Counter8_B_Din_ISR(void)
{
	_b.bDinTimerFlags |= 0x01;	// ready to send din values
}

void Enter_Config_B(void)
{
	_gainer.bCurrentConfig = _gainer.bRequestedConfig;
	_gainer.pMainFunction = Main_Config_B;

	_gainer.bContinuousAinRequested = FALSE;
	_gainer.bContinuousAinMask = 0x00;
	_gainer.bContinuousDinRequested = FALSE;

	_b.bDinTimerFlags = 0x00;

	switch (_gainer.bCurrentConfig) {
		case CONFIG_5:
			// consigure number of each port type
			_gainer.bChannels_AIN = 0;
			_gainer.bChannels_DIN = 16;
			_gainer.bChannels_AOUT = 0;
			_gainer.bChannels_DOUT = 0;

			// change drive mode of *in 1~8 to 'Pull Down' (DM[2:0] = '000')
			PRT0DM2 = 0x00;		// P0[7:0]
			PRT0DM1 = 0x00;		// P0[7:0]
			PRT0DM0 = 0x00;		// P0[7:0]

			// change drive mode of *in 9~16 to 'Pull Down' (DM[2:0] = '000')
			PRT1DM2 &= ~0x54;	// P1[2], P1[4] and P1[6]
			PRT1DM1 &= ~0x54;	// P1[2], P1[4] and P1[6]
			PRT1DM0 &= ~0x54;	// P1[2], P1[4] and P1[6]
			PRT2DM2 &= ~0xAB;	// P2[0], P2[1], P2[3], P2[5] and P2[7]
			PRT2DM1 &= ~0xAB;	// P2[0], P2[1], P2[3], P2[5] and P2[7]
			PRT2DM0 &= ~0xAB;	// P2[0], P2[1], P2[3], P2[5] and P2[7]
			break;

		case CONFIG_6:
			// consigure number of each port type
			_gainer.bChannels_AIN = 0;
			_gainer.bChannels_DIN = 0;
			_gainer.bChannels_AOUT = 0;
			_gainer.bChannels_DOUT = 16;

			// change drive mode of *in 1~8 to 'Strong' (DM[2:0] = '001')
			PRT0DM2 = 0x00;		// P0[7:0]
			PRT0DM1 = 0x00;		// P0[7:0]
			PRT0DM0 = 0xFF;		// P0[7:0]

			// change drive mode of *in 9~16 to 'Strong' (DM[2:0] = '001')
			PRT1DM2 &= ~0x54;	// P1[2], P1[4] and P1[6]
			PRT1DM1 &= ~0x54;	// P1[2], P1[4] and P1[6]
			PRT1DM0 |= 0x54;	// P1[2], P1[4] and P1[6]
			PRT2DM2 &= ~0xAB;	// P2[0], P2[1], P2[3], P2[5] and P2[7]
			PRT2DM1 &= ~0xAB;	// P2[0], P2[1], P2[3], P2[5] and P2[7]
			PRT2DM0 |= 0xAB;	// P2[0], P2[1], P2[3], P2[5] and P2[7]
			break;

		default:
			break;
	}

	LoadConfig_config_b();

	M8C_EnableGInt;

	Counter8_B_Din_EnableInt();
	Counter8_B_Din_Start();

	config_b_init_output_ports();
}

void Exit_Config_B(void)
{
	config_b_init_output_ports();

	Counter8_B_Din_DisableInt();
	Counter8_B_Din_Stop();

	M8C_DisableGInt;

	UnloadConfig_config_b();
}

void Main_Config_B(void)
{
	if ((_gainer.bContinuousDinRequested) && (_b.bDinTimerFlags & 0x01)) {
		config_b_send_din_values(TRUE);
		_b.bDinTimerFlags &= ~0x01;
	}

	config_b_handle_commands();
}

void config_b_handle_commands(void)
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
				case 'R':	// get all digital inputs (R)
					bNumBytes = config_b_command_get_din_all(pCommand, FALSE);
					break;
				
				case 'r':	// get all digital inputs (continuous) (r)
					bNumBytes = config_b_command_get_din_all(pCommand, TRUE);
					break;

				case 'D':	// set all digital outputs (Dxxxx)
					bNumBytes = config_b_command_set_dout_all(pCommand);
					break;
				
				case 'H':	// set the digital output high (Hn)
					bNumBytes = config_b_command_set_dout_ch_h(pCommand);
					break;
				
				case 'L':	// set the digital output low (Ln)
					bNumBytes = config_b_command_set_dout_ch_l(pCommand);
					break;
				
				case 'E':	// stop continuous sampling
					bNumBytes = config_b_command_stop_cont();
					break;
				
				case 'Q':	// reboot (Q)
					bNumBytes = config_b_command_reboot();
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

BYTE config_b_command_get_din_all(char *pCommand, BOOL bContinuous)
{
	if (1 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	if (_gainer.bChannels_DIN < 1) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	_gainer.bContinuousDinRequested = bContinuous;

	if (!_gainer.bContinuousDinRequested) {
		config_b_send_din_values(FALSE);
	}

	return 0;
}

BYTE config_b_command_set_dout_all(char *pCommand)
{
	BYTE i = 0;
	WORD value = 0;

	if (5 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	if (_gainer.bChannels_DOUT < 1) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	value = HEX_TO_BYTE(*(pCommand + 1));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 2));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 3));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 4));

	// set all output ports
	for (i = 0; i < _gainer.bChannels_DOUT; i++) {
		config_b_set_dout(i, (value & (1 << i)));
	}

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'D';
	_gainer.cReplyBuffer[1] = *(pCommand + 1);
	_gainer.cReplyBuffer[2] = *(pCommand + 2);
	_gainer.cReplyBuffer[3] = *(pCommand + 3);
	_gainer.cReplyBuffer[4] = *(pCommand + 4);
	_gainer.cReplyBuffer[5] = '*';

	return 6;
}

BYTE config_b_command_set_dout_ch_h(char *pCommand)
{
	BYTE channel = HEX_TO_BYTE(*(pCommand + 1));

	if (2 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	if (_gainer.bChannels_DOUT < 1) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	if (channel > (_gainer.bChannels_DOUT - 1)) {
		// specified channel number seems to be invalid
		PutErrorStringToReplyBuffer();
		return 2;
	}

	config_b_set_dout(channel, TRUE);

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'H';
	_gainer.cReplyBuffer[1] = *(pCommand + 1);
	_gainer.cReplyBuffer[2] = '*';

	return 3;
}

BYTE config_b_command_set_dout_ch_l(char *pCommand)
{
	BYTE channel = HEX_TO_BYTE(*(pCommand + 1));

	if (2 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	if (_gainer.bChannels_DOUT < 1) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	if (channel > (_gainer.bChannels_DOUT - 1)) {
		// specified channel number seems to be invalid
		PutErrorStringToReplyBuffer();
		return 2;
	}

	config_b_set_dout(channel, FALSE);

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'L';
	_gainer.cReplyBuffer[1] = *(pCommand + 1);
	_gainer.cReplyBuffer[2] = '*';

	return 3;
}

BYTE config_b_command_stop_cont(void)
{
	if (1 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

//	bContinuousAinRequested = FALSE;
//	bContinuousAinMask = 0x00;
	_gainer.bContinuousDinRequested = FALSE;
	_gainer.cReplyBuffer[0] = 'E';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}

BYTE config_b_command_reboot(void)
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

BOOL config_b_set_dout(BYTE channel, WORD value)
{
	if (_gainer.bChannels_DOUT < 1) {
		return FALSE;
	}

	switch (channel) {
		case 0: if (value) B_SET_DOUT_01_H(); else B_SET_DOUT_01_L(); break;
		case 1: if (value) B_SET_DOUT_02_H(); else B_SET_DOUT_02_L(); break;
		case 2: if (value) B_SET_DOUT_03_H(); else B_SET_DOUT_03_L(); break;
		case 3: if (value) B_SET_DOUT_04_H(); else B_SET_DOUT_04_L(); break;
	
		case 4: if (value) B_SET_DOUT_05_H(); else B_SET_DOUT_05_L(); break;
		case 5: if (value) B_SET_DOUT_06_H(); else B_SET_DOUT_06_L(); break;
		case 6: if (value) B_SET_DOUT_07_H(); else B_SET_DOUT_07_L(); break;
		case 7: if (value) B_SET_DOUT_08_H(); else B_SET_DOUT_08_L(); break;
	
		case 8: if (value) B_SET_DOUT_09_H(); else B_SET_DOUT_09_L(); break;
		case 9: if (value) B_SET_DOUT_10_H(); else B_SET_DOUT_10_L(); break;
		case 10: if (value) B_SET_DOUT_11_H(); else B_SET_DOUT_11_L(); break;
		case 11: if (value) B_SET_DOUT_12_H(); else B_SET_DOUT_12_L(); break;
	
		case 12: if (value) B_SET_DOUT_13_H(); else B_SET_DOUT_13_L(); break;
		case 13: if (value) B_SET_DOUT_14_H(); else B_SET_DOUT_14_L(); break;
		case 14: if (value) B_SET_DOUT_15_H(); else B_SET_DOUT_15_L(); break;
		case 15: if (value) B_SET_DOUT_16_H(); else B_SET_DOUT_16_L(); break;
	
		default: break;
	}

	return TRUE;
}

void config_b_send_din_values(BOOL bContinuous)
{
	BYTE length = 0;
	BYTE value_h = 0x00;
	BYTE value_l = 0x00;

	if (_gainer.bChannels_DIN < 1) {
		return;
	}

	value_h += B_GET_DIN_16() ? 0x80 : 0x00;
	value_h += B_GET_DIN_15() ? 0x40 : 0x00;
	value_h += B_GET_DIN_14() ? 0x20 : 0x00;
	value_h += B_GET_DIN_13() ? 0x10 : 0x00;

	value_h += B_GET_DIN_12() ? 0x08 : 0x00;
	value_h += B_GET_DIN_11() ? 0x04 : 0x00;
	value_h += B_GET_DIN_10() ? 0x02 : 0x00;
	value_h += B_GET_DIN_09() ? 0x01 : 0x00;

	value_l += B_GET_DIN_08() ? 0x80 : 0x00;
	value_l += B_GET_DIN_07() ? 0x40 : 0x00;
	value_l += B_GET_DIN_06() ? 0x20 : 0x00;
	value_l += B_GET_DIN_05() ? 0x10 : 0x00;

	value_l += B_GET_DIN_04() ? 0x08 : 0x00;
	value_l += B_GET_DIN_03() ? 0x04 : 0x00;
	value_l += B_GET_DIN_02() ? 0x02 : 0x00;
	value_l += B_GET_DIN_01() ? 0x01 : 0x00;

	_gainer.cReplyBuffer[0] = bContinuous ? 'r' : 'R';
	ByteToHex(value_h, &_gainer.cReplyBuffer[1]);	// 'x', 'x'
	ByteToHex(value_l, &_gainer.cReplyBuffer[3]);	// 'x', 'x'
	_gainer.cReplyBuffer[5] = '*';
	length = 6;

	UART_Write(_gainer.cReplyBuffer, length);
}

void config_b_init_output_ports(void)
{
	BYTE i = 0;

	// initialize digital outputs
	for (i = 0; i < _gainer.bChannels_DOUT; i++) {
		config_b_set_dout(i, FALSE);
	}
}
