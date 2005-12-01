#include "gainer_common.h"

#define GET_DIN_01() (PRT0DR&0x80)	// P0[7]
#define GET_DIN_02() (PRT0DR&0x20)	// P0[5]
#define GET_DIN_03() (PRT0DR&0x08)	// P0[3]
#define GET_DIN_04() (PRT0DR&0x02)	// P0[1]
#define GET_DIN_05() (PRT0DR&0x40)	// P0[6]
#define GET_DIN_06() (PRT0DR&0x10)	// P0[4]
#define GET_DIN_07() (PRT0DR&0x04)	// P0[2]
#define GET_DIN_08() (PRT0DR&0x01)	// P0[0]

#define GET_DIN_09() (PRT2DR&0x80)	// P2[7]
#define GET_DIN_10() (PRT2DR&0x20)	// P2[5]
#define GET_DIN_11() (PRT2DR&0x08)	// P2[3]
#define GET_DIN_12() (PRT2DR&0x02)	// P2[1]
#define GET_DIN_13() (PRT2DR&0x01)	// P2[0]
#define GET_DIN_14() (PRT1DR&0x40)	// P1[6]
#define GET_DIN_15() (PRT1DR&0x10)	// P1[4]
#define GET_DIN_16() (PRT1DR&0x04)	// P1[2]

// NOTE: P1[5] is pull-downed, so should be always ZERO!!!
#define SET_DOUT_01_H() (PRT0DR|=0x80)				// P0[7]
#define SET_DOUT_02_H() (PRT0DR|=0x20)				// P0[5]
#define SET_DOUT_03_H() (PRT0DR|=0x08)				// P0[3]
#define SET_DOUT_04_H() (PRT0DR|=0x02)				// P0[1]
#define SET_DOUT_05_H() (PRT0DR|=0x40)				// P0[6]
#define SET_DOUT_06_H() (PRT0DR|=0x10)				// P0[4]
#define SET_DOUT_07_H() (PRT0DR|=0x04)				// P0[2]
#define SET_DOUT_08_H() (PRT0DR|=0x01)				// P0[0]

#define SET_DOUT_09_H() (PRT2DR|=0x80)				// P2[7]
#define SET_DOUT_10_H() (PRT2DR|=0x20)				// P2[5]
#define SET_DOUT_11_H() (PRT2DR|=0x08)				// P2[3]
#define SET_DOUT_12_H() (PRT2DR|=0x02)				// P2[1]
#define SET_DOUT_13_H() (PRT2DR|=0x01)				// P2[0]
#define SET_DOUT_14_H() (PRT1DR=(PRT1DR&0xDF)|0x40)	// P1[6]
#define SET_DOUT_15_H() (PRT1DR=(PRT1DR&0xDF)|0x10)	// P1[4]
#define SET_DOUT_16_H() (PRT1DR=(PRT1DR&0xDF)|0x04)	// P1[2]

// NOTE: P1[5]  is pull-downed, so should be always ZERO!!!
#define SET_DOUT_01_L() (PRT0DR&=~0x80)	// P0[7]
#define SET_DOUT_02_L() (PRT0DR&=~0x20)	// P0[5]
#define SET_DOUT_03_L() (PRT0DR&=~0x08)	// P0[3]
#define SET_DOUT_04_L() (PRT0DR&=~0x02)	// P0[1]
#define SET_DOUT_05_L() (PRT0DR&=~0x40)	// P0[6]
#define SET_DOUT_06_L() (PRT0DR&=~0x10)	// P0[4]
#define SET_DOUT_07_L() (PRT0DR&=~0x04)	// P0[2]
#define SET_DOUT_08_L() (PRT0DR&=~0x01)	// P0[0]

#define SET_DOUT_09_L() (PRT2DR&=0x7F)	// P2[7]
#define SET_DOUT_10_L() (PRT2DR&=0xDF)	// P2[5]
#define SET_DOUT_11_L() (PRT2DR&=0xF7)	// P2[3]
#define SET_DOUT_12_L() (PRT2DR&=0xFD)	// P2[1]
#define SET_DOUT_13_L() (PRT2DR&=0xFE)	// P2[0]
#define SET_DOUT_14_L() (PRT1DR&=0x9F)	// P1[6]
#define SET_DOUT_15_L() (PRT1DR&=0xCF)	// P1[4]
#define SET_DOUT_16_L() (PRT1DR&=0xDB)	// P1[2]

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

/**
 * interrupt handler
 */
#pragma interrupt_handler Counter8_B_Din_ISR
void Counter8_B_Din_ISR(void);

BYTE bDinTimerFlags = 0x00;

// this is the interrupt handler (will be called at 200Hz)
void Counter8_B_Din_ISR(void)
{
	bDinTimerFlags |= 0x01;	// ready to send din values
}

