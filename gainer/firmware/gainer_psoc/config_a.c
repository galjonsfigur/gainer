#include "gainer_common.h"

#define A_GET_DIN_1() (PRT0DR&0x40)	// P0[6]
#define A_GET_DIN_2() (PRT0DR&0x10)	// P0[4]
#define A_GET_DIN_3() (PRT0DR&0x04)	// P0[2]
#define A_GET_DIN_4() (PRT0DR&0x01)	// P0[0]
#define A_GET_DIN_5() (PRT0DR&0x80)	// P0[7]
#define A_GET_DIN_6() (PRT0DR&0x20)	// P0[5]
#define A_GET_DIN_7() (PRT0DR&0x08)	// P0[3]
#define A_GET_DIN_8() (PRT0DR&0x02)	// P0[1]

// NOTE: P1[5] is pull-downed, so should be always ZERO!!!
#define A_SET_DOUT_1_H() (PRT2DR|=0x01)				// P2[0]
#define A_SET_DOUT_2_H() (PRT1DR=(PRT1DR&0xDF)|0x40)	// P1[6]
#define A_SET_DOUT_3_H() (PRT1DR=(PRT1DR&0xDF)|0x10)	// P1[4]
#define A_SET_DOUT_4_H() (PRT1DR=(PRT1DR&0xDF)|0x04)	// P1[2]
#define A_SET_DOUT_5_H() (PRT2DR|=0x80)				// P2[7]
#define A_SET_DOUT_6_H() (PRT2DR|=0x20)				// P2[5]
#define A_SET_DOUT_7_H() (PRT2DR|=0x08)				// P2[3]
#define A_SET_DOUT_8_H() (PRT2DR|=0x02)				// P2[1]

// NOTE: P1[5]  is pull-downed, so should be always ZERO!!!
#define A_SET_DOUT_1_L() (PRT2DR&=0xFE)	// P2[0]
#define A_SET_DOUT_2_L() (PRT1DR&=0x9F)	// P1[6]
#define A_SET_DOUT_3_L() (PRT1DR&=0xCF)	// P1[4]
#define A_SET_DOUT_4_L() (PRT1DR&=0xDB)	// P1[2]
#define A_SET_DOUT_5_L() (PRT2DR&=0x7F)	// P2[7]
#define A_SET_DOUT_6_L() (PRT2DR&=0xDF)	// P2[5]
#define A_SET_DOUT_7_L() (PRT2DR&=0xF7)	// P2[3]
#define A_SET_DOUT_8_L() (PRT2DR&=0xFD)	// P2[1]

/**
 * private functions of CONFIG_A
 */
void handle_ain_event(void);
void handle_button_event(void);
void handle_commands_config_a(void);

BYTE command_set_dout_all(char *pCommand);
BYTE command_set_dout_h(char *pCommand);
BYTE command_set_dout_l(char *pCommand);
BYTE command_set_aout_all(char *pCommand);
BYTE command_set_aout_ch(char *pCommand);
BYTE command_get_din_all(char *pCommand, BOOL bContinuous);
BYTE command_get_ain_all(char *pCommand, BOOL bContinuous);
BYTE command_get_ain_ch(char *pCommand, BOOL bContinuous);
BYTE command_stop_cont(void);
BYTE command_set_led_h(void);
BYTE command_set_led_l(void);
BYTE command_set_gain(char *pCommand);

BYTE command_reboot_a(void);

BOOL set_aout(BYTE channel, BYTE value);
BOOL set_dout(BYTE channel, WORD value);
void send_ain_values(void);
void send_din_values(void);

void init_output_ports(void);

/**
 * private variables of CONFIG_A
 */
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

typedef struct {
	WORD wAdcValue[16];
	BYTE bAdcChannelNumber;
	BYTE bAdcFlags;
	BYTE bAnalogInput[8];
	BOOL bButtonWasOn;
	BOOL bButtonIsOn;
} config_a_parameters;

