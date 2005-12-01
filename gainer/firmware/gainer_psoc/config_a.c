#include "gainer_common.h"

#define GET_DIN_1() (PRT0DR&0x40)	// P0[6]
#define GET_DIN_2() (PRT0DR&0x10)	// P0[4]
#define GET_DIN_3() (PRT0DR&0x04)	// P0[2]
#define GET_DIN_4() (PRT0DR&0x01)	// P0[0]
#define GET_DIN_5() (PRT0DR&0x80)	// P0[7]
#define GET_DIN_6() (PRT0DR&0x20)	// P0[5]
#define GET_DIN_7() (PRT0DR&0x08)	// P0[3]
#define GET_DIN_8() (PRT0DR&0x02)	// P0[1]

// NOTE: P1[5] is pull-downed, so should be always ZERO!!!
#define SET_DOUT_1_H() (PRT2DR|=0x01)				// P2[0]
#define SET_DOUT_2_H() (PRT1DR=(PRT1DR&0xDF)|0x40)	// P1[6]
#define SET_DOUT_3_H() (PRT1DR=(PRT1DR&0xDF)|0x10)	// P1[4]
#define SET_DOUT_4_H() (PRT1DR=(PRT1DR&0xDF)|0x04)	// P1[2]
#define SET_DOUT_5_H() (PRT2DR|=0x80)				// P2[7]
#define SET_DOUT_6_H() (PRT2DR|=0x20)				// P2[5]
#define SET_DOUT_7_H() (PRT2DR|=0x08)				// P2[3]
#define SET_DOUT_8_H() (PRT2DR|=0x02)				// P2[1]
///

// NOTE: P1[5]  is pull-downed, so should be always ZERO!!!
#define SET_DOUT_1_L() (PRT2DR&=0xFE)	// P2[0]
#define SET_DOUT_2_L() (PRT1DR&=0x9F)	// P1[6]
#define SET_DOUT_3_L() (PRT1DR&=0xCF)	// P1[4]
#define SET_DOUT_4_L() (PRT1DR&=0xDB)	// P1[2]
#define SET_DOUT_5_L() (PRT2DR&=0x7F)	// P2[7]
#define SET_DOUT_6_L() (PRT2DR&=0xDF)	// P2[5]
#define SET_DOUT_7_L() (PRT2DR&=0xF7)	// P2[3]
#define SET_DOUT_8_L() (PRT2DR&=0xFD)	// P2[1]

extern BOOL bContinuousAinRequested;
extern BOOL bContinuousDinRequested;
extern BYTE bCurrentConfig;
extern BYTE bRequestedConfig;

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
BOOL set_dout(BYTE channel, BOOL value);
void send_ain_values(void);
void send_din_values(void);

/**
 * private variables of CONFIG_A
 */
BYTE b_ain[8];
BOOL bButtonWasOn = FALSE;
BOOL bButtonIsOn = FALSE;