void Enter_Config_B(void)
{
	bCurrentConfig = bRequestedConfig;

	bContinuousAinRequested = FALSE;
	bContinuousAinMask = 0x00;
	bContinuousDinRequested = FALSE;

	switch (bCurrentConfig) {
		case CONFIG_4:
			// consigure number of each port type
			bChannels_AIN = 0;
			bChannels_DIN = 16;
			bChannels_AOUT = 0;
			bChannels_DOUT = 0;

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

		case CONFIG_5:
			// consigure number of each port type
			bChannels_AIN = 0;
			bChannels_DIN = 0;
			bChannels_AOUT = 0;
			bChannels_DOUT = 16;

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
	UART_B_IntCntl(UART_B_ENABLE_RX_INT);
	UART_B_Start(UART_B_PARITY_NONE);
	Counter8_B_Din_EnableInt();
	Counter8_B_Din_Start();

	// set drive mode of P2[6] (TxD) to 'Strong'
	// DM[2:0] = '001' (Strong)
	PRT2DM2 &= ~0x40;
	PRT2DM1 &= ~0x40;
	PRT2DM0 |= 0x40;
}

void Exit_Config_B(void)
{
	M8C_DisableGInt;
	UART_B_Stop();
	Counter8_B_Din_Stop();
	Counter8_B_Din_DisableInt();

	// set drive mode of P2[6] (TxD) to 'High-Z Analog'
	// DM[2:0] = '110' (High-Z Analog)
	PRT2DM2 |= 0x40;
	PRT2DM1 |= 0x40;
	PRT2DM0 &= ~0x40;

	UnloadConfig_config_b();
}

void Main_Config_B(void)
{
	if ((bContinuousDinRequested) && (bDinTimerFlags & 0x01)) {
		config_b_send_din_values(TRUE);
		bDinTimerFlags &= ~0x01;
	}

	config_b_handle_commands();
}

void config_b_handle_commands(void)
{
	char * pCommand;						// Parameter pointer
	BYTE bNumBytes = 0;

	// reset Rx buffer if it seems to be broken
	if (UART_B_bErrCheck()) {
		UART_B_CmdReset();
		return;
	}

	if (UART_B_bCmdCheck()) {				// Wait for command    
		if(pCommand = UART_B_szGetParam()) {
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
					cReplyBuffer[0] = '!';
					cReplyBuffer[1] = '*';
					bNumBytes = 2;
					break;
			}
		}

		if (bNumBytes > 0) {
			UART_B_Write(cReplyBuffer, bNumBytes);
		}

		UART_B_CmdReset();					// Reset command buffer
	}
}

BYTE config_b_command_get_din_all(char *pCommand, BOOL bContinuous)
{
	if (1 != UART_B_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	if (bChannels_DIN < 1) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	bContinuousDinRequested = bContinuous;

	if (!bContinuousDinRequested) {
		config_b_send_din_values(FALSE);
	}

	return 0;
}

BYTE config_b_command_set_dout_all(char *pCommand)
{
	WORD value = 0;

	if (5 != UART_B_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	if (bChannels_DOUT < 1) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	value = HEX_TO_BYTE(*(pCommand + 1));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 2));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 3));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 4));

	config_b_set_dout(15, (value & 0x8000));
	config_b_set_dout(14, (value & 0x4000));
	config_b_set_dout(13, (value & 0x2000));
	config_b_set_dout(12, (value & 0x1000));
	config_b_set_dout(11, (value & 0x0800));
	config_b_set_dout(10, (value & 0x0400));
	config_b_set_dout(9, (value & 0x0200));
	config_b_set_dout(8, (value & 0x0100));

	config_b_set_dout(7, (value & 0x0080));
	config_b_set_dout(6, (value & 0x0040));
	config_b_set_dout(5, (value & 0x0020));
	config_b_set_dout(4, (value & 0x0010));
	config_b_set_dout(3, (value & 0x0008));
	config_b_set_dout(2, (value & 0x0004));
	config_b_set_dout(1, (value & 0x0002));
	config_b_set_dout(0, (value & 0x0001));

	cReplyBuffer[0] = 'D';
	cReplyBuffer[1] = *(pCommand + 1);
	cReplyBuffer[2] = *(pCommand + 2);
	cReplyBuffer[3] = *(pCommand + 3);
	cReplyBuffer[4] = *(pCommand + 4);
	cReplyBuffer[5] = '*';

	return 6;
}

