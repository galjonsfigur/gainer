#include "gainer_common.h"

//uncomment the following line if the button is pull-up
//#define BUTTON_IS_PULL_UP

extern BOOL bContinuousAinRequested;
extern BOOL bContinuousDinRequested;
extern BYTE bCurrentConfig;
extern BYTE bRequestedConfig;

/**
 * private functions of CONFIG_A
 */
void handle_commands_config_a(void);
BYTE command_set_dout_all(char *pCommand);
BYTE command_set_dout_h(char *pCommand);
BYTE command_set_dout_l(char *pCommand);
BYTE command_get_din_all(char *pCommand, BOOL bContinuous);
BYTE command_set_aout_all(char *pCommand);
BYTE command_set_aout_ch(char *pCommand);
BYTE command_get_ain_all(char *pCommand, BOOL bContinuous);
BYTE command_get_ain_ch(char *pCommand, BOOL bContinuous);
BYTE command_stop_cont(void);
BYTE command_set_led_h(void);
BYTE command_set_led_l(void);
BYTE command_set_gain(char *pCommand);
BYTE command_reboot(void);

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

	bCurrentConfig = CONFIG_A;

	LoadConfig_config_a();

	bContinuousAinRequested = FALSE;
	bContinuousAinMask = 0x0000;
	bContinuousDinRequested = FALSE;

	bAdcChannelNumber = 0;

	bChannels_AIN = 4;
	bChannels_AOUT = 4;
	bChannels_DIN = 4;
	bChannels_DOUT = 4;

	// start UART
//	UART_A_IntCntl(UART_A_ENABLE_RX_INT | UART_A_ENABLE_TX_INT);
	UART_A_IntCntl(UART_A_ENABLE_RX_INT);
	UART_A_Start(UART_A_PARITY_NONE);

	// start analog inputs
	AMUX4_A_1_Start();
	AMUX4_A_2_Start();
//	AMUX4_A_1_InputSelect(bAdcChannelNumber);
	AMUX4_A_1_InputSelect(AMUX4_A_1_PORT0_7);
	AMUX4_A_2_InputSelect(bAdcChannelNumber);
	PGA_A_1_SetGain(bGainTable[0]);
	PGA_A_2_SetGain(bGainTable[0]);
	PGA_A_1_Start(PGA_A_1_LOWPOWER);		// see the module datasheet
	PGA_A_2_Start(PGA_A_2_LOWPOWER);		// might be good lpf!?
	DUALADC_A_Start(DUALADC_A_HIGHPOWER);	// try lower power later
	DUALADC_A_GetSamples(0);	// continuous sampling

	// start analog outputs
	Counter16_A_PWMClk_WritePeriod(155);
	Counter16_A_PWMClk_WriteCompareValue(0);
	Counter16_A_PWMClk_Start();
	PWM8_A_1_WritePeriod(255);
	PWM8_A_2_WritePeriod(255);
	PWM8_A_3_WritePeriod(255);
	PWM8_A_4_WritePeriod(255);
	PWM8_A_1_WritePulseWidth(0);
	PWM8_A_2_WritePulseWidth(0);
	PWM8_A_3_WritePulseWidth(0);
	PWM8_A_4_WritePulseWidth(0);
	PWM8_A_1_Start();
	PWM8_A_2_Start();
	PWM8_A_3_Start();
	PWM8_A_4_Start();

	M8C_EnableGInt;

	// SHOULD HANDLE I2C HERE IN THE NEAR FUTURE!?

#if SERIAL_DEBUG_ENABLED
	UART_A_CPutString("\r\nEnter_Config_A()\r\n");
#endif
}

void Exit_Config_A(void)
{
#if SERIAL_DEBUG_ENABLED
	UART_A_CPutString("\r\nExit_Config_A()\r\n");
#endif

	// SHOULD HANDLE I2C HERE IN THE NEAR FUTURE!?

	M8C_DisableGInt;
	// stop UART
	UART_A_Stop();

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

	UnloadConfig_config_a();
}

