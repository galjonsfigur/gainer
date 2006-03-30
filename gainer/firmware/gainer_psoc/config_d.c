#include "gainer_common.h"

#define D_GET_DIN_0() (PRT0DR&0x40)	// P0[6]
#define D_GET_DIN_1() (PRT0DR&0x10)	// P0[4]
#define D_GET_DIN_2() (PRT0DR&0x04)	// P0[2]
#define D_GET_DIN_3() (PRT0DR&0x01)	// P0[0]

// NOTE: P1[5] is pull-downed, so should be always ZERO!!!
#define D_SET_DOUT_0_H() (PRT2DR|=0x80)					// P2[7]
#define D_SET_DOUT_1_H() (PRT2DR|=0x20)					// P2[5]
#define D_SET_DOUT_2_H() (PRT2DR|=0x08)					// P2[3]
#define D_SET_DOUT_3_H() (PRT2DR|=0x02)					// P2[1]
#define D_SET_DOUT_4_H() (PRT2DR|=0x01)					// P2[0]
#define D_SET_DOUT_5_H() (PRT1DR=(PRT1DR&0xDF)|0x40)	// P1[6]
#define D_SET_DOUT_6_H() (PRT1DR=(PRT1DR&0xDF)|0x10)	// P1[4]
#define D_SET_DOUT_7_H() (PRT1DR=(PRT1DR&0xDF)|0x04)	// P1[2]

// NOTE: P1[5]  is pull-downed, so should be always ZERO!!!
#define D_SET_DOUT_0_L() (PRT2DR&=0x7F)	// P2[7]
#define D_SET_DOUT_1_L() (PRT2DR&=0xDF)	// P2[5]
#define D_SET_DOUT_2_L() (PRT2DR&=0xF7)	// P2[3]
#define D_SET_DOUT_3_L() (PRT2DR&=0xFD)	// P2[1]
#define D_SET_DOUT_4_L() (PRT2DR&=0xFE)	// P2[0]
#define D_SET_DOUT_5_L() (PRT1DR&=0x9F)	// P1[6]
#define D_SET_DOUT_6_L() (PRT1DR&=0xCF)	// P1[4]
#define D_SET_DOUT_7_L() (PRT1DR&=0xDB)	// P1[2]

/**
 * interrupt handler
 */
#pragma interrupt_handler Timer16_D_ISR;
void Timer16_D_ISR(void);

/**
 * private functions of CONFIG_D
 */
void config_d_handle_commands(void);
BYTE config_d_command_get_din_all(char *pCommand, BOOL bContinuous);
BYTE config_d_command_set_dout_all(char *pCommand);
//BYTE config_d_command_set_dout_ch_h(char *pCommand);
//BYTE config_d_command_set_dout_ch_l(char *pCommand);
//BYTE config_d_command_stop_cont(void);
BYTE config_d_command_set_sensitivity(char *pCommand);
BYTE config_d_command_reboot(void);

BOOL config_d_set_dout(BYTE channel, WORD value);
void config_d_send_din_values(BOOL bContinuous);

/**
 * private variables of CONFIG_D
 */
#define	CALIBRATION_COUNT	16	// for calibration
#define SENSITIVITY_MARGIN	10	// for threshold level

typedef struct {
	BYTE bCalibrationTimer[8];
	WORD wThresholdLevel[8];
	WORD wThresholdOffset[8];
	WORD wCurrentLevel[8];
	BYTE bTouched[8];
	WORD wTempValue;
	BYTE bInputNumber;
} config_d_parameters;

config_d_parameters _d;

void Timer16_D_ISR(void)
{
	DigBuf_D_Stop();
	Timer8_D_Stop();
	Timer16_D_Stop();

	_d.wTempValue = 0xFFFF - Timer16_D_wReadCompareValue();

	if (_d.bCalibrationTimer[_d.bInputNumber]) {
		_d.bCalibrationTimer[_d.bInputNumber]--;
		if (_d.bCalibrationTimer[_d.bInputNumber] < 12) {
			// TEMPORAL IMPLEMENTATION!!!
			_d.wTempValue -= _d.wTempValue >> 5;
			if (_d.wTempValue > SENSITIVITY_MARGIN) {
				_d.wTempValue -= SENSITIVITY_MARGIN;
			}

			if (_d.wThresholdLevel[_d.bInputNumber] > _d.wTempValue)
				_d.wThresholdLevel[_d.bInputNumber] = _d.wTempValue;
		}
	} else {
		if (_d.wTempValue < (_d.wThresholdLevel[_d.bInputNumber] + _d.wThresholdOffset[_d.bInputNumber])) {
			_d.bTouched[_d.bInputNumber] = TRUE;
		} else {
			_d.bTouched[_d.bInputNumber] = FALSE;
		}
		_d.wCurrentLevel[_d.bInputNumber] = _d.wTempValue;
	}

	_d.bInputNumber = (_d.bInputNumber + 1) & 0x03;

	AMUX4_D_InputSelect(_d.bInputNumber);

	Timer16_D_WritePeriod(Timer16_D_PERIOD);
	Timer16_D_WriteCompareValue(Timer16_D_COMPARE_VALUE);
	Timer8_D_WritePeriod(Timer8_D_PERIOD);
	Timer16_D_Start();
	Timer8_D_Start();
	DigBuf_D_Start();
}