config_a_parameters _a;

void Enter_Config_A(void)
{
	BYTE i = 0;

	_gainer.bCurrentConfig = _gainer.bRequestedConfig;

	LoadConfig_config_a();

	_gainer.bContinuousAinRequested = FALSE;
	_gainer.bContinuousAinMask = 0x00;
	_gainer.bContinuousDinRequested = FALSE;

	_a.bAdcChannelNumber = 0;
	_a.bButtonWasOn = FALSE;
	_a.bButtonIsOn = FALSE;

	switch (_gainer.bCurrentConfig) {
		case CONFIG_2:
			// consigure number of each port type
			_gainer.bChannels_AIN = 8;
			_gainer.bChannels_DIN = 0;
			_gainer.bChannels_AOUT = 4;
			_gainer.bChannels_DOUT = 4;

			// change drive mode of *in 1~8 to 'High-Z Analog' (DM[2:0] = '110')
			PRT0DM2 = 0xFF;
			PRT0DM1 = 0xFF;
			PRT0DM0 = 0x00;
			// change select of *in 1~8 to 'Analog'
			// <nothing to do!?>
			break;

		case CONFIG_3:
			// consigure number of each port type
			_gainer.bChannels_AIN = 4;
			_gainer.bChannels_DIN = 4;
			_gainer.bChannels_AOUT = 8;
			_gainer.bChannels_DOUT = 0;

			// connect PWM8 modules to *out 5~8
			// enable global select (enable global bypass)
			PRT1GS |= 0x54;		// connect P1[2], P1[4] and P1[6] to global bus
			PRT2GS |= 0x01;		// connect P2[0] to global bus
			break;

		case CONFIG_4:
			// consigure number of each port type
			_gainer.bChannels_AIN = 8;
			_gainer.bChannels_DIN = 0;
			_gainer.bChannels_AOUT = 8;
			_gainer.bChannels_DOUT = 0;

			// change drive mode of *in 1~8 to 'High-Z Analog' (DM[2:0] = '110')
			PRT0DM2 = 0xFF;
			PRT0DM1 = 0xFF;
			PRT0DM0 = 0x00;

			// connect PWM8 modules to *out 5~8
			// enable global select (enable global bypass)
			PRT1GS |= 0x54;		// connect P1[2], P1[4] and P1[6] to global bus
			PRT2GS |= 0x01;		// connect P2[0] to global bus
			break;

#if 0	// ***** UNUSED *****
		case CONFIG_3:
			// consigure number of each port type
			bChannels_AIN = 0;
			bChannels_DIN = 8;
			bChannels_AOUT = 4;
			bChannels_DOUT = 4;

			// change drive mode of *in 1~8 to 'Pull Down' (DM[2:0] = '000')
			PRT0DM2 = 0x00;
			PRT0DM1 = 0x00;
			PRT0DM0 = 0x00;
			// change select of *in 1~8 to 'StdCPU'
			// <nothing to do!?>
			break;

		case CONFIG_5:
			// consigure number of each port type
			bChannels_AIN = 4;
			bChannels_DIN = 4;
			bChannels_AOUT = 0;
			bChannels_DOUT = 8;

			// disconnect PWM8 modules from *out 1~4
			// disable global select (disable global bypass)
			PRT2GS &= 0x55;		// disconnect P2[1], P2[3], P2[5] and P2[7] from global bus
			break;
#endif	// ***** UNUSED *****
		default:
			_gainer.bChannels_AIN = 4;
			_gainer.bChannels_DIN = 4;
			_gainer.bChannels_AOUT = 4;
			_gainer.bChannels_DOUT = 4;
			break;
	}

	// start UART
//	UART_A_IntCntl(UART_A_ENABLE_RX_INT | UART_A_ENABLE_TX_INT);
	UART_A_IntCntl(UART_A_ENABLE_RX_INT);
	UART_A_Start(UART_A_PARITY_NONE);

	// set drive mode of P2[6] (TxD) to 'Strong'
	// DM[2:0] = '001' (Strong)
	PRT2DM2 &= ~0x40;
	PRT2DM1 &= ~0x40;
	PRT2DM0 |= 0x40;

	// start analog inputs
	AMUX4_A_1_Start();
	AMUX4_A_2_Start();
	PGA_A_1_SetGain(bGainTable[0]);
	PGA_A_2_SetGain(bGainTable[0]);
	PGA_A_1_Start(PGA_A_1_LOWPOWER);		// see the module datasheet
	PGA_A_2_Start(PGA_A_2_LOWPOWER);		// might be good lpf!?
	DUALADC_A_Start(DUALADC_A_HIGHPOWER);	// try lower power later

	// start analog outputs
	Counter16_A_PWMClk_WritePeriod(7);	// 1000Hz (i.e. 1ms)
	Counter16_A_PWMClk_WriteCompareValue(0);
	Counter16_A_PWMClk_Start();

	// set period of all PWM8 modules to 254
	// NOTE: THIS IS JUST AN WORKSROUND!!!
	PWM8_A_1_WritePeriod(254);
	PWM8_A_2_WritePeriod(254);
	PWM8_A_3_WritePeriod(254);
	PWM8_A_4_WritePeriod(254);
	PWM8_A_5_WritePeriod(254);
	PWM8_A_6_WritePeriod(254);
	PWM8_A_7_WritePeriod(254);
	PWM8_A_8_WritePeriod(254);

	// set pulse width of all PWM8 modules to 0
	PWM8_A_1_WritePulseWidth(0);
	PWM8_A_2_WritePulseWidth(0);
	PWM8_A_3_WritePulseWidth(0);
	PWM8_A_4_WritePulseWidth(0);
	PWM8_A_5_WritePulseWidth(0);
	PWM8_A_6_WritePulseWidth(0);
	PWM8_A_7_WritePulseWidth(0);
	PWM8_A_8_WritePulseWidth(0);

	// enable used analog (actually PWM) outputs
	switch (_gainer.bChannels_AOUT) {
		case 4:
			PWM8_A_1_Start();
			PWM8_A_2_Start();
			PWM8_A_3_Start();
			PWM8_A_4_Start();
			break;

		case 8:
			PWM8_A_1_Start();
			PWM8_A_2_Start();
			PWM8_A_3_Start();
			PWM8_A_4_Start();
			PWM8_A_5_Start();
			PWM8_A_6_Start();
			PWM8_A_7_Start();
			PWM8_A_8_Start();
			break;

		default:
			break;
	}

	init_output_ports();

	M8C_EnableGInt;
}