void Main_Config_A(void)
{
	WORD i;

	AMUX4_A_1_InputSelect(bAdcChannelNumber);
	
	DUALADC_A_GetSamples(1);	// continuous sampling
	while(DUALADC_A_fIsDataAvailable() == 0);  // Wait for data to be ready 
	bAdcValue[bAdcChannelNumber] = DUALADC_A_iGetData1ClearFlag(); 
		
	bAdcChannelNumber++;
	if (bAdcChannelNumber == 4) {
		bAdcChannelNumber=0;
		bAdcFlags|=0x01;
	}

	// Check if all the 4 channels have been measured
	if (bAdcFlags & 0x01) {
		b_ain[3] = (BYTE)(bAdcValue[0] & 0x00FF);
		b_ain[2] = (BYTE)(bAdcValue[1] & 0x00FF);
		b_ain[1] = (BYTE)(bAdcValue[2] & 0x00FF);
		b_ain[0] = (BYTE)(bAdcValue[3] & 0x00FF);
		bAdcFlags &= ~0x01;
	}

#ifdef BUTTON_IS_PULL_UP
	PRT1DR |= 0x20;	// let's pull-up!
#endif

	bButtonIsOn = GET_BUTTON();

	if (bContinuousAinRequested) {
		send_ain_values();
	}

	if (bContinuousDinRequested) {
		send_din_values();	
	}

	if (bButtonIsOn != bButtonWasOn) {
#ifdef BUTTON_IS_PULL_UP
		cReplyBuffer[0] = bButtonIsOn ? 'F' : 'N';
#else
		cReplyBuffer[0] = bButtonIsOn ? 'N' : 'F';
#endif

		cReplyBuffer[1] = '*';
		UART_A_Write(cReplyBuffer, 2);
	}

	bButtonWasOn = bButtonIsOn;

	handle_commands_config_a();
}

