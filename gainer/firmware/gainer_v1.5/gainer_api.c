#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "gainer_api.h"

/*
 * PSoC port drive mode definitions
 */
enum {
	PULL_DOWN = 0,
	STRONG,
	HIGH_Z,
	PULL_UP,
	OPEN_DRAIN_HIGH,
	HIGH_Z_ANALOG,
	OPEN_DRAIN_LOW
};

static BYTE gPortMode[18] = {
	NOT_INITIALIZED,	// PORT 0
	NOT_INITIALIZED,	// PORT 1
	NOT_INITIALIZED,	// PORT 2
	NOT_INITIALIZED,	// PORT 3
	NOT_INITIALIZED,	// PORT 4
	NOT_INITIALIZED,	// PORT 5
	NOT_INITIALIZED,	// PORT 6
	NOT_INITIALIZED,	// PORT 7
	NOT_INITIALIZED,	// PORT 8
	NOT_INITIALIZED,	// PORT 9
	NOT_INITIALIZED,	// PORT 10
	NOT_INITIALIZED,	// PORT 11
	NOT_INITIALIZED,	// PORT 12
	NOT_INITIALIZED,	// PORT 13
	NOT_INITIALIZED,	// PORT 14
	NOT_INITIALIZED,	// PORT 15
	NOT_INITIALIZED,	// PORT 16 (Button)
	NOT_INITIALIZED		// PORT 17 (LED)
};

static BYTE gPortValue[18] = {
	0, 0, 0, 0, 0, 0, 0, 0,	// port 0-7
	0, 0, 0, 0, 0, 0, 0, 0,	// port 8-15
	0, 0					// port 16 (Button), port 17 (LED)
};

static BYTE gPort0Shadow = 0x00;
static BYTE gPort1Shadow = 0x00;
static BYTE gPort2Shadow = 0x00;

#include "gainer_ports.h"

static BOOL gVerboseMode = FALSE;
static char cReplyBuffer[32];
static BYTE bGetAinChannelMask = 0x00;
static char cLocalRxBuffer[128];
static BYTE bCommandLength = 0;

const BYTE pinForPort[18] = {
	7,	// 00: P0[7]
	5,	// 01: P0[5]
	3,	// 02: P0[3]
	1,	// 03: P0[1]
	6,	// 04: P0[6]
	4,	// 05: P0[4]
	2,	// 06: P0[2]
	0,	// 07: P0[0]
	7,	// 08: P2[7]
	5,	// 09: P2[5]
	3,	// 10: P2[3]
	1,	// 11: P2[1]
	0,	// 12: P2[0]
	6,	// 13: P1[6]
	4,	// 14: P1[4]
	2,	// 15: P1[2]
	5,	// 16: P1[5]
	7	// 17: P1[7]
};

void set_drive_mode_registers(BYTE port, BYTE v2, BYTE v1, BYTE v0)
{
	BYTE mask = 1 << pinForPort[port];

	if ((0 <= port) && (port <= 7)) {
		PRT0DM2 = (0 == v2) ? (PRT0DM2 & ~mask) : ((PRT0DM2 & ~mask) | mask);
		PRT0DM1 = (0 == v1) ? (PRT0DM1 & ~mask) : ((PRT0DM1 & ~mask) | mask);
		PRT0DM0 = (0 == v0) ? (PRT0DM0 & ~mask) : ((PRT0DM0 & ~mask) | mask);
	} else if ((0 <= port) && (port <= 7)) {
		PRT2DM2 = (0 == v2) ? (PRT2DM2 & ~mask) : ((PRT2DM2 & ~mask) | mask);
		PRT2DM1 = (0 == v1) ? (PRT2DM1 & ~mask) : ((PRT2DM1 & ~mask) | mask);
		PRT2DM0 = (0 == v0) ? (PRT2DM0 & ~mask) : ((PRT2DM0 & ~mask) | mask);
	} else if ((0 <= port) && (port <= 7)) {
		PRT1DM2 = (0 == v2) ? (PRT1DM2 & ~mask) : ((PRT1DM2 & ~mask) | mask);
		PRT1DM1 = (0 == v1) ? (PRT1DM1 & ~mask) : ((PRT1DM1 & ~mask) | mask);
		PRT1DM0 = (0 == v0) ? (PRT1DM0 & ~mask) : ((PRT1DM0 & ~mask) | mask);
	}
}

/*
 * Set port drive mode
 * ref: PSoC TRM, Version 2.10 pp.101
 * DM2 DM1 DM0
 * -----------------------------
 * 0   0   0   pull down
 * 0   0   1   strong
 * 0   1   0   high-Z
 * 0   1   1   pull up
 * 1   0   0   open drain high
 * 1   0   1   slow strong drive
 * 1   1   0   high-Z analog
 * 1   1   1   open drain low
 */