void Enter_Config_D(void)
{
	BYTE i = 0;

	_gainer.bCurrentConfig = _gainer.bRequestedConfig;
	_gainer.pMainFunction = Main_Config_D;

	_gainer.bContinuousAinRequested = FALSE;
	_gainer.bContinuousAinMask = 0x00;
	_gainer.bContinuousDinRequested = FALSE;

	_gainer.bChannels_AIN = 0;
	_gainer.bChannels_DIN = 8;	// CIN[0..3] + DIN[0..3]
	_gainer.bChannels_AOUT = 0;
	_gainer.bChannels_DOUT = 8;	// DOUT[0..7]

	LoadConfig_config_d();

	for (i = 0; i < 8; i++) {
		_d.bCalibrationTimer[i] = CALIBRATION_COUNT;
		_d.wThresholdLevel[i] = 0xFFFF;
		_d.wThresholdOffset[i] = 0x0000;
		_d.bTouched[i] = FALSE;
	}
	_d.bInputNumber = AMUX4_D_PORT0_1;

	AMUX4_D_Start();
	AMUX4_D_InputSelect(_d.bInputNumber);

	CMPPRG_D_Start(CMPPRG_D_HIGHPOWER);
	Timer8_D_Start();
	Timer16_D_Start();
	Timer16_D_EnableInt();

	M8C_EnableGInt;
}

void Exit_Config_D(void)
{
	M8C_DisableGInt;

	AMUX4_D_Stop();
	CMPPRG_D_Stop();
	Timer8_D_Stop();
	Timer16_D_DisableInt();
	Timer16_D_Stop();

#if 0
	// When backed to configuration 0, Rx side of UART won't work
	UnloadConfig_config_d();
#else
	// A workaround to fix bug ID 1441410
	UnloadConfig_Total();
#endif
}

void Main_Config_D(void)
{
	config_d_handle_commands();
}

void config_d_handle_commands(void)
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
					bNumBytes = config_d_command_get_din_all(pCommand, FALSE);
					break;
/*				
				case 'r':	// get all digital inputs (continuous) (r)
					bNumBytes = config_d_command_get_din_all(pCommand, TRUE);
					break;
*/
				case 'D':	// set all digital outputs (Dxxxx)
					bNumBytes = config_d_command_set_dout_all(pCommand);
					break;
/*				
				case 'H':	// set the digital output high (Hn)
					bNumBytes = config_d_command_set_dout_ch_h(pCommand);
					break;

				case 'L':	// set the digital output low (Ln)
					bNumBytes = config_d_command_set_dout_ch_l(pCommand);
					break;

				case 'E':	// stop continuous sampling
					bNumBytes = config_d_command_stop_cont();
					break;
*/				
				case 'T':
					bNumBytes = config_d_command_set_sensitivity(pCommand);
					break;

				case 'Q':	// reboot (Q)
					bNumBytes = config_d_command_reboot();
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