void Exit_Config_A(void)
{
	init_output_ports();

	M8C_DisableGInt;

	// stop UART
	UART_A_Stop();

	// set drive mode of P2[6] (TxD) to 'High-Z Analog'
	// DM[2:0] = '110' (High-Z Analog)
	PRT2DM2 |= 0x40;
	PRT2DM1 |= 0x40;
	PRT2DM0 &= ~0x40;

	// stop analog inputs
	AMUX4_A_1_Stop();
	AMUX4_A_2_Stop();
	PGA_A_1_Stop();
	PGA_A_2_Stop();
	DUALADC_A_Stop();

	// stop analog outputs
	Counter16_A_PWMClk_Stop();
	PWM8_A_1_Stop();
	PWM8_A_2_Stop();
	PWM8_A_3_Stop();
	PWM8_A_4_Stop();
	PWM8_A_5_Stop();
	PWM8_A_6_Stop();
	PWM8_A_7_Stop();
	PWM8_A_8_Stop();

	UnloadConfig_config_a();
}

void Main_Config_A(void)
{
	handle_ain_event();
	handle_button_event();
	handle_commands_config_a();
}

void handle_ain_event(void)
{
	AMUX4_A_1_InputSelect(_a.bAdcChannelNumber);	// ain 1~4
	AMUX4_A_2_InputSelect(_a.bAdcChannelNumber);	// ain 5~8
	
	DUALADC_A_GetSamples(1);	// continuous sampling
	while(DUALADC_A_fIsDataAvailable() == 0);  // Wait for data to be ready
	_a.wAdcValue[_a.bAdcChannelNumber] = DUALADC_A_iGetData1ClearFlag();		// ain 1~4
	_a.wAdcValue[_a.bAdcChannelNumber + 4] = DUALADC_A_iGetData2ClearFlag();	// ain 5~8

	_a.bAdcChannelNumber++;
	if (_a.bAdcChannelNumber == 4) {
		_a.bAdcChannelNumber = 0;
		_a.bAdcFlags |= 0x01;
	}

	// Check if all the 4 channels have been measured
	if (_a.bAdcFlags & 0x01) {
		// ain 1~4
		_a.bAnalogInput[3] = (BYTE)(_a.wAdcValue[0] & 0x00FF);
		_a.bAnalogInput[2] = (BYTE)(_a.wAdcValue[1] & 0x00FF);
		_a.bAnalogInput[1] = (BYTE)(_a.wAdcValue[2] & 0x00FF);
		_a.bAnalogInput[0] = (BYTE)(_a.wAdcValue[3] & 0x00FF);

		// ain 5~8
		_a.bAnalogInput[7] = (BYTE)(_a.wAdcValue[4] & 0x00FF);
		_a.bAnalogInput[6] = (BYTE)(_a.wAdcValue[5] & 0x00FF);
		_a.bAnalogInput[5] = (BYTE)(_a.wAdcValue[6] & 0x00FF);
		_a.bAnalogInput[4] = (BYTE)(_a.wAdcValue[7] & 0x00FF);

		_a.bAdcFlags &= ~0x01;

		// send ain data when measured all channels
		if (_gainer.bContinuousAinRequested) {
			send_ain_values();
		}

		// send din data here to avaiod stupid sending
		if (_gainer.bContinuousDinRequested) {
			send_din_values();
		}
	}
}