BYTE config_b_command_set_dout_ch_h(char *pCommand)
{
	BYTE channel = HEX_TO_BYTE(*(pCommand + 1));

	if (2 != UART_B_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	if (bChannels_DOUT < 1) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	if (channel > (bChannels_DOUT - 1)) {
		// specified channel number seems to be invalid
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	config_b_set_dout(channel, TRUE);

	cReplyBuffer[0] = 'H';
	cReplyBuffer[1] = *(pCommand + 1);
	cReplyBuffer[2] = '*';

	return 3;
}

BYTE config_b_command_set_dout_ch_l(char *pCommand)
{
	BYTE channel = HEX_TO_BYTE(*(pCommand + 1));

	if (2 != UART_B_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	if (bChannels_DOUT < 1) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	if (channel > (bChannels_DOUT - 1)) {
		// specified channel number seems to be invalid
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	config_b_set_dout(channel, FALSE);

	cReplyBuffer[0] = 'L';
	cReplyBuffer[1] = *(pCommand + 1);
	cReplyBuffer[2] = '*';

	return 3;
}

BYTE config_b_command_stop_cont(void)
{
	if (1 != UART_B_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

//	bContinuousAinRequested = FALSE;
//	bContinuousAinMask = 0x00;
	bContinuousDinRequested = FALSE;
	cReplyBuffer[0] = 'E';
	cReplyBuffer[1] = '*';

	return 2;
}

BYTE config_b_command_reboot(void)
{
	if (1 != UART_B_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	bQuitRequested = TRUE;

	cReplyBuffer[0] = 'Q';
	cReplyBuffer[1] = '*';

	return 2;
}

BOOL config_b_set_dout(BYTE channel, WORD value)
{
	if (bChannels_DOUT < 1) {
		return FALSE;
	}

	switch (channel) {
		case 0: if (value) SET_DOUT_01_H(); else SET_DOUT_01_L(); break;
		case 1: if (value) SET_DOUT_02_H(); else SET_DOUT_02_L(); break;
		case 2: if (value) SET_DOUT_03_H(); else SET_DOUT_03_L(); break;
		case 3: if (value) SET_DOUT_04_H(); else SET_DOUT_04_L(); break;
	
		case 4: if (value) SET_DOUT_05_H(); else SET_DOUT_05_L(); break;
		case 5: if (value) SET_DOUT_06_H(); else SET_DOUT_06_L(); break;
		case 6: if (value) SET_DOUT_07_H(); else SET_DOUT_07_L(); break;
		case 7: if (value) SET_DOUT_08_H(); else SET_DOUT_08_L(); break;
	
		case 8: if (value) SET_DOUT_09_H(); else SET_DOUT_09_L(); break;
		case 9: if (value) SET_DOUT_10_H(); else SET_DOUT_10_L(); break;
		case 10: if (value) SET_DOUT_11_H(); else SET_DOUT_11_L(); break;
		case 11: if (value) SET_DOUT_12_H(); else SET_DOUT_12_L(); break;
	
		case 12: if (value) SET_DOUT_13_H(); else SET_DOUT_13_L(); break;
		case 13: if (value) SET_DOUT_14_H(); else SET_DOUT_14_L(); break;
		case 14: if (value) SET_DOUT_15_H(); else SET_DOUT_15_L(); break;
		case 15: if (value) SET_DOUT_16_H(); else SET_DOUT_16_L(); break;
	
		default: break;
	}

	return TRUE;
}

void config_b_send_din_values(BOOL bContinuous)
{
	BYTE length = 0;
	BYTE value_h = 0x00;
	BYTE value_l = 0x00;

	if (bChannels_DIN < 1) {
		return;
	}

	value_h += GET_DIN_16() ? 0x80 : 0x00;
	value_h += GET_DIN_15() ? 0x40 : 0x00;
	value_h += GET_DIN_14() ? 0x20 : 0x00;
	value_h += GET_DIN_13() ? 0x10 : 0x00;

	value_h += GET_DIN_12() ? 0x08 : 0x00;
	value_h += GET_DIN_11() ? 0x04 : 0x00;
	value_h += GET_DIN_10() ? 0x02 : 0x00;
	value_h += GET_DIN_09() ? 0x01 : 0x00;

	value_l += GET_DIN_08() ? 0x80 : 0x00;
	value_l += GET_DIN_07() ? 0x40 : 0x00;
	value_l += GET_DIN_06() ? 0x20 : 0x00;
	value_l += GET_DIN_05() ? 0x10 : 0x00;

	value_l += GET_DIN_04() ? 0x08 : 0x00;
	value_l += GET_DIN_03() ? 0x04 : 0x00;
	value_l += GET_DIN_02() ? 0x02 : 0x00;
	value_l += GET_DIN_01() ? 0x01 : 0x00;

	cReplyBuffer[0] = bContinuous ? 'r' : 'R';
	ByteToHex(value_h, &cReplyBuffer[1]);	// 'x', 'x'
	ByteToHex(value_l, &cReplyBuffer[3]);	// 'x', 'x'
	cReplyBuffer[5] = '*';
	length = 6;

	UART_B_Write(cReplyBuffer, length);
}
