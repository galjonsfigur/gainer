#include "gainer_common.h"

#pragma interrupt_handler ISR_Timer16_D;

/**
 * private functions of CONFIG_D
 */
void config_d_handle_commands(void);
BYTE config_d_command_get_din_all(char *pCommand, BOOL bContinuous);
BYTE config_d_command_reboot(void);

void config_d_send_din_values(BOOL bContinuous);

/**
 * private variables of CONFIG_D
 */
#define	CALIBRATION_COUNT	16	// for calibration

typedef struct {
	BYTE bCalibrationTimer[8];
	WORD wThresholdLevel[8];
	BYTE bTouched[8];
	WORD wTempValue;
	BYTE bInputNumber;
} config_d_parameters;

config_d_parameters _d;

void ISR_Timer16_D()
{
	DigBuf_D_Stop();
	Timer8_D_Stop();
	Timer16_D_Stop();

	_d.wTempValue = 0xFFFF - Timer16_D_wReadCompareValue();

	if (_d.bCalibrationTimer[_d.bInputNumber]) {
		_d.bCalibrationTimer[_d.bInputNumber]--;
		if (_d.bCalibrationTimer[_d.bInputNumber] < 12) {
			_d.wTempValue -= _d.wTempValue >> 5;
			if (_d.wThresholdLevel[_d.bInputNumber] > _d.wTempValue)
				_d.wThresholdLevel[_d.bInputNumber] = _d.wTempValue;
		}
	} else {
		if (_d.wTempValue < _d.wThresholdLevel[_d.bInputNumber]) {
			_d.bTouched[_d.bInputNumber] = TRUE;
		} else {
			_d.bTouched[_d.bInputNumber] = FALSE;
		}
	}

	_d.bInputNumber = (_d.bInputNumber + 1) & 0x07;
//	_d.bInputNumber = (_d.bInputNumber + 1) & 0x03;

	AMUX8_D_InputSelect(_d.bInputNumber);
//	AMUX4_1_InputSelect(_d.bInputNumber);

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

	_gainer.bContinuousAinRequested = FALSE;
	_gainer.bContinuousAinMask = 0x00;
	_gainer.bContinuousDinRequested = FALSE;

	_gainer.bChannels_AIN = 0;
	_gainer.bChannels_DIN = 8;
	_gainer.bChannels_AOUT = 0;
	_gainer.bChannels_DOUT = 0;

	LoadConfig_config_d();

	for (i = 0; i < 8; i++) {
		_d.bCalibrationTimer[i] = CALIBRATION_COUNT;
		_d.wThresholdLevel[i] = 0xFFFF;
		_d.bTouched[i] = FALSE;
	}
	_d.bInputNumber = AMUX8_D_PORT0_0;
//	_d.bInputNumber = AMUX4_1_PORT0_0;

	AMUX8_D_Start();
	AMUX8_D_InputSelect(_d.bInputNumber & 0x07);
//	AMUX4_1_Start();
//	AMUX4_1_InputSelect(_d.bInputNumber & 0x03);

	CMPPRG_D_Start(CMPPRG_D_HIGHPOWER);
	Timer8_D_Start();
	Timer16_D_Start();
	Timer16_D_EnableInt();

	UART_D_IntCntl(UART_D_ENABLE_RX_INT);
	UART_D_Start(UART_D_PARITY_NONE);

	M8C_EnableGInt;

	// set drive mode of P2[6] (TxD) to 'Strong'
	// DM[2:0] = '001' (Strong)
	PRT2DM2 &= ~0x40;
	PRT2DM1 &= ~0x40;
	PRT2DM0 |= 0x40;
}

void Exit_Config_D(void)
{
	M8C_DisableGInt;

	AMUX8_D_Stop();
	CMPPRG_D_Stop();
	Timer8_D_Stop();
	Timer16_D_Stop();

	UART_D_Stop();

	// set drive mode of P2[6] (TxD) to 'High-Z Analog'
	// DM[2:0] = '110' (High-Z Analog)
	PRT2DM2 |= 0x40;
	PRT2DM1 |= 0x40;
	PRT2DM0 &= ~0x40;

	UnloadConfig_config_d();
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
	if (UART_D_bErrCheck()) {
		UART_D_CmdReset();
		return;
	}

	if (UART_D_bCmdCheck()) {				// Wait for command    
		if(pCommand = UART_D_szGetParam()) {
			switch (*pCommand) {
				case 'R':	// get all digital inputs (R)
					bNumBytes = config_d_command_get_din_all(pCommand, FALSE);
					break;
				
				case 'r':	// get all digital inputs (continuous) (r)
					bNumBytes = config_d_command_get_din_all(pCommand, TRUE);
					break;

				case 'Q':	// reboot (Q)
					bNumBytes = config_d_command_reboot();
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
			UART_D_Write(_gainer.cReplyBuffer, bNumBytes);
		}

		UART_D_CmdReset();					// Reset command buffer
	}
}

BYTE config_d_command_get_din_all(char *pCommand, BOOL bContinuous)
{
#if 1
	UART_D_CPutString("7:");
	UART_D_PutSHexInt(_d.wThresholdLevel[7]);
	UART_D_PutChar(' ');
	UART_D_CPutString("5:");
	UART_D_PutSHexInt(_d.wThresholdLevel[5]);
	UART_D_PutChar(' ');
#endif

	if (1 != UART_D_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	if (_gainer.bChannels_DIN < 1) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	_gainer.bContinuousDinRequested = bContinuous;

	if (!_gainer.bContinuousDinRequested) {
		config_d_send_din_values(FALSE);
	}

	return 0;
}

BYTE config_d_command_reboot(void)
{
	if (1 != UART_D_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	_gainer.bQuitRequested = TRUE;

	_gainer.cReplyBuffer[0] = 'Q';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}

void config_d_send_din_values(BOOL bContinuous)
{
	BYTE length = 0;
	BYTE value_h = 0x00;
	BYTE value_l = 0x00;

	if (_gainer.bChannels_DIN < 1) {
		return;
	}

	value_l += _d.bTouched[7] ? 0x80 : 0x00;
	value_l += _d.bTouched[6] ? 0x40 : 0x00;
	value_l += _d.bTouched[5] ? 0x20 : 0x00;
	value_l += _d.bTouched[4] ? 0x10 : 0x00;

	value_l += _d.bTouched[3] ? 0x08 : 0x00;
	value_l += _d.bTouched[2] ? 0x04 : 0x00;
	value_l += _d.bTouched[1] ? 0x02 : 0x00;
	value_l += _d.bTouched[0] ? 0x01 : 0x00;

	_gainer.cReplyBuffer[0] = bContinuous ? 'r' : 'R';
	ByteToHex(value_h, &_gainer.cReplyBuffer[1]);	// 'x', 'x'
	ByteToHex(value_l, &_gainer.cReplyBuffer[3]);	// 'x', 'x'
	_gainer.cReplyBuffer[5] = '*';
	length = 6;

	UART_D_Write(_gainer.cReplyBuffer, length);
}