void handle_commands_config_a(void)
{
	char * pCommand;						// Parameter pointer
	BYTE bNumBytes = 0;

	if (UART_A_bCmdCheck()) {				// Wait for command    
		if(pCommand = UART_A_szGetParam()) {
			switch (*pCommand) {
				case 'C':
					bNumBytes = HandleConfigCommand(pCommand);
					break;

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
					bNumBytes = command_reboot();
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
	BYTE value = 0;
	value = HEX_TO_BYTE(*(pCommand + 1));
	value = (value << 4) + HEX_TO_BYTE(*(pCommand + 2));

	set_dout(3, (value & 0x08));
	set_dout(2, (value & 0x04));
	set_dout(1, (value & 0x02));
	set_dout(0, (value & 0x01));

	cReplyBuffer[0] = 'D';
	cReplyBuffer[1] = *(pCommand + 1);
	cReplyBuffer[2] = *(pCommand + 2);
	cReplyBuffer[3] = '*';

	return 4;
}

BYTE command_set_dout_h(char *pCommand)
{
	BYTE channel = HEX_TO_BYTE(*(pCommand + 1));

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

BYTE command_get_din_all(char *pCommand, BOOL bContinuous)
{
	BYTE value = 0x00;

	bContinuousDinRequested = bContinuous;

	value += GET_DIN_1() ? 0x01 : 0x00;
	value += GET_DIN_2() ? 0x02 : 0x00;
	value += GET_DIN_3() ? 0x04 : 0x00;
	value += GET_DIN_4() ? 0x08 : 0x00;

	cReplyBuffer[0] = *pCommand;
	ByteToHex(value, &cReplyBuffer[1]);	// 'x','x'
	cReplyBuffer[3] = '*';

	return 4;
}

BYTE command_set_aout_all(char *pCommand)
{
	BYTE i = 0;
	BYTE value = 0;

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

BYTE command_get_ain_all(char *pCommand, BOOL bContinuous)
{
	bContinuousAinRequested = bContinuous;
	bContinuousAinMask = bContinuousAinRequested ? 0x000F : 0x0000;

	cReplyBuffer[0] = *pCommand;
	ByteToHex(b_ain[0], &cReplyBuffer[1]);
	ByteToHex(b_ain[1], &cReplyBuffer[3]);
	ByteToHex(b_ain[2], &cReplyBuffer[5]);
	ByteToHex(b_ain[3], &cReplyBuffer[7]);
	cReplyBuffer[9] = '*';

	return 10;
}

BYTE command_get_ain_ch(char *pCommand, BOOL bContinuous)
{
	BYTE channel = 0;
	BYTE value = 0;

	bContinuousAinRequested = bContinuous;

	channel = HEX_TO_BYTE(*(pCommand + 1));

	if (channel > (bChannels_AIN - 1)) {
		// specified channel number seems to be invalid
		cReplyBuffer[0] = '!';
		cReplyBuffer[1] = '*';
		return 2;
	}

	if (bContinuousAinRequested) {
		bContinuousAinMask = 1 << channel;	// e.g. 0x0010 means 4th channel
	} else {
		bContinuousAinMask = 0x0000;
	}

	cReplyBuffer[0] = *pCommand;
	ByteToHex(b_ain[channel], &cReplyBuffer[1]);
	cReplyBuffer[3] = '*';

	return 4;
}

BYTE command_stop_cont(void)
{
	bContinuousAinRequested = FALSE;
	bContinuousAinMask = 0x0000;
	bContinuousDinRequested = FALSE;
	cReplyBuffer[0] = 'E';
	cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_led_h(void)
{
	SET_LED_H();

	cReplyBuffer[0] = 'h';
	cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_led_l(void)
{
	SET_LED_L();

	cReplyBuffer[0] = 'l';
	cReplyBuffer[1] = '*';

	return 2;
}

BYTE command_set_gain(char *pCommand)
{
	BYTE gain = HEX_TO_BYTE(*(pCommand + 1));
	BYTE reference = HEX_TO_BYTE(*(pCommand + 2));

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

BYTE command_reboot(void)
{
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

		default:
			break;
	}

	return TRUE;
}

BOOL set_dout(BYTE channel, BOOL value)
{
	switch (channel) {
		case 0:
			if (value) SET_DOUT_1_H(); else SET_DOUT_1_L();
			break;

		case 1:
			if (value) SET_DOUT_2_H(); else SET_DOUT_2_L();
			break;

		case 2:
			if (value) SET_DOUT_3_H(); else SET_DOUT_3_L();
			break;

		case 3:
			if (value) SET_DOUT_4_H(); else SET_DOUT_4_L();
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

	if (0x000F == bContinuousAinMask) {
		cReplyBuffer[0] = 'i';
		ByteToHex(b_ain[0], &cReplyBuffer[1]);
		ByteToHex(b_ain[1], &cReplyBuffer[3]);
		ByteToHex(b_ain[2], &cReplyBuffer[5]);
		ByteToHex(b_ain[3], &cReplyBuffer[7]);
		cReplyBuffer[9] = '*';
		length = 10;
	} else {
		if (bContinuousAinMask & 0x08) {
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

//	PRT0DR |= 0x55;	// let's pull-up all DIN ports

	value += GET_DIN_1() ? 0x01 : 0x00;
	value += GET_DIN_2() ? 0x02 : 0x00;
	value += GET_DIN_3() ? 0x04 : 0x00;
	value += GET_DIN_4() ? 0x08 : 0x00;

	cReplyBuffer[0] = 'r';
	ByteToHex(value, &cReplyBuffer[1]);	// 'x', 'x'
	cReplyBuffer[3] = '*';
	length = 4;

	UART_A_Write(cReplyBuffer, length);
}