void Enter_Config_A(void)
{
	BYTE i = 0;

	bCurrentConfig = bRequestedConfig;

	LoadConfig_config_a();

	bContinuousAinRequested = FALSE;
	bContinuousAinMask = 0x00;
	bContinuousDinRequested = FALSE;

	bAdcChannelNumber = 0;

	switch (bCurrentConfig) {
		case CONFIG_2:
			// consigure number of each port type
			bChannels_AIN = 8;
			bChannels_DIN = 0;
			bChannels_AOUT = 4;
			bChannels_DOUT = 4;

			// change drive mode of *in 1~8 to 'High-Z Analog' (DM[2:0] = '110')
			PRT0DM2 = 0xFF;
			PRT0DM1 = 0xFF;
			PRT0DM0 = 0x00;
			// change select of *in 1~8 to 'Analog'
			// <nothing to do!?>
			break;
/*
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
*/
		case CONFIG_3:
			// consigure number of each port type
			bChannels_AIN = 4;
			bChannels_DIN = 4;
			bChannels_AOUT = 8;
			bChannels_DOUT = 0;

			// connect PWM8 modules to *out 5~8
			// enable global select (enable global bypass)
			PRT1GS |= 0x54;		// connect P1[2], P1[4] and P1[6] to global bus
			PRT2GS |= 0x01;		// connect P2[0] to global bus
			break;
/*
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
*/
		default:
			bChannels_AIN = 4;
			bChannels_DIN = 4;
			bChannels_AOUT = 4;
			bChannels_DOUT = 4;
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
//	AMUX4_A_2_InputSelect(bAdcChannelNumber);
	PGA_A_1_SetGain(bGainTable[0]);
	PGA_A_2_SetGain(bGainTable[0]);
	PGA_A_1_Start(PGA_A_1_LOWPOWER);		// see the module datasheet
	PGA_A_2_Start(PGA_A_2_LOWPOWER);		// might be good lpf!?
	DUALADC_A_Start(DUALADC_A_HIGHPOWER);	// try lower power later
//	DUALADC_A_GetSamples(0);	// continuous sampling

#if 0
	// start analog outputs
	Counter16_A_PWMClk_WritePeriod(155);	// 50Hz (i.e. 20ms)
	Counter16_A_PWMClk_WriteCompareValue(0);
	Counter16_A_PWMClk_Start();

	// set period of all PWM8 modules to 255
	PWM8_A_1_WritePeriod(255);
	PWM8_A_2_WritePeriod(255);
	PWM8_A_3_WritePeriod(255);
	PWM8_A_4_WritePeriod(255);
	PWM8_A_5_WritePeriod(255);
	PWM8_A_6_WritePeriod(255);
	PWM8_A_7_WritePeriod(255);
	PWM8_A_8_WritePeriod(255);
#else
	// start analog outputs
	Counter16_A_PWMClk_WritePeriod(157);	// 50Hz (i.e. 20ms)
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
#endif

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
	switch (bChannels_AOUT) {
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

	M8C_EnableGInt;
}

void Exit_Config_A(void)
{
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
	AMUX4_A_1_InputSelect(bAdcChannelNumber);	// ain 1~4
	AMUX4_A_2_InputSelect(bAdcChannelNumber);	// ain 5~8
	
	DUALADC_A_GetSamples(1);	// continuous sampling
	while(DUALADC_A_fIsDataAvailable() == 0);  // Wait for data to be ready
	wAdcValue[bAdcChannelNumber] = DUALADC_A_iGetData1ClearFlag();		// ain 1~4
	wAdcValue[bAdcChannelNumber + 4] = DUALADC_A_iGetData2ClearFlag();	// ain 5~8

	bAdcChannelNumber++;
	if (bAdcChannelNumber == 4) {
		bAdcChannelNumber = 0;
		bAdcFlags |= 0x01;
	}

	// Check if all the 4 channels have been measured
	if (bAdcFlags & 0x01) {
		// ain 1~4
		b_ain[3] = (BYTE)(wAdcValue[0] & 0x00FF);
		b_ain[2] = (BYTE)(wAdcValue[1] & 0x00FF);
		b_ain[1] = (BYTE)(wAdcValue[2] & 0x00FF);
		b_ain[0] = (BYTE)(wAdcValue[3] & 0x00FF);

		// ain 5~8
		b_ain[7] = (BYTE)(wAdcValue[4] & 0x00FF);
		b_ain[6] = (BYTE)(wAdcValue[5] & 0x00FF);
		b_ain[5] = (BYTE)(wAdcValue[6] & 0x00FF);
		b_ain[4] = (BYTE)(wAdcValue[7] & 0x00FF);

		bAdcFlags &= ~0x01;

		// send ain data when measured all channels
		if (bContinuousAinRequested) {
			send_ain_values();
		}

		// send din data here to avaiod stupid sending
		if (bContinuousDinRequested) {
			send_din_values();
		}
	}
}

void handle_button_event(void)
{
	bButtonIsOn = GET_BUTTON();

	if (bButtonIsOn != bButtonWasOn) {
		cReplyBuffer[0] = bButtonIsOn ? 'N' : 'F';
		cReplyBuffer[1] = '*';
		UART_A_Write(cReplyBuffer, 2);
	}

	bButtonWasOn = bButtonIsOn;
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
					cReplyBuffer[0] = '!';
					cReplyBuffer[1] = '*';
					bNumBytes = 2;
					break;
			}
		}

		if (bNumBytes > 0) {
			UART_A_Write(cReplyBuffer, bNumBytes);
		}

		UART_A_CmdReset();					// Reset command buffer
	}
}