void handle_button_event(void)
{
	_a.bButtonIsOn = GET_BUTTON();

	if (_a.bButtonIsOn != _a.bButtonWasOn) {
		_gainer.cReplyBuffer[0] = _a.bButtonIsOn ? 'N' : 'F';
		_gainer.cReplyBuffer[1] = '*';
		UART_A_Write(_gainer.cReplyBuffer, 2);
	}

	_a.bButtonWasOn = _a.bButtonIsOn;
}

void handle_commands_config_a(void)
{
	char * pCommand;						// Parameter pointer
	BYTE bNumBytes = 0;

	// reset Rx buffer if it seems to be broken
	if (UART_A_bErrCheck()) {
		UART_A_CmdReset();
		return;
	}

	if (UART_A_bCmdCheck()) {				// Wait for command    
		if(pCommand = UART_A_szGetParam()) {
			switch (*pCommand) {
				case 'D':	// set all digital outputs (Dxx)
					bNumBytes = command_set_dout_all(pCommand);
					break;
				
				case 'H':	// set the digital output high (Hn)
					bNumBytes = command_set_dout_h(pCommand);
					break;
				
				case 'L':	// set the digital output low (Ln)
					bNumBytes = command_set_dout_l(pCommand);
					break;
				
				case 'R':	// get all digital inputs (R)
					bNumBytes = command_get_din_all(pCommand, FALSE);
					break;
				
				case 'r':	// get all digital inputs (continuous) (r)
					bNumBytes = command_get_din_all(pCommand, TRUE);
					break;
				
				case 'A':	// set all analog outputs (Axx...xx)
					bNumBytes = command_set_aout_all(pCommand);
					break;
				
				case 'a':	// set the analog output (anxx)
					bNumBytes = command_set_aout_ch(pCommand);
					break;
				
				case 'I':	// get all analog inputs (I)
					bNumBytes = command_get_ain_all(pCommand, FALSE);
					break;
				
				case 'i':	// get all analog inputs (continuous) (i)
					bNumBytes = command_get_ain_all(pCommand, TRUE);
					break;
				
				case 'S':	// get the analog input (Sn)
					bNumBytes = command_get_ain_ch(pCommand, FALSE);
					break;
				
				case 's':	// get the analog input (continuous) (sn)
					bNumBytes = command_get_ain_ch(pCommand, TRUE);
					break;
				
				case 'E':	// stop continuous sampling
					bNumBytes = command_stop_cont();
					break;
				
				case 'h':	// set the on-board LED high (h)
					bNumBytes = command_set_led_h();
					break;
				
				case 'l':	// set the on-board LED low (l)
					bNumBytes = command_set_led_l();
					break;
				
				case 'G':	// set gain and reference (Gxx)
					bNumBytes = command_set_gain(pCommand);
					break;
				
				case 'Q':	// reboot (Q)
					bNumBytes = command_reboot_a();
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
			UART_A_Write(_gainer.cReplyBuffer, bNumBytes);
		}

		UART_A_CmdReset();					// Reset command buffer
	}
}

BYTE command_set_dout_all(char *pCommand)
{
	BYTE i = 0;
	WORD value = 0;

	if (5 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	value = HEX_TO_BYTE(*(pCommand + 1));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 2));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 3));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 4));

	// set all output ports
	for (i = 0; i < _gainer.bChannels_DOUT; i++) {
		set_dout(i, (value & (1 << i)));
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

BYTE command_set_dout_h(char *pCommand)
{
	BYTE channel = HEX_TO_BYTE(*(pCommand + 1));

	if (2 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	if (_gainer.bChannels_DOUT < 1) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	if (channel > (_gainer.bChannels_DOUT - 1)) {
		// specified channel number seems to be invalid
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	set_dout(channel, TRUE);

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'H';
	_gainer.cReplyBuffer[1] = *(pCommand + 1);
	_gainer.cReplyBuffer[2] = '*';

	return 3;
}

BYTE command_set_dout_l(char *pCommand)
{
	BYTE channel = HEX_TO_BYTE(*(pCommand + 1));

	if (2 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	if (_gainer.bChannels_DOUT < 1) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	if (channel > (_gainer.bChannels_DOUT - 1)) {
		// specified channel number seems to be invalid
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	set_dout(channel, FALSE);

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'L';
	_gainer.cReplyBuffer[1] = *(pCommand + 1);
	_gainer.cReplyBuffer[2] = '*';

	return 3;
}

BYTE command_set_aout_all(char *pCommand)
{
	BYTE i = 0;
	BYTE value = 0;
	BYTE commandLength = 0;

	if (_gainer.bChannels_AOUT < 1) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	switch (_gainer.bChannels_AOUT) {
		case 4:
			commandLength = 9;
			break;

		case 8:
			commandLength = 17;
			break;

		default:
			break;
	}

	// should change 0 or 8 channel condition
	if (commandLength != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	for (i = 0; i < _gainer.bChannels_AOUT; i++) {
		pCommand++;
		value = HEX_TO_BYTE(*pCommand);
		pCommand++;
		value = (value << 4) + HEX_TO_BYTE(*pCommand);
		set_aout(i, value);
	}

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'A';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_aout_ch(char *pCommand)
{
	BYTE channel = 0;
	BYTE value = 0;

	if (4 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	channel = HEX_TO_BYTE(*(pCommand + 1));

	if (channel > (_gainer.bChannels_AOUT - 1)) {
		// specified channel number seems to be invalid
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	value = HEX_TO_BYTE(*(pCommand + 2));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 3));

	set_aout(channel, value);

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'a';
	_gainer.cReplyBuffer[1] = *(pCommand + 1);
	_gainer.cReplyBuffer[2] = *(pCommand + 2);
	_gainer.cReplyBuffer[3] = *(pCommand + 3);
	_gainer.cReplyBuffer[4] = '*';

	return 5;
}

BYTE command_get_din_all(char *pCommand, BOOL bContinuous)
{
	BYTE value = 0x00;

	if (1 != UART_A_bCmdLength()) {
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

	switch (_gainer.bChannels_DIN) {
		case 4:
			value += A_GET_DIN_1() ? 0x01 : 0x00;
			value += A_GET_DIN_2() ? 0x02 : 0x00;
			value += A_GET_DIN_3() ? 0x04 : 0x00;
			value += A_GET_DIN_4() ? 0x08 : 0x00;
			break;

		case 8:
			value += A_GET_DIN_5() ? 0x01 : 0x00;
			value += A_GET_DIN_6() ? 0x02 : 0x00;
			value += A_GET_DIN_7() ? 0x04 : 0x00;
			value += A_GET_DIN_8() ? 0x08 : 0x00;	
			value += A_GET_DIN_1() ? 0x10 : 0x00;
			value += A_GET_DIN_2() ? 0x20 : 0x00;
			value += A_GET_DIN_3() ? 0x40 : 0x00;
			value += A_GET_DIN_4() ? 0x80 : 0x00;
		break;

		default:
			break;
	}

	_gainer.cReplyBuffer[0] = *pCommand;
	ByteToHex(0x00, &_gainer.cReplyBuffer[1]);	// 'x','x'
	ByteToHex(value, &_gainer.cReplyBuffer[3]);	// 'x','x'
	_gainer.cReplyBuffer[5] = '*';

	return 6;
}

BYTE command_get_ain_all(char *pCommand, BOOL bContinuous)
{
	BYTE i = 0;

	if (1 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	_gainer.bContinuousAinRequested = bContinuous;
	_gainer.bContinuousAinMask = _gainer.bContinuousAinRequested ? 0xFF : 0x00;

	_gainer.cReplyBuffer[0] = *pCommand;

	for (i = 0; i < _gainer.bChannels_AIN; i++) {
		ByteToHex(_a.bAnalogInput[i], &_gainer.cReplyBuffer[(i * 2) + 1]);
	}
	_gainer.cReplyBuffer[(_gainer.bChannels_AIN * 2) + 1] = '*';

	return ((_gainer.bChannels_AIN * 2) + 2);	// 10 or 18 bytes
}

BYTE command_get_ain_ch(char *pCommand, BOOL bContinuous)
{
	BYTE channel = 0;
	BYTE value = 0;

	if (2 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	_gainer.bContinuousAinRequested = bContinuous;

	channel = HEX_TO_BYTE(*(pCommand + 1));

	if (channel > (_gainer.bChannels_AIN - 1)) {
		// specified channel number seems to be invalid
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	if (_gainer.bContinuousAinRequested) {
		_gainer.bContinuousAinMask = 1 << channel;	// e.g. 0x10 means 4th channel
	} else {
		_gainer.bContinuousAinMask = 0x00;
	}

	_gainer.cReplyBuffer[0] = *pCommand;
	ByteToHex(_a.bAnalogInput[channel], &_gainer.cReplyBuffer[1]);
	_gainer.cReplyBuffer[3] = '*';

	return 4;
}

BYTE command_stop_cont(void)
{
	if (1 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	_gainer.bContinuousAinRequested = FALSE;
	_gainer.bContinuousAinMask = 0x00;
	_gainer.bContinuousDinRequested = FALSE;
	_gainer.cReplyBuffer[0] = 'E';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_led_h(void)
{
	if (1 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	SET_LED_H();

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'h';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_led_l(void)
{
	if (1 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	SET_LED_L();

	if (!_gainer.bVerboseMode) {
		return 0;
	}

	_gainer.cReplyBuffer[0] = 'l';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_gain(char *pCommand)
{
	BYTE gain = HEX_TO_BYTE(*(pCommand + 1));
	BYTE reference = HEX_TO_BYTE(*(pCommand + 2));

	if (3 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	// gain: 0 ~ 15 (x1.00 ~ x48.0)
	if (gain > 15) {
		// specified gain seems to be invalid
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	// reference: 0 ~ 1 (VSS or AGND)
	if (reference > 1) {
		// specified reference seems to be invalid
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	PGA_A_1_SetGain(bGainTable[gain]);
	PGA_A_2_SetGain(bGainTable[gain]);

	// see TRM v.1.22, 13.2.37
	switch (reference) {
		case 0:	// VSS (10b)
			PGA_A_1_GAIN_CR0 = (PGA_A_1_GAIN_CR0 & 0xFC) | 0x02;
			PGA_A_2_GAIN_CR0 = (PGA_A_2_GAIN_CR0 & 0xFC) | 0x02;
			break;

		case 1:	// AGND (01b)
			PGA_A_1_GAIN_CR0 = (PGA_A_1_GAIN_CR0 & 0xFC) | 0x01;
			PGA_A_2_GAIN_CR0 = (PGA_A_2_GAIN_CR0 & 0xFC) | 0x01;
			break;

		default:
			break;
	}

	_gainer.cReplyBuffer[0] = 'G';
	_gainer.cReplyBuffer[1] = *(pCommand + 1);
	_gainer.cReplyBuffer[2] = *(pCommand + 2);
	_gainer.cReplyBuffer[3] = '*';

	return 4;
}

BYTE command_reboot_a(void)
{
	if (1 != UART_A_bCmdLength()) {
		_gainer.cReplyBuffer[0] = '!';
		_gainer.cReplyBuffer[1] = '*';
		return 2;
	}

	_gainer.bQuitRequested = TRUE;

	_gainer.cReplyBuffer[0] = 'Q';
	_gainer.cReplyBuffer[1] = '*';

	return 2;
}

BOOL set_aout(BYTE channel, BYTE value)
{
	switch (channel) {
		case 0:
			PWM8_A_4_WritePulseWidth(value);
			break;

		case 1:
			PWM8_A_3_WritePulseWidth(value);
			break;

		case 2:
			PWM8_A_2_WritePulseWidth(value);
			break;

		case 3:
			PWM8_A_1_WritePulseWidth(value);
			break;

		case 4:
			PWM8_A_8_WritePulseWidth(value);
			break;

		case 5:
			PWM8_A_7_WritePulseWidth(value);
			break;

		case 6:
			PWM8_A_6_WritePulseWidth(value);
			break;

		case 7:
			PWM8_A_5_WritePulseWidth(value);
			break;

		default:
			break;
	}

	return TRUE;
}

BOOL set_dout(BYTE channel, WORD value)
{
	switch (_gainer.bChannels_DOUT) {
		case 4:
			switch (channel) {
				case 0: if (value) A_SET_DOUT_1_H(); else A_SET_DOUT_1_L(); break;
				case 1: if (value) A_SET_DOUT_2_H(); else A_SET_DOUT_2_L(); break;
				case 2: if (value) A_SET_DOUT_3_H(); else A_SET_DOUT_3_L(); break;
				case 3: if (value) A_SET_DOUT_4_H(); else A_SET_DOUT_4_L(); break;
				default: break;
			}
			break;

		case 8:
			switch (channel) {
				case 0: if (value) A_SET_DOUT_5_H(); else A_SET_DOUT_5_L(); break;		
				case 1: if (value) A_SET_DOUT_6_H(); else A_SET_DOUT_6_L(); break;
				case 2: if (value) A_SET_DOUT_7_H(); else A_SET_DOUT_7_L(); break;
				case 3: if (value) A_SET_DOUT_8_H(); else A_SET_DOUT_8_L(); break;
				case 4: if (value) A_SET_DOUT_1_H(); else A_SET_DOUT_1_L(); break;
				case 5: if (value) A_SET_DOUT_2_H(); else A_SET_DOUT_2_L(); break;
				case 6: if (value) A_SET_DOUT_3_H(); else A_SET_DOUT_3_L(); break;
				case 7: if (value) A_SET_DOUT_4_H(); else A_SET_DOUT_4_L(); break;
				default: break;
			}
			break;

		default:
			break;
	}

	return TRUE;
}

void send_ain_values(void)
{
	BYTE length = 0;
	BYTE channel = 0;
	BYTE i = 0;

	if (_gainer.bChannels_AIN < 1) {
		return;
	}

	if (0xFF == _gainer.bContinuousAinMask) {
		_gainer.cReplyBuffer[0] = 'i';

		for (i = 0; i < _gainer.bChannels_AIN; i++) {
			ByteToHex(_a.bAnalogInput[i], &_gainer.cReplyBuffer[(i * 2) + 1]);
		}

		_gainer.cReplyBuffer[(_gainer.bChannels_AIN * 2) + 1] = '*';
		length = (_gainer.bChannels_AIN * 2) + 2;	// 10 or 18 bytes
	} else {
		if (_gainer.bContinuousAinMask & 0x80) {
			channel = 7;
		} else if (_gainer.bContinuousAinMask & 0x40) {
			channel = 6;
		} else if (_gainer.bContinuousAinMask & 0x20) {
			channel = 5;
		} else if (_gainer.bContinuousAinMask & 0x10) {
			channel = 4;
		} else if (_gainer.bContinuousAinMask & 0x04) {
			channel = 3;
		} else if (_gainer.bContinuousAinMask & 0x04) {
			channel = 2;
		} else if (_gainer.bContinuousAinMask & 0x02) {
			channel = 1;
		}

		_gainer.cReplyBuffer[0] = 's';
		ByteToHex(_a.bAnalogInput[channel], &_gainer.cReplyBuffer[1]);
		_gainer.cReplyBuffer[3] = '*';
		length = 4;
	}

	UART_A_Write(_gainer.cReplyBuffer, length);
}

void send_din_values(void)
{
	BYTE length = 0;
	BYTE value = 0;

	if (_gainer.bChannels_DIN < 1) {
		return;
	}

	switch (_gainer.bChannels_DIN) {
		case 4:
			value += A_GET_DIN_1() ? 0x01 : 0x00;
			value += A_GET_DIN_2() ? 0x02 : 0x00;
			value += A_GET_DIN_3() ? 0x04 : 0x00;
			value += A_GET_DIN_4() ? 0x08 : 0x00;
			break;

		case 8:
			value += A_GET_DIN_5() ? 0x01 : 0x00;
			value += A_GET_DIN_6() ? 0x02 : 0x00;
			value += A_GET_DIN_7() ? 0x04 : 0x00;
			value += A_GET_DIN_8() ? 0x08 : 0x00;
			value += A_GET_DIN_1() ? 0x10 : 0x00;
			value += A_GET_DIN_2() ? 0x20 : 0x00;
			value += A_GET_DIN_3() ? 0x40 : 0x00;
			value += A_GET_DIN_4() ? 0x80 : 0x00;
			break;

		default:
			break;
	}

	_gainer.cReplyBuffer[0] = 'r';
	ByteToHex(0x00, &_gainer.cReplyBuffer[1]);	// 'x', 'x'
	ByteToHex(value, &_gainer.cReplyBuffer[3]);	// 'x', 'x'
	_gainer.cReplyBuffer[5] = '*';
	length = 6;

	UART_A_Write(_gainer.cReplyBuffer, length);
}

void init_output_ports(void)
{
	BYTE i = 0;

	// initialize analog outputs
	for (i = 0; i < _gainer.bChannels_AOUT; i++) {
		set_aout(i, 0);
	}

	// initialize digital outputs
	for (i = 0; i < _gainer.bChannels_DOUT; i++) {
		set_dout(i, FALSE);
	}

	// turn off the LED
	SET_LED_L();
}