BYTE config_d_command_get_din_all(char *pCommand, BOOL bContinuous)
{
#if 0
	UART_CPutString("CIN0:");
	UART_PutSHexInt(_d.wThresholdLevel[AMUX4_D_PORT0_7]);
	UART_PutChar('(');
	UART_PutSHexInt(_d.wCurrentLevel[AMUX4_D_PORT0_7]);
	UART_PutChar(')');
	UART_PutSHexByte(_d.bCalibrationTimer[AMUX4_D_PORT0_7]);
	UART_PutChar(':');
#endif

	if (1 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

#if 0
	// Since the number of din is not configurable, we don't have to do checking.
	if (_gainer.bChannels_DIN < 1) {
		PutErrorStringToReplyBuffer();
		return 2;
	}
#endif

	_gainer.bContinuousDinRequested = bContinuous;

	if (!_gainer.bContinuousDinRequested) {
		config_d_send_din_values(FALSE);
	}

	return 0;
}

BYTE config_d_command_set_dout_all(char *pCommand)
{
	BYTE i = 0;
	WORD value = 0;

	if (5 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

#if 0
	// Since the number of dout is not configurable, we don't have to do checking.
	if (_gainer.bChannels_DOUT < 1) {
		PutErrorStringToReplyBuffer();
		return 2;
	}
#endif

	value = HEX_TO_BYTE(*(pCommand + 1));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 2));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 3));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 4));

	// set all output ports
	for (i = 0; i < _gainer.bChannels_DOUT; i++) {
		config_d_set_dout(i, (value & (1 << i)));
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
/*
BYTE config_d_command_set_dout_ch_h(char *pCommand)
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

	config_d_set_dout(channel, TRUE);

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'H';
	_gainer.cReplyBuffer[1] = *(pCommand + 1);
	_gainer.cReplyBuffer[2] = '*';

	return 3;
}

BYTE config_d_command_set_dout_ch_l(char *pCommand)
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

	config_d_set_dout(channel, FALSE);

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'L';
	_gainer.cReplyBuffer[1] = *(pCommand + 1);
	_gainer.cReplyBuffer[2] = '*';

	return 3;
}

BYTE config_d_command_stop_cont(void)
{
	if (1 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	_gainer.bContinuousAinRequested = FALSE;
	_gainer.bContinuousAinMask = 0x00;
	_gainer.bContinuousDinRequested = FALSE;
	_gainer.cReplyBuffer[0] = 'E';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}
*/
BYTE config_d_command_set_sensitivity(char *pCommand)
{
	BYTE i = 0;

	if (2 != UART_bCmdLength()) {
		PutErrorStringToReplyBuffer();
		return 2;
	}

	for (i = 0; i < 8; i++) {
		_d.wThresholdOffset[i] = (WORD)(HEX_TO_BYTE(*(pCommand + 1)) * 1);
	}

	_gainer.cReplyBuffer[0] = 'T';
	_gainer.cReplyBuffer[1] = *(pCommand + 1);
	_gainer.cReplyBuffer[2] = '*';

	return 3;
}

BYTE config_d_command_reboot(void)
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

BOOL config_d_set_dout(BYTE channel, WORD value)
{
#if 0
	// Since the number of dout is not configurable, we don't have to do checking.
	if (_gainer.bChannels_DOUT < 1) {
		return FALSE;
	}
#endif

	switch (channel) {
		case 0: if (value) D_SET_DOUT_0_H(); else D_SET_DOUT_0_L(); break;
		case 1: if (value) D_SET_DOUT_1_H(); else D_SET_DOUT_1_L(); break;
		case 2: if (value) D_SET_DOUT_2_H(); else D_SET_DOUT_2_L(); break;
		case 3: if (value) D_SET_DOUT_3_H(); else D_SET_DOUT_3_L(); break;
	
		case 4: if (value) D_SET_DOUT_4_H(); else D_SET_DOUT_4_L(); break;
		case 5: if (value) D_SET_DOUT_5_H(); else D_SET_DOUT_5_L(); break;
		case 6: if (value) D_SET_DOUT_6_H(); else D_SET_DOUT_6_L(); break;
		case 7: if (value) D_SET_DOUT_7_H(); else D_SET_DOUT_7_L(); break;
	
		default: break;
	}

	return TRUE;
}

void config_d_send_din_values(BOOL bContinuous)
{
	BYTE length = 0;
	BYTE value_h = 0x00;
	BYTE value_l = 0x00;

#if 0
	// Since the number of din is not configurable, we don't have to do checking.
	if (_gainer.bChannels_DIN < 1) {
		return;
	}
#endif

	value_l += D_GET_DIN_3() ? 0x80 : 0x00;
	value_l += D_GET_DIN_2() ? 0x40 : 0x00;
	value_l += D_GET_DIN_1() ? 0x20 : 0x00;
	value_l += D_GET_DIN_0() ? 0x10 : 0x00;

	value_l += _d.bTouched[AMUX4_D_PORT0_1] ? 0x08 : 0x00;
	value_l += _d.bTouched[AMUX4_D_PORT0_3] ? 0x04 : 0x00;
	value_l += _d.bTouched[AMUX4_D_PORT0_5] ? 0x02 : 0x00;
	value_l += _d.bTouched[AMUX4_D_PORT0_7] ? 0x01 : 0x00;

	_gainer.cReplyBuffer[0] = bContinuous ? 'r' : 'R';
	ByteToHex(value_h, &_gainer.cReplyBuffer[1]);	// 'x', 'x'
	ByteToHex(value_l, &_gainer.cReplyBuffer[3]);	// 'x', 'x'
	_gainer.cReplyBuffer[5] = '*';
	length = 6;

	UART_Write(_gainer.cReplyBuffer, length);
}