BYTE command_set_dout_all(char *pCommand)
{
	WORD value = 0;

	if (5 != UART_A_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	value = HEX_TO_BYTE(*(pCommand + 1));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 2));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 3));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 4));

	switch (bChannels_DOUT) {
		case 4:
			set_dout(3, (value & 0x0008));
			set_dout(2, (value & 0x0004));
			set_dout(1, (value & 0x0002));
			set_dout(0, (value & 0x0001));
			break;

		case 8:
			set_dout(7, (value & 0x0080));
			set_dout(6, (value & 0x0040));
			set_dout(5, (value & 0x0020));
			set_dout(4, (value & 0x0010));
			set_dout(3, (value & 0x0008));
			set_dout(2, (value & 0x0004));
			set_dout(1, (value & 0x0002));
			set_dout(0, (value & 0x0001));
			break;

		default:
			break;
	}

	cReplyBuffer[0] = 'D';
	cReplyBuffer[1] = *(pCommand + 1);
	cReplyBuffer[2] = *(pCommand + 2);
	cReplyBuffer[3] = *(pCommand + 3);
	cReplyBuffer[4] = *(pCommand + 4);
	cReplyBuffer[5] = '*';

	return 6;
}

BYTE command_set_dout_h(char *pCommand)
{
	BYTE channel = HEX_TO_BYTE(*(pCommand + 1));

	if (2 != UART_A_bCmdLength()) {
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

	set_dout(channel, TRUE);

	cReplyBuffer[0] = 'H';
	cReplyBuffer[1] = *(pCommand + 1);
	cReplyBuffer[2] = '*';

	return 3;
}

BYTE command_set_dout_l(char *pCommand)
{
	BYTE channel = HEX_TO_BYTE(*(pCommand + 1));

	if (2 != UART_A_bCmdLength()) {
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

	set_dout(channel, FALSE);

	cReplyBuffer[0] = 'L';
	cReplyBuffer[1] = *(pCommand + 1);
	cReplyBuffer[2] = '*';

	return 3;
}

BYTE command_set_aout_all(char *pCommand)
{
	BYTE i = 0;
	BYTE value = 0;
	BYTE commandLength = 0;

	if (bChannels_AOUT < 1) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	switch (bChannels_AOUT) {
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
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	for (i = 0; i < bChannels_AOUT; i++) {
		pCommand++;
		value = HEX_TO_BYTE(*pCommand);
		pCommand++;
		value = (value << 4) + HEX_TO_BYTE(*pCommand);
		set_aout(i, value);
	}

	cReplyBuffer[0] = 'A';
	cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_aout_ch(char *pCommand)
{
	BYTE channel = 0;
	BYTE value = 0;

	if (4 != UART_A_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	channel = HEX_TO_BYTE(*(pCommand + 1));

	if (channel > (bChannels_AOUT - 1)) {
		// specified channel number seems to be invalid
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	value = HEX_TO_BYTE(*(pCommand + 2));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 3));

	set_aout(channel, value);

	cReplyBuffer[0] = 'a';
	cReplyBuffer[1] = *(pCommand + 1);
	cReplyBuffer[2] = *(pCommand + 2);
	cReplyBuffer[3] = *(pCommand + 3);
	cReplyBuffer[4] = '*';

	return 5;
}

BYTE command_get_din_all(char *pCommand, BOOL bContinuous)
{
	BYTE value = 0x00;

	if (1 != UART_A_bCmdLength()) {
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

	switch (bChannels_DIN) {
		case 4:
			value += GET_DIN_1() ? 0x01 : 0x00;
			value += GET_DIN_2() ? 0x02 : 0x00;
			value += GET_DIN_3() ? 0x04 : 0x00;
			value += GET_DIN_4() ? 0x08 : 0x00;
			break;

		case 8:
			value += GET_DIN_5() ? 0x01 : 0x00;
			value += GET_DIN_6() ? 0x02 : 0x00;
			value += GET_DIN_7() ? 0x04 : 0x00;
			value += GET_DIN_8() ? 0x08 : 0x00;	
			value += GET_DIN_1() ? 0x10 : 0x00;
			value += GET_DIN_2() ? 0x20 : 0x00;
			value += GET_DIN_3() ? 0x40 : 0x00;
			value += GET_DIN_4() ? 0x80 : 0x00;
		break;

		default:
			break;
	}

	cReplyBuffer[0] = *pCommand;
	ByteToHex(0x00, &cReplyBuffer[1]);	// 'x','x'
	ByteToHex(value, &cReplyBuffer[3]);	// 'x','x'
	cReplyBuffer[5] = '*';

	return 6;
}

BYTE command_get_ain_all(char *pCommand, BOOL bContinuous)
{
	BYTE i = 0;

	if (1 != UART_A_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	bContinuousAinRequested = bContinuous;
	bContinuousAinMask = bContinuousAinRequested ? 0xFF : 0x00;

	cReplyBuffer[0] = *pCommand;

	for (i = 0; i < bChannels_AIN; i++) {
		ByteToHex(b_ain[i], &cReplyBuffer[(i * 2) + 1]);
	}
	cReplyBuffer[(bChannels_AIN * 2) + 1] = '*';

	return ((bChannels_AIN * 2) + 2);	// 10 or 18 bytes
}

BYTE command_get_ain_ch(char *pCommand, BOOL bContinuous)
{
	BYTE channel = 0;
	BYTE value = 0;

	if (2 != UART_A_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	bContinuousAinRequested = bContinuous;

	channel = HEX_TO_BYTE(*(pCommand + 1));

	if (channel > (bChannels_AIN - 1)) {
		// specified channel number seems to be invalid
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	if (bContinuousAinRequested) {
		bContinuousAinMask = 1 << channel;	// e.g. 0x10 means 4th channel
	} else {
		bContinuousAinMask = 0x00;
	}

	cReplyBuffer[0] = *pCommand;
	ByteToHex(b_ain[channel], &cReplyBuffer[1]);
	cReplyBuffer[3] = '*';

	return 4;
}

BYTE command_stop_cont(void)
{
	if (1 != UART_A_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	bContinuousAinRequested = FALSE;
	bContinuousAinMask = 0x00;
	bContinuousDinRequested = FALSE;
	cReplyBuffer[0] = 'E';
	cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_led_h(void)
{
	if (1 != UART_A_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	SET_LED_H();

	cReplyBuffer[0] = 'h';
	cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_led_l(void)
{
	if (1 != UART_A_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	SET_LED_L();

	cReplyBuffer[0] = 'l';
	cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_gain(char *pCommand)
{
	BYTE gain = HEX_TO_BYTE(*(pCommand + 1));
	BYTE reference = HEX_TO_BYTE(*(pCommand + 2));

	if (3 != UART_A_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	// gain: 0 ~ 15 (x1.00 ~ x48.0)
	if (gain > 15) {
		// specified gain seems to be invalid
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	// reference: 0 ~ 1 (VSS or AGND)
	if (reference > 1) {
		// specified reference seems to be invalid
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
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

	cReplyBuffer[0] = 'G';
	cReplyBuffer[1] = *(pCommand + 1);
	cReplyBuffer[2] = *(pCommand + 2);
	cReplyBuffer[3] = '*';

	return 4;
}

BYTE command_reboot_a(void)
{
	if (1 != UART_A_bCmdLength()) {
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	bQuitRequested = TRUE;

	cReplyBuffer[0] = 'Q';
	cReplyBuffer[1] = '*';

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

BOOL set_dout(BYTE channel, BOOL value)
{
	switch (bChannels_DOUT) {
		case 4:
			switch (channel) {
				case 0: if (value) SET_DOUT_1_H(); else SET_DOUT_1_L(); break;
				case 1: if (value) SET_DOUT_2_H(); else SET_DOUT_2_L(); break;
				case 2: if (value) SET_DOUT_3_H(); else SET_DOUT_3_L(); break;
				case 3: if (value) SET_DOUT_4_H(); else SET_DOUT_4_L(); break;
				default: break;
			}
			break;

		case 8:
			switch (channel) {
				case 0: if (value) SET_DOUT_5_H(); else SET_DOUT_5_L(); break;		
				case 1: if (value) SET_DOUT_6_H(); else SET_DOUT_6_L(); break;
				case 2: if (value) SET_DOUT_7_H(); else SET_DOUT_7_L(); break;
				case 3: if (value) SET_DOUT_8_H(); else SET_DOUT_8_L(); break;
				case 4: if (value) SET_DOUT_1_H(); else SET_DOUT_1_L(); break;
				case 5: if (value) SET_DOUT_2_H(); else SET_DOUT_2_L(); break;
				case 6: if (value) SET_DOUT_3_H(); else SET_DOUT_3_L(); break;
				case 7: if (value) SET_DOUT_4_H(); else SET_DOUT_4_L(); break;
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

	if (bChannels_AIN < 1) {
		return;
	}

	if (0xFF == bContinuousAinMask) {
		cReplyBuffer[0] = 'i';

		for (i = 0; i < bChannels_AIN; i++) {
			ByteToHex(b_ain[i], &cReplyBuffer[(i * 2) + 1]);
		}

		cReplyBuffer[(bChannels_AIN * 2) + 1] = '*';
		length = (bChannels_AIN * 2) + 2;	// 10 or 18 bytes
	} else {
		if (bContinuousAinMask & 0x80) {
			channel = 7;
		} else if (bContinuousAinMask & 0x40) {
			channel = 6;
		} else if (bContinuousAinMask & 0x20) {
			channel = 5;
		} else if (bContinuousAinMask & 0x10) {
			channel = 4;
		} else if (bContinuousAinMask & 0x04) {
			channel = 3;
		} else if (bContinuousAinMask & 0x04) {
			channel = 2;
		} else if (bContinuousAinMask & 0x02) {
			channel = 1;
		}

		cReplyBuffer[0] = 's';
		ByteToHex(b_ain[channel], &cReplyBuffer[1]);
		cReplyBuffer[3] = '*';
		length = 4;
	}

	UART_A_Write(cReplyBuffer, length);
}

void send_din_values(void)
{
	BYTE length = 0;
	BYTE value = 0;

	if (bChannels_DIN < 1) {
		return;
	}

	switch (bChannels_DIN) {
		case 4:
			value += GET_DIN_1() ? 0x01 : 0x00;
			value += GET_DIN_2() ? 0x02 : 0x00;
			value += GET_DIN_3() ? 0x04 : 0x00;
			value += GET_DIN_4() ? 0x08 : 0x00;
			break;

		case 8:
			value += GET_DIN_5() ? 0x01 : 0x00;
			value += GET_DIN_6() ? 0x02 : 0x00;
			value += GET_DIN_7() ? 0x04 : 0x00;
			value += GET_DIN_8() ? 0x08 : 0x00;
			value += GET_DIN_1() ? 0x10 : 0x00;
			value += GET_DIN_2() ? 0x20 : 0x00;
			value += GET_DIN_3() ? 0x40 : 0x00;
			value += GET_DIN_4() ? 0x80 : 0x00;
			break;

		default:
			break;
	}

	cReplyBuffer[0] = 'r';
	ByteToHex(0x00, &cReplyBuffer[1]);	// 'x', 'x'
	ByteToHex(value, &cReplyBuffer[3]);	// 'x', 'x'
	cReplyBuffer[5] = '*';
	length = 6;

	UART_A_Write(cReplyBuffer, length);
}