BOOL set_port_drive_mode(BYTE port, BYTE mode)
{
	switch(mode) {
		case PULL_DOWN:
			set_drive_mode_registers(port, 0, 0, 0);
			break;

		case STRONG:
			set_drive_mode_registers(port, 0, 0, 1);
			break;

		case HIGH_Z:
			set_drive_mode_registers(port, 0, 1, 0);
			break;

		case PULL_UP:
			set_drive_mode_registers(port, 0, 1, 1);
			break;

		case OPEN_DRAIN_HIGH:
			set_drive_mode_registers(port, 1, 0, 0);
			break;

		case HIGH_Z_ANALOG:
			set_drive_mode_registers(port, 1, 1, 0);
			break;

		case OPEN_DRAIN_LOW:
			set_drive_mode_registers(port, 1, 1, 1);
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}

BOOL is_valid_port_number(BYTE port, BYTE mode)
{
	switch (mode) {
		case AIN_NORMAL:
		case AIN_LPF:
		case AIN_HPF:
		case AIN_PEAK_HOLD:
			return (port <= AIN_PORT_MAX);
			break;

		case AIN_CAPSENSE:
			return (port <= AIN_PORT_MAX);
			break;

		case AOUT_PRSPWM:
			return ((AOUT_PORT_MIN <= port) && (port <= AOUT_PORT_MAX));
			break;

		case AOUT_SERVO:
			return ((AOUT_PORT_MIN <= port) && (port <= AOUT_PORT_MAX));
			break;

		default:
			break;
	}

	return FALSE;
}

void configure_port_din(BYTE port, BYTE mode)
{
	switch (mode) {
		case DIN_HIGH_Z:
			// configure the pin mode for the port to 'High Z'
			set_port_drive_mode(port, HIGH_Z);
			break;

		case DIN_PULL_UP:
			// configure the pin mode for the port to 'Pull Up'
			set_port_drive_mode(port, PULL_UP);
			break;

		case DIN_PULL_DOWN:
			// configure the pin mode for the port to 'Pull Down'
			set_port_drive_mode(port, PULL_DOWN);
			break;

		default:
			break;
	}
}

void configure_port_dout(BYTE port, BYTE mode)
{
	switch (mode) {
		case DOUT_STRONG:
			// configure the pin mode for the port to 'Strong'
			set_port_drive_mode(port, STRONG);
			break;

		case DOUT_OPEN_DRAIN_HIGH:
			// configure the pin mode for the port to 'Open Drain High'
			set_port_drive_mode(port, OPEN_DRAIN_HIGH);
			break;

		case DOUT_OPEN_DRAIN_LOW:
			// configure the pin mode for the port to 'Open Drain Low'
			set_port_drive_mode(port, OPEN_DRAIN_LOW);
			break;

		default:
			break;
	}
}

void configure_port_adc(BYTE port, BYTE mode)
{
	// load the configuration if needed
	if (!Isain_adcLoaded()) {
		LoadConfig_ain_adc();
	}

	// configure the pin mode for the port to 'High Z Analog'
	set_port_drive_mode(port, HIGH_Z_ANALOG);

	// enable port scan for the port

	// enable specified port function
	switch (mode) {
		case AIN_NORMAL:
			break;

		case AIN_LPF:
			// enable LPF for the port
			break;

		case AIN_HPF:
			// enable HPF for the port
			break;

		case AIN_PEAK_HOLD:
			// enable peak hold for the port
			break;

		default:
			break;
	}
}

void configure_port_capsense(BYTE number, BYTE mode)
{
	// load the configuration if needed
	if (!Isain_capsenseLoaded()) {
		LoadConfig_ain_capsense();
	}

	// configure the pin mode for the port to 'High Z Analog'
	set_port_drive_mode(number, HIGH_Z_ANALOG);

	// enable port scan for the port

}

void configure_port_prspwm(BYTE number, BYTE mode)
{
	// load the configuration if needed
	if (number < 12) {
		if (!Isaout_prspwm_aLoaded()) {
			LoadConfig_aout_prspwm_a();
		}
	} else {
		if (!Isaout_prspwm_bLoaded()) {
			LoadConfig_aout_prspwm_b();
		}
	}

	// configure the pin mode for the port to 'Strong'
	set_port_drive_mode(number, STRONG);
}

void configure_port_servo(BYTE number, BYTE mode)
{
	// load the configuration if needed
	if (number < 12) {
		if (!Isaout_servo_aLoaded()) {
			LoadConfig_aout_servo_a();
		}
	} else {
		if (!Isaout_servo_bLoaded()) {
			LoadConfig_aout_servo_b();
		}
	}

	// configure the pin mode for the port to 'Strong'
	set_port_drive_mode(number, STRONG);
}

void SyncWait(void)
{

}

void ScanInputs(void)
{

}

void ProcessCommands(void)
{
/*
	char * pCommand;		// Parameter pointer
	BYTE bNumBytes = 0;

	// reset Rx buffer if it seems to be broken
	if (UART_bErrCheck()) {
		UART_CmdReset();
		return;
	}

	if (UART_bCmdCheck()) {				// Wait for command    
		if (pCommand = UART_szGetParam()) {
			// copy the command to the local Rx buffer and reset the command buffer ASAP
			_gainer.bCommandLength = UART_bCmdLength();
			memcpy(_gainer.cLocalRxBuffer, pCommand, _gainer.bCommandLength);
			UART_CmdReset();

			// ignore a command if specified command length is zero
			if (0 == _gainer.bCommandLength) {
				return;			
			}

			switch (*_gainer.cLocalRxBuffer) {
				case 'D':	// set all digital outputs (Dxx)
					bNumBytes = command_set_dout_all(_gainer.cLocalRxBuffer);
					break;
				
				case 'H':	// set the digital output high (Hn)
					bNumBytes = command_set_dout_h(_gainer.cLocalRxBuffer);
					break;
				
				case 'L':	// set the digital output low (Ln)
					bNumBytes = command_set_dout_l(_gainer.cLocalRxBuffer);
					break;
				
				case 'R':	// get all digital inputs (R)
					bNumBytes = command_get_din_all(_gainer.cLocalRxBuffer, FALSE);
					break;
				
				case 'r':	// get all digital inputs (continuous) (r)
					bNumBytes = command_get_din_all(_gainer.cLocalRxBuffer, TRUE);
					break;
				
				case 'A':	// set all analog outputs (Axx...xx)
					bNumBytes = command_set_aout_all(_gainer.cLocalRxBuffer);
					break;
				
				case 'a':	// set the analog output (anxx)
					bNumBytes = command_set_aout_ch(_gainer.cLocalRxBuffer);
					break;
				
				case 'I':	// get all analog inputs (I)
					bNumBytes = command_get_ain_all(_gainer.cLocalRxBuffer, FALSE);
					break;
				
				case 'i':	// get all analog inputs (continuous) (i)
					bNumBytes = command_get_ain_all(_gainer.cLocalRxBuffer, TRUE);
					break;
				
				case 'S':	// get the analog input (Sn)
					bNumBytes = command_get_ain_ch(_gainer.cLocalRxBuffer, FALSE);
					break;
				
				case 's':	// get the analog input (continuous) (sn)
					bNumBytes = command_get_ain_ch(_gainer.cLocalRxBuffer, TRUE);
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
					bNumBytes = command_set_gain(_gainer.cLocalRxBuffer);
					break;
				
				case 'M':	// set sampling mode (Mx)
					bNumBytes = command_set_mode(_gainer.cLocalRxBuffer);
					break;

				case 'Q':	// reboot (Q)
					bNumBytes = command_reboot_a();
					break;
				
				case 'n':	// notify when changed (experimental)
					bNumBytes = command_get_din_notify(_gainer.cLocalRxBuffer);
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
	}
*/
}

BOOL DigitalWrite(BYTE port, BYTE value)
{
	switch (port) {
		case 0:
			digital_write_00(value);
			break;
		case 1:
			digital_write_01(value);
			break;
		case 2:
			digital_write_02(value);
			break;
		case 3:
			digital_write_03(value);
			break;
		case 4:
			digital_write_04(value);
			break;
		case 5:
			digital_write_05(value);
			break;
		case 6:
			digital_write_06(value);
			break;
		case 7:
			digital_write_07(value);
			break;
		case 8:
			digital_write_08(value);
			break;
		case 9:
			digital_write_09(value);
			break;
		case 10:
			digital_write_10(value);
			break;
		case 11:
			digital_write_11(value);
			break;
		case 12:
			digital_write_12(value);
			break;
		case 13:
			digital_write_13(value);
			break;
		case 14:
			digital_write_14(value);
			break;
		case 15:
			digital_write_15(value);
			break;
		case 17:
			digital_write_17(value);
			break;
		default:
			break;
	}

	return TRUE;
}

BYTE DigitalRead(BYTE port)
{
	switch (port) {
		case 0:
			return digital_read_00();
			break;
		case 1:
			return digital_read_01();
			break;
		case 2:
			return digital_read_02();
			break;
		case 3:
			return digital_read_03();
			break;
		case 4:
			return digital_read_04();
			break;
		case 5:
			return digital_read_05();
			break;
		case 6:
			return digital_read_06();
			break;
		case 7:
			return digital_read_07();
			break;
		case 8:
			return digital_read_08();
			break;
		case 9:
			return digital_read_09();
			break;
		case 10:
			return digital_read_10();
			break;
		case 11:
			return digital_read_11();
			break;
		case 12:
			return digital_read_12();
			break;
		case 13:
			return digital_read_13();
			break;
		case 14:
			return digital_read_14();
			break;
		case 15:
			return digital_read_15();
			break;
		case 16:
			return digital_read_16();
			break;
		default:
			break;
	}

	return TRUE;
}

BOOL AnalogWrite(BYTE port, BYTE value)
{
	if (gPortMode[port] == AOUT_PRSPWM) {
		switch (port) {
			case 8:
				PRS8_0_WritePolynomial(value);
				break;
			case 9:
				PRS8_1_WritePolynomial(value);
				break;
			case 10:
				PRS8_2_WritePolynomial(value);
				break;
			case 11:
				PRS8_3_WritePolynomial(value);
				break;
			case 12:
				PRS8_4_WritePolynomial(value);
				break;
			case 13:
				PRS8_5_WritePolynomial(value);
				break;
			case 14:
				PRS8_6_WritePolynomial(value);
				break;
			case 15:
				PRS8_7_WritePolynomial(value);
				break;
			default:
				break;
		}
	} else if (gPortMode[port] == AOUT_SERVO) {
		switch (port) {
			case 8:
				Counter8_0_WritePeriod(value);
				break;
			case 9:
				Counter8_1_WritePeriod(value);
				break;
			case 10:
				Counter8_2_WritePeriod(value);
				break;
			case 11:
				Counter8_3_WritePeriod(value);
				break;
			case 12:
				Counter8_4_WritePeriod(value);
				break;
			case 13:
				Counter8_5_WritePeriod(value);
				break;
			case 14:
				Counter8_6_WritePeriod(value);
				break;
			case 15:
				Counter8_7_WritePeriod(value);
				break;
			default:
				break;
		}
	}

	gPortValue[port] = value;

	return TRUE;
}

BYTE AnalogRead(BYTE port)
{
	return gPortValue[port];
}

BOOL SetPortMode(BYTE port, GPortMode mode)
{
	switch (mode) {
		case DIN_HIGH_Z:
		case DIN_PULL_UP:
		case DIN_PULL_DOWN:
			configure_port_din(port, mode);
			gPortMode[port] = mode;
			break;

		case AIN_NORMAL:
		case AIN_LPF:
		case AIN_HPF:
		case AIN_PEAK_HOLD:
			if (!is_valid_port_number(port, mode)) {
				return FALSE;
			}

			if (Isain_capsenseLoaded()) {
				return FALSE;
			}

			configure_port_adc(port, mode);
			gPortMode[port] = mode;
			break;

		case AIN_CAPSENSE:
			if (!is_valid_port_number(port, mode)) {
				return FALSE;
			}

			if (Isain_adcLoaded()) {
				return FALSE;
			}

			configure_port_capsense(port, mode);
			gPortMode[port] = mode;
			break;

		case DOUT_STRONG:
		case DOUT_OPEN_DRAIN_HIGH:
		case DOUT_OPEN_DRAIN_LOW:
			configure_port_dout(port, mode);
			gPortMode[port] = mode;
			break;

		case AOUT_PRSPWM:
			if (!is_valid_port_number(port, mode)) {
				return FALSE;
			}

			if (port < 12) {
				if (Isaout_servo_aLoaded()) {
					return FALSE;
				}
			} else {
				if (Isaout_servo_bLoaded()) {
					return FALSE;
				}
			}

			configure_port_prspwm(port, mode);
			gPortMode[port] = mode;
			break;

		case AOUT_SERVO:
			if (!is_valid_port_number(port, mode)) {
				return FALSE;
			}

			if (port < 12) {
				if (Isaout_prspwm_aLoaded()) {
					return FALSE;
				}
			} else {
				if (Isaout_prspwm_bLoaded()) {
					return FALSE;
				}
			}

			configure_port_servo(port, mode);
			gPortMode[port] = mode;
			break;

		default:
			gPortMode[port] = NOT_INITIALIZED;
			return FALSE;
			break;
	}

	return TRUE;
}

GPortMode GetPortMode(BYTE port)
{
	return gPortMode[port];
}

void Reboot(void)
{

}
