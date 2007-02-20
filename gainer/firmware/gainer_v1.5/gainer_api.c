#include <m8c.h>        // part specific constants and macros
#include <string.h>        // for memcpy
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "psocdynamic.h"
#include "gainer_api.h"

#pragma interrupt_handler PWM16_1_ISR
void PWM16_1_ISR(void);

const char cVersionString[] = {'1','.','5','.','0','b','0','1'};

#define MIN(A,B)	(((A)<(B))?(A):(B))
#define MAX(A,B)	(((A)>(B))?(A):(B))
#define ABS(X)		(((X)>0x80)?((X)-0x80):(0x80-(X)))	// NOTE: valid for BYTE only
#define HEX_TO_BYTE(X) (((X)>='0'&&(X)<='9')?((X)-'0'):((X)-'A'+10))

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

/*
 * The array to store port mode.
 * @see 
 */
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

/*
 * The array to store values of each port.
 */
static BYTE gPortValue[18] = {
	0, 0, 0, 0, 0, 0, 0, 0,	// port 0-7
	0, 0, 0, 0, 0, 0, 0, 0,	// port 8-15
	0, 0					// port 16 (Button), port 17 (LED)
};

static BYTE gLastPortValue[18] = {
	0, 0, 0, 0, 0, 0, 0, 0,	// port 0-7
	0, 0, 0, 0, 0, 0, 0, 0,	// port 8-15
	0, 0					// port 16 (Button), port 17 (LED)
};

/*
 * Shadow registers for ports from 0 to 2.
 */
static BYTE gPort0Shadow = 0x00;
static BYTE gPort1Shadow = 0x00;
static BYTE gPort2Shadow = 0x00;

#include "gainer_ports.h"

/*
 * The array to convert from a port number to the pin number
 */
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

const BYTE bGainTable[16] = {
	PGA_0_G1_00,	// G0x
	PGA_0_G1_14,	// G1x
	PGA_0_G1_33,	// G2x
	PGA_0_G1_46,	// G3x
	PGA_0_G1_60,	// G4x
	PGA_0_G1_78,	// G5x
	PGA_0_G2_00,	// G6x
	PGA_0_G2_27,	// G7x
	PGA_0_G2_67,	// G8x
	PGA_0_G3_20,	// G9x
	PGA_0_G4_00,	// GAx
	PGA_0_G5_33,	// GBx
	PGA_0_G8_00,	// GCx
	PGA_0_G16_0,	// GDx
	PGA_0_G24_0,	// GEx
	PGA_0_G48_0,	// GFx
};

/*
 * Port scan pattern list
 * ----: 0, 0, 0, 0, 0 ...
 * ---0: 0, 0, 0, 0, 0 ...
 * --1-: 1, 1, 1, 1, 1 ...
 * --10: 0, 1, 0, 1, 0 ...
 * -2--: 2, 2, 2, 2, 2 ...
 * -2-0: 0, 2, 0, 2, 0 ...
 * -21-: 1, 2, 1, 2, 1 ...
 * -210: 0, 1, 0, 2, 0 ...
 * 3---: 3, 3, 3, 3, 3 ...
 * 3--0: 0, 3, 0, 3, 0 ...
 * 3-1-: 1, 3, 1, 3, 1 ...
 * 3-10: 0, 1, 0, 3, 0 ...
 * 32--: 2, 3, 2, 3, 2 ...
 * 32-0: 0, 2, 0, 3, 0 ...
 * 321-: 1, 2, 1, 3, 1 ...
 * 3210: 0, 1, 2, 3, 0 ...
 */
const BYTE gPortScanPatternList[16][4] = {
	{ 0, 0, 0, 0 },	// DUMMY
	{ 3, 3, 3, 3 },	// port 0, 0, 0, 0
	{ 2, 2, 2, 2 },	// port 1, 1, 1, 1
	{ 3, 2, 3, 2 },	// port 0, 1, 0, 1
	{ 1, 1, 1, 1 },	// port 2, 2, 2, 2
	{ 3, 1, 3, 1 },	// port 0, 2, 0, 2
	{ 2, 1, 2, 1 },	// port 1, 2, 1, 2
	{ 3, 2, 3, 1 },	// port 0, 1, 0, 2
	{ 0, 0, 0, 0 },	// port 3, 3, 3, 3
	{ 3, 0, 3, 0 },	// port 0, 3, 0, 3
	{ 2, 0, 2, 0 },	// port 1, 3, 1, 3
	{ 3, 2, 3, 0 },	// port 0, 1, 0, 3
	{ 1, 0, 1, 0 },	// port 2, 3, 2, 3
	{ 3, 1, 3, 0 },	// port 0, 2, 0, 3
	{ 2, 1, 2, 0 },	// port 1, 2, 1, 3
	{ 3, 2, 1, 0 }	// port 0, 1, 2, 3
};

enum {
	PGA_REFERENCE_DGND,	// was PGA_REFERENCE_VSS
	PGA_REFERENCE_AGND
};


/*
 * Additional global variables.
 */
static BOOL gVerboseMode = FALSE;
static char gReplyBuffer[32];
static BYTE gPortScanMask = 0x00;
static BYTE gPortScanPattern[2];
static BOOL gPortScanEnabled[2];
static BYTE gAnalogInputMin[AIN_PORT_MAX + 1];
static BYTE gAnalogInputMax[AIN_PORT_MAX + 1];
static BYTE gPgaReference = PGA_REFERENCE_DGND;
static WORD gReportMask = 0x0000;
static BYTE gReportMode = 0;
static BOOL gReadyToReport = FALSE;


/*
 * Interrupt handlers.
 */
void PWM16_1_ISR(void)
{
//	Counter8_4_Stop();
	Counter8_4_WriteCompareValue(gPortValue[12]);
//	Counter8_4_COUNTER_REG = Counter8_4_PERIOD;
//	Counter8_4_Start();

//	Counter8_5_Stop();
	Counter8_5_WriteCompareValue(gPortValue[13]);
//	Counter8_5_COUNTER_REG = Counter8_5_PERIOD;
//	Counter8_5_Start();

//	Counter8_6_Stop();
	Counter8_6_WriteCompareValue(gPortValue[14]);
//	Counter8_6_COUNTER_REG = Counter8_6_PERIOD;
//	Counter8_6_Start();

//	Counter8_7_Stop();
	Counter8_7_WriteCompareValue(gPortValue[15]);
//	Counter8_7_COUNTER_REG = Counter8_7_PERIOD;
//	Counter8_7_Start();
}


/*
 * Prototypes for local functions (i.e. not user functions)
 */
BYTE hex_to_byte(char *str);
WORD hex_to_word(char *str);
void byte_to_hex(BYTE value, char *str);
BYTE count_enabled_bits(WORD bitMask);

void init_variables(void);
void init_ports(void);
void set_drive_mode_registers(BYTE port, BYTE v2, BYTE v1, BYTE v0);
BOOL set_port_drive_mode(BYTE port, BYTE mode);
BOOL is_valid_port_number(BYTE port, BYTE mode);
void configure_port_din(BYTE port, BYTE mode);
void configure_port_dout(BYTE port, BYTE mode);
void configure_port_capsense(BYTE port, BYTE mode);
void configure_port_prspwm(BYTE port, BYTE mode);
void configure_port_servo(BYTE port, BYTE mode);
void connect_port_to_bus(BYTE port);
void disconnect_port_from_bus(BYTE port);
BOOL is_checksum_ok(char *p, BYTE length);
BYTE handle_port_mode_command(char *p, BYTE length);
BYTE handle_write_command(char *p, BYTE length);
BYTE handle_read_command(char *p, BYTE length);
BYTE handle_quit_command(char *p, BYTE length);
BYTE handle_configuration_command(char *p, BYTE length);
BYTE handle_version_command(char *p, BYTE length);
BYTE put_error_string_to_reply_buffer(BYTE errorCode);
void update_port_scan_info(void);


BYTE hex_to_byte(char *str)
{
	BYTE h = HEX_TO_BYTE(*(str));
	BYTE l = HEX_TO_BYTE(*(str + 1));

	return ((h << 4) + l);
}


WORD hex_to_word(char *str)
{
	BYTE h = hex_to_byte(str);
	BYTE l = hex_to_byte(str + 2);

	return ((h << 8) + l);
}


const char cHexString[16] = "0123456789ABCDEF";
void byte_to_hex(BYTE value, char *str)
{
	BYTE v = value;

	v >>= 4;
	*str = cHexString[v & 0x0F];
	str++;
	v = value;
	*str = cHexString[v & 0x0F];
}


BYTE count_enabled_bits(WORD bitMask)
{
	BYTE enabledBits = 0;
	BYTE i = 0;

	for (i = 0; i < 16; i++) {
		if (bitMask & ((WORD)1 << (WORD)i)) {
			enabledBits++;
		}
	}

	return enabledBits;
}


void init_variables(void)
{
	gVerboseMode = FALSE;
	gPortScanMask = 0x00;
	gReportMask = 0x0000;
	gReportMode = REPORT_NONE;
	gReadyToReport = FALSE;

	update_port_scan_info();
}


void init_ports(void)
{
	BYTE i = 0;

	for (i = 0; i < NUM_OF_PORTS; i++) {
		SetPortMode(i, DIN_HIGH_Z);
	}
}


void set_drive_mode_registers(BYTE port, BYTE v2, BYTE v1, BYTE v0)
{
	BYTE mask = 1 << pinForPort[port];

	if ((0 <= port) && (port <= 7)) {
		PRT0DM2 = (0 == v2) ? (PRT0DM2 & ~mask) : ((PRT0DM2 & ~mask) | mask);
		PRT0DM1 = (0 == v1) ? (PRT0DM1 & ~mask) : ((PRT0DM1 & ~mask) | mask);
		PRT0DM0 = (0 == v0) ? (PRT0DM0 & ~mask) : ((PRT0DM0 & ~mask) | mask);
	} else if ((8 <= port) && (port <= 12)) {
		PRT2DM2 = (0 == v2) ? (PRT2DM2 & ~mask) : ((PRT2DM2 & ~mask) | mask);
		PRT2DM1 = (0 == v1) ? (PRT2DM1 & ~mask) : ((PRT2DM1 & ~mask) | mask);
		PRT2DM0 = (0 == v0) ? (PRT2DM0 & ~mask) : ((PRT2DM0 & ~mask) | mask);
	} else if ((13 <= port) && (port <= 17)) {
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
		case AIN_ADC:
		case AIN_ADC_LPF:
		case AIN_ADC_HPF:
		case AIN_ADC_PEAK_HOLD:
			return (port <= AIN_PORT_MAX);
			break;

		case AIN_CAPSENSE:
			return (port <= AIN_PORT_MAX);
			break;

		case AOUT_PSEUDO_ANALOG:
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


void connect_port_to_bus(BYTE port)
{
	switch (port) {
		case 8:
			// connect P2[1] to the global bus
			PRT2GS |= PIN_1;
			break;

		case 9:
			// connect P2[3] to the global bus
			PRT2GS |= PIN_3;
			break;

		case 10:
			// connect P2[5] to the global bus
			PRT2GS |= PIN_5;
			break;

		case 11:
			// connect P2[7] to the global bus
			PRT2GS |= PIN_7;
			break;

		case 12:
			// connect P1[2] to the global bus
			PRT1GS |= PIN_2;
			break;

		case 13:
			// connect P1[4] to the global bus
			PRT1GS |= PIN_4;
			break;

		case 14:
			// connect P1[6] to the global bus
			PRT1GS |= PIN_6;
			break;

		case 15:
			// connect P2[0] to the global bus
			PRT2GS |= PIN_0;
			break;

		default:
			// NO ACTION
			break;
	}	
}


void disconnect_port_from_bus(BYTE port)
{
	switch (port) {
		case 8:
			// disconnect P2[1] from the global bus
			PRT2GS &= ~PIN_1;
			break;

		case 9:
			// disconnect P2[3] from the global bus
			PRT2GS &= ~PIN_3;
			break;

		case 10:
			// disconnect P2[5] from the global bus
			PRT2GS &= ~PIN_5;
			break;

		case 11:
			// disconnect P2[7] from the global bus
			PRT2GS &= ~PIN_7;
			break;

		case 12:
			// disconnect P1[2] from the global bus
			PRT1GS &= ~PIN_2;
			break;

		case 13:
			// disconnect P1[4] from the global bus
			PRT1GS &= ~PIN_4;
			break;

		case 14:
			// disconnect P1[6] from the global bus
			PRT1GS &= ~PIN_6;
			break;

		case 15:
			// disconnect P2[0] from the global bus
			PRT2GS &= ~PIN_0;
			break;

		default:
			// NO ACTION
			break;
	}
}


void configure_port_din(BYTE port, BYTE mode)
{
	// disconnect a module from an output if needed
	if ((gPortMode[port] == AOUT_PSEUDO_ANALOG) || (gPortMode[port] == AOUT_SERVO)) {
		disconnect_port_from_bus(port);
	}

	switch (mode) {
		case DIN_HIGH_Z:
			// configure the pin mode for the port to 'High Z'
			set_port_drive_mode(port, HIGH_Z);
			DigitalWrite(port, 0);
			break;

		case DIN_PULL_UP:
			// configure the pin mode for the port to 'Pull Up'
			set_port_drive_mode(port, PULL_UP);
			DigitalWrite(port, 1);
			break;

		case DIN_PULL_DOWN:
			// configure the pin mode for the port to 'Pull Down'
			set_port_drive_mode(port, PULL_DOWN);
			DigitalWrite(port, 0);
			break;

		default:
			break;
	}

	switch (gPortMode[port]) {
		case AIN_ADC:
		case AIN_ADC_LPF:
		case AIN_ADC_HPF:
		case AIN_ADC_PEAK_HOLD:
		case AIN_CAPSENSE:
			gPortScanMask &= ~(0x01 << port);
			update_port_scan_info();
			break;

		default:
			break;
	}

	gPortMode[port] = mode;
}


void configure_port_dout(BYTE port, BYTE mode)
{
	// disconnect a module from an output if needed
	if ((gPortMode[port] == AOUT_PSEUDO_ANALOG) || (gPortMode[port] == AOUT_SERVO)) {
		disconnect_port_from_bus(port);
	}

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

	switch (gPortMode[port]) {
		case AIN_ADC:
		case AIN_ADC_LPF:
		case AIN_ADC_HPF:
		case AIN_ADC_PEAK_HOLD:
		case AIN_CAPSENSE:
			gPortScanMask &= ~(0x01 << port); 
			update_port_scan_info();
			break;

		default:
			break;
	}

	gPortMode[port] = mode;
}


void configure_port_adc(BYTE port, BYTE mode)
{
	// load the configuration if needed
	if (!Isain_adcLoaded()) {
		LoadConfig_ain_adc();

		// start modules
		AMUX4_0_Start();
		AMUX4_1_Start();
		PGA_0_SetGain(bGainTable[0]);
		PGA_1_SetGain(bGainTable[0]);
		PGA_0_Start(PGA_0_LOWPOWER);		// see the module datasheet
		PGA_1_Start(PGA_1_LOWPOWER);		// might be good lpf!?
		DUALADC_Start(DUALADC_HIGHPOWER);	// try lower power later
	}

	// configure the pin mode for the port to 'High Z Analog'
	set_port_drive_mode(port, HIGH_Z_ANALOG);

	// enable port scan for the port
	gPortScanMask |= (0x01 << port);
	update_port_scan_info();

	// enable specified port function
	switch (mode) {
		case AIN_ADC:
			break;

		case AIN_ADC_LPF:
			// enable LPF for the port
			break;

		case AIN_ADC_HPF:
			// enable HPF for the port
			break;

		case AIN_ADC_PEAK_HOLD:
			// enable peak hold for the port
			break;

		default:
			break;
	}

	gPortMode[port] = mode;
}


void configure_port_capsense(BYTE port, BYTE mode)
{
	// load the configuration if needed
	if (!Isain_capsenseLoaded()) {
		LoadConfig_ain_capsense();

		AMUX4_D_Start();
//		AMUX4_D_InputSelect(_d.bInputNumber);

		CMPPRG_D_Start(CMPPRG_D_HIGHPOWER);
		Timer8_D_Start();
		Timer16_D_Start();
		Timer16_D_EnableInt();
	}

	// configure the pin mode for the port to 'High Z Analog'
	set_port_drive_mode(port, HIGH_Z_ANALOG);

	// enable port scan for the port
	gPortScanMask |= (0x01 << port);
	update_port_scan_info();

	gPortMode[port] = mode;
}


#define bPOLY_255 0xB8	// Modular Polynomial = [8,6,5,4]
#define	bSEED_255 0xFF	// initial seed value
#define	bSEED_000 0x00	// initial seed value

void configure_port_prspwm(BYTE port, BYTE mode)
{
	// load the configuration if needed
	if (port < 12) {
		if (!Isaout_prspwm_aLoaded()) {
			LoadConfig_aout_prspwm_a();

			// configure modules
			PRS8_0_WritePolynomial(bPOLY_255);
			PRS8_1_WritePolynomial(bPOLY_255);
			PRS8_2_WritePolynomial(bPOLY_255);
			PRS8_3_WritePolynomial(bPOLY_255);

			// write initial seed values
			PRS8_0_WriteSeed(bSEED_000);
			PRS8_1_WriteSeed(bSEED_000);
			PRS8_2_WriteSeed(bSEED_000);
			PRS8_3_WriteSeed(bSEED_000);

			// start PRS8 modules
			PRS8_0_Start();
			PRS8_1_Start();
			PRS8_2_Start();
			PRS8_3_Start();
		}
	} else {
		if (!Isaout_prspwm_bLoaded()) {
			LoadConfig_aout_prspwm_b();

			// configure modules
			PRS8_4_WritePolynomial(bPOLY_255);
			PRS8_5_WritePolynomial(bPOLY_255);
			PRS8_6_WritePolynomial(bPOLY_255);
			PRS8_7_WritePolynomial(bPOLY_255);

			// write initial seed values
			PRS8_4_WriteSeed(bSEED_000);
			PRS8_5_WriteSeed(bSEED_000);
			PRS8_6_WriteSeed(bSEED_000);
			PRS8_7_WriteSeed(bSEED_000);

			// start PRS8 modules
			PRS8_4_Start();
			PRS8_5_Start();
			PRS8_6_Start();
			PRS8_7_Start();
		}
	}

	// connect a PRSPWM8 module to an output if needed
	if ((gPortMode[port] != AOUT_PSEUDO_ANALOG) && (gPortMode[port] != AOUT_SERVO)) {
		connect_port_to_bus(port);
	}

	// configure the pin mode for the port to 'Strong'
	set_port_drive_mode(port, STRONG);

	gPortMode[port] = mode;
}


void configure_port_servo(BYTE port, BYTE mode)
{
	// load the configuration if needed
	if (port < 12) {
		if (!Isaout_servo_aLoaded()) {
			LoadConfig_aout_servo_a();

			// configure modules
			Counter8_0_WritePeriod(255);
			Counter8_1_WritePeriod(255);
			Counter8_2_WritePeriod(255);
			Counter8_3_WritePeriod(255);

			Counter8_0_WriteCompareValue(0);
			Counter8_1_WriteCompareValue(0);
			Counter8_2_WriteCompareValue(0);
			Counter8_3_WriteCompareValue(0);

			// start Counter8 modules
			Counter8_0_Start();
			Counter8_1_Start();
			Counter8_2_Start();
			Counter8_3_Start();
		}
	} else {
		if (!Isaout_servo_bLoaded()) {
			LoadConfig_aout_servo_b();

			// configure modules
			Counter8_4_WritePeriod(255);
			Counter8_5_WritePeriod(255);
			Counter8_6_WritePeriod(255);
			Counter8_7_WritePeriod(255);

			Counter8_4_WriteCompareValue(0);
			Counter8_5_WriteCompareValue(0);
			Counter8_6_WriteCompareValue(0);
			Counter8_7_WriteCompareValue(0);

			// start Counter8 modules
			Counter8_4_Start();
			Counter8_5_Start();
			Counter8_6_Start();
			Counter8_7_Start();
		}
	}

	// connect a Counter8 module to an output if needed
	if ((gPortMode[port] != AOUT_PSEUDO_ANALOG) && (gPortMode[port] != AOUT_SERVO)) {
		connect_port_to_bus(port);
	}

	// configure the pin mode for the port to 'Strong'
//	set_port_drive_mode(port, STRONG);

	gPortMode[port] = mode;
}


BOOL is_checksum_ok(char *p, BYTE length)
{
	BYTE checksum = hex_to_byte(p + length - 2);

	// THIS IS TEMPORAL IMPLEMENTATION
	return (checksum == 0x00);
}


// {P}+{00..FF: port number}+{00..FF: port mode}+{00..FF: checksum}
// e.g. P0301ss (set port mode of the port 03 to mode 01)
#define COMMAND_LENGTH_PORT_MODE 7

BYTE handle_port_mode_command(char *p, BYTE length)
{
	BYTE i = 0;
	BYTE port = 0;
	BYTE mode = 0;
	BYTE result = 0;

	if (length != COMMAND_LENGTH_PORT_MODE) {
		return put_error_string_to_reply_buffer(SYNTAX_ERROR);
	}

	// calc checksum, then compare
	if (!is_checksum_ok(p, length)) {
		return put_error_string_to_reply_buffer(CHECKSUM_ERROR);
	}

	port = hex_to_byte(p + 1);
	mode = hex_to_byte(p + 3);
	result = SetPortMode(port, mode);

	if (result == FALSE) {
		return put_error_string_to_reply_buffer(PORT_MODE_ERROR);
	}

	// echo the command
	for (i = 0; i < length; i++) {
		gReplyBuffer[i] = *(p + i);
	}
	gReplyBuffer[i] = '*';

	return (COMMAND_LENGTH_PORT_MODE + 1);
}


// {W}+{0000..FFFF: port bit mask}+{00..FF: value 0}...{00..FF: value n}+{00..FF: checksum}
// e.g. W000CF83Css* (set port 02 and 03 to0xF8 and 0x3C)
// e.g. W000Css* (set port 02 and 03 to high)

#define COMMAND_LENGTH_WRITE_BITMASK 7

BYTE handle_write_command(char *p, BYTE length)
{
	BYTE i = 0;
	BYTE readPorts = 0;
	BYTE numberOfPorts = 0;
	BYTE value = 0;
	WORD bitMask = 0x0000;
	
	// calc checksum, then compare
	if (!is_checksum_ok(p, length)) {
		return put_error_string_to_reply_buffer(CHECKSUM_ERROR);
	}

	// 01234....
	// W000C....
	//  ^^^^
	bitMask = hex_to_word(p + 1);

	if (length == COMMAND_LENGTH_WRITE_BITMASK) {
		for (i = 0; i < NUM_OF_PORTS; i++) {
			if (bitMask & ((WORD)1 << (WORD)i)) {
				DigitalWrite(i, 1);
			} else {
				DigitalWrite(i, 0);
			}
		}
	} else {
		numberOfPorts = count_enabled_bits(bitMask);
		if (length != (5 + (numberOfPorts * 2) + 2)) {
			return put_error_string_to_reply_buffer(SYNTAX_ERROR);
		}

		// 0123456..
		// W000CF8..
		//      ^^
		for (i = 0; i < NUM_OF_PORTS; i++) {
			if (bitMask & ((WORD)1 << (WORD)i)) {
				value = hex_to_byte(p + 5 + (readPorts * 2));
				AnalogWrite(i, value);
				readPorts++;
			}
		}
	}

	// echo the command
	for (i = 0; i < length; i++) {
		gReplyBuffer[i] = *(p + i);
	}
	gReplyBuffer[i] = '*';

	return (length + 1);
}


// {R}+{0..3: stop, once, always, when changed}+{0000..FFFF: port mask}+{00..FF: checksum}
// e.g. R3000Fss (notify when changed: port 0, 1, 2 and 3)
// e.g. R0ss (stop reading)
#define COMMAND_LENGTH_READ 8
#define COMMAND_LENGTH_READ_STOP 4

BYTE handle_read_command(char *p, BYTE length)
{
	BYTE i = 0;
	BYTE mode = 0;

	// calc checksum, then compare
	if (!is_checksum_ok(p, length)) {
		return put_error_string_to_reply_buffer(CHECKSUM_ERROR);
	}

	// 01...
	// R3...
	//  ^
	mode = HEX_TO_BYTE(*(p + 1));

	switch (mode) {
		case REPORT_NONE:
			// {R}+{00}+{ss}+{*}
			if (length != COMMAND_LENGTH_READ_STOP) {
				return put_error_string_to_reply_buffer(SYNTAX_ERROR);
			} else {
				// STOP READING
				gReportMode = REPORT_NONE;
				gReportMask = 0x0000;

				// echo the command
				for (i = 0; i < length; i++) {
					gReplyBuffer[i] = *(p + i);
				}
				gReplyBuffer[i] = '*';
				return (length + 1);
			}
			break;

		case REPORT_ONCE:
		case REPORT_ALWAYS:
		case REPORT_WHEN_CHANGED:
			if (length != COMMAND_LENGTH_READ) {
				return put_error_string_to_reply_buffer(SYNTAX_ERROR);
			} else {
				gReportMode = mode;
				gReportMask = hex_to_word(p + 2);
				for (i = 0; i < NUM_OF_PORTS; i++) {
					if ((gReportMask & ((WORD)1 << (WORD)i)) == 0) {
						continue;
					}

					switch (gPortMode[i]) {
						case DIN_HIGH_Z:
						case DIN_PULL_UP:
						case DIN_PULL_DOWN:
							gPortValue[i] = DigitalRead(i);
							break;

						default:
							break;
					}
					gLastPortValue[i] = gPortValue[i];
				}
				return 0;
			}
			break;

		default:
			return put_error_string_to_reply_buffer(SYNTAX_ERROR);
			break;
	}
}


// {Q}+{U}+{I}+{T}+{00..FF: checksum}
// e.g. QUITss
#define COMMAND_LENGTH_QUIT 6

BYTE handle_quit_command(char *p, BYTE length)
{
	BYTE i = 0;

	if (length != COMMAND_LENGTH_QUIT) {
		return put_error_string_to_reply_buffer(SYNTAX_ERROR);
	}

	// calc checksum, then compare
	if (!is_checksum_ok(p, length)) {
		return put_error_string_to_reply_buffer(CHECKSUM_ERROR);
	}

	Reboot();

	// echo the command
	for (i = 0; i < length; i++) {
		gReplyBuffer[i] = *(p + i);
	}
	gReplyBuffer[i] = '*';

	return (length + 1);
}


// {K}+{00..FF: parameter id}+{00..FF: value}+{checksum}
// e.g. K0105ss (set value of the parameter 0x01 to 0x05)
#define COMMAND_LENGTH_CONFIGURATION 7

BYTE handle_configuration_command(char *p, BYTE length)
{
	BYTE i = 0;
	BYTE id = 0;
	BYTE value = 0;

	// calc checksum, then compare
	if (!is_checksum_ok(p, length)) {
		return put_error_string_to_reply_buffer(CHECKSUM_ERROR);
	}

	if (!Isain_adcLoaded()) {
		return put_error_string_to_reply_buffer(CHECKSUM_ERROR);
	}

	id = hex_to_byte(p + 1);
	value = hex_to_byte(p + 3);

	switch (id) {
		case PGA_GAIN:
			if (value > 15) {
				// specified gain seems to be invalid
				return put_error_string_to_reply_buffer(SYNTAX_ERROR);
			}
			PGA_0_SetGain(bGainTable[value]);
			PGA_1_SetGain(bGainTable[value]);
			break;

		case PGA_REFERENCE:
			if (value > 1) {
				// specified reference seems to be invalid
				return put_error_string_to_reply_buffer(SYNTAX_ERROR);
			}

			// see TRM v.1.22, 13.2.37
			if (0 == value) {	// VSS (10b)
				PGA_0_GAIN_CR0 = (PGA_0_GAIN_CR0 & 0xFC) | 0x02;
				PGA_1_GAIN_CR0 = (PGA_1_GAIN_CR0 & 0xFC) | 0x02;
				gPgaReference = PGA_REFERENCE_DGND;
			} else if (1 == value) {	// AGND (01b)
				PGA_0_GAIN_CR0 = (PGA_0_GAIN_CR0 & 0xFC) | 0x01;
				PGA_1_GAIN_CR0 = (PGA_1_GAIN_CR0 & 0xFC) | 0x01;
				gPgaReference = PGA_REFERENCE_AGND;
			}
			break;

		default:
			return put_error_string_to_reply_buffer(SYNTAX_ERROR);
			break;
	}

	// echo the command
	for (i = 0; i < length; i++) {
		gReplyBuffer[i] = *(p + i);
	}
	gReplyBuffer[i] = '*';

	return (length + 1);
}


// {'?'}+{'*'}
#define COMMAND_LENGTH_VERSION	1

BYTE handle_version_command(char *p, BYTE length)
{
	BYTE i = 0;

	if (length != COMMAND_LENGTH_VERSION) {
		return put_error_string_to_reply_buffer(SYNTAX_ERROR);
	}
	
	// e.g. "?1.0.0.10*"
	gReplyBuffer[0] = '?';

	for (i = 0; i < sizeof(cVersionString); i++) {
		gReplyBuffer[1 + i] = cVersionString[i];
	}

	gReplyBuffer[1 + i] = '*';

	return (2 + sizeof(cVersionString));
}


BYTE put_error_string_to_reply_buffer(BYTE errorCode)
{
	gReplyBuffer[0] = '!';
	gReplyBuffer[1] = '0' + errorCode;
	gReplyBuffer[2] = '*';

	return 3;
}


void update_port_scan_info(void)
{
	gPortScanPattern[0] = gPortScanMask & 0x0F;
	gPortScanEnabled[0] = (gPortScanPattern[0] != 0);

	gPortScanPattern[1] = (gPortScanMask & 0xF0) >> 4;
	gPortScanEnabled[1] = (gPortScanPattern[1] != 0);
}


void SyncWait(void)
{
	// wait for about 2ms
	SleepTimer_SyncWait(1, SleepTimer_WAIT_RELOAD);
}


#define ADC_SCAN_FINISHED	0x01

void ScanInputs(void)
{
	static BYTE bWaitingForADC = FALSE;
	static BYTE bCounter = 0;
	static WORD wAdcValue[4][2];

	BYTE i = 0;
	BYTE j = 0;	
	BYTE port = 0;
	BYTE value = 0;
	BYTE a = 0;
	BYTE b = 0;

	if (!bWaitingForADC) {	
		AMUX4_0_InputSelect(gPortScanPatternList[gPortScanPattern[0]][bCounter]);
		AMUX4_1_InputSelect(gPortScanPatternList[gPortScanPattern[1]][bCounter]);
		DUALADC_GetSamples(1);	// get one sample
		bWaitingForADC = TRUE;	// set the flag to let CPU do other tasks
								// DO NOT KEEP WAITING HERE!!
		return;
	} else {
		// it seems that we are waiting for a data is available
		if (DUALADC_fIsDataAvailable() == 0) {
			// it seems that a data is not available
			return;
		} else {
			// it seems that a data is available
			// clear the flag and let's process the data
			bWaitingForADC = FALSE;
		}
	}

	wAdcValue[bCounter][0] = DUALADC_iGetData1();			// port 0 ~ 3
	wAdcValue[bCounter][1] = DUALADC_iGetData2ClearFlag();	// port 4 ~ 7

	bCounter++;
	if (bCounter < 4) {
		return;
	}

	for (j = 0; j < 2; j++) {
		if (!gPortScanEnabled[j]) {
			continue;
		}

		for (i = 0; i < 4; i++) {
			port = 3 - gPortScanPatternList[gPortScanPattern[j]][i] + (j * 4);
			value = wAdcValue[i][j] & 0x00FF;

			if (gPortMode[port] == AIN_ADC) {
				gPortValue[port] = value;
			} else if (gPortMode[port] == AIN_ADC_LPF) {
				// LPF processing
				// NOT IMPLEMENTED
				gPortValue[port] = value;
			} else if (gPortMode[port] == AIN_ADC_HPF) {
				// HPF processing
				// NOT IMPLEMENTED
				gPortValue[port] = value;
			} else if (gPortMode[port] == AIN_ADC_PEAK_HOLD) {
				gAnalogInputMin[port] = MIN(gAnalogInputMin[port], value);
				gAnalogInputMax[port] = MAX(gAnalogInputMax[port], value);

				if (PGA_REFERENCE_DGND == gPgaReference) {
					gPortValue[port] = gAnalogInputMax[port];
				} else {	// PGA_REFERENCE_AGND
					a = gAnalogInputMin[port];
					b = gAnalogInputMax[port];
					gPortValue[port] = (ABS(a) > ABS(b)) ? a : b;
				}
			}
		}
	}

	bCounter = 0;
	gReadyToReport = TRUE;
}


void ProcessCommands(void)
{
	static char cLocalRxBuffer[128];
	static BYTE bCommandLength = 0;

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
			bCommandLength = UART_bCmdLength();
			memcpy(cLocalRxBuffer, pCommand, bCommandLength);
			UART_CmdReset();

			// ignore a command if specified command length is zero
			if (0 == bCommandLength) {
				return;			
			}

			switch (*cLocalRxBuffer) {
				case 'P':
					bNumBytes = handle_port_mode_command(cLocalRxBuffer, bCommandLength);
					break;

				case 'W':
					bNumBytes = handle_write_command(cLocalRxBuffer, bCommandLength);
					break;

				case 'R':
					bNumBytes = handle_read_command(cLocalRxBuffer, bCommandLength);
					break;

				case 'Q':
					bNumBytes = handle_quit_command(cLocalRxBuffer, bCommandLength);
					break;

				case 'K':
					bNumBytes = handle_configuration_command(cLocalRxBuffer, bCommandLength);
					break;

				case '?':
					bNumBytes = handle_version_command(cLocalRxBuffer, bCommandLength);
					break;

				case 'H':
					DigitalWrite(PORT_LED, 0x01);
					gReplyBuffer[0] = 'H';
					gReplyBuffer[1] = '*';
					bNumBytes = 2;
					break;

				case 'L':
					DigitalWrite(PORT_LED, 0x00);
					gReplyBuffer[0] = 'L';
					gReplyBuffer[1] = '*';
					bNumBytes = 2;
					break;

				default:
					// seems to be an invalid command
					bNumBytes = put_error_string_to_reply_buffer(SYNTAX_ERROR);
					break;
			}
		}

		if (bNumBytes > 0) {
			UART_Write(gReplyBuffer, bNumBytes);
		}
	}
}


void ReportToHost(void)
{
	BYTE i = 0;
	BOOL hasChanged = FALSE;

	// Special case for the button
	gPortValue[PORT_BUTTON] = DigitalRead(PORT_BUTTON);
	if (gLastPortValue[PORT_BUTTON] != gPortValue[PORT_BUTTON]) {
		if (gPortValue[PORT_BUTTON]) {
			UART_PutChar('h');
		} else {
			UART_PutChar('l');
		}
		UART_PutChar('*');

		gLastPortValue[PORT_BUTTON] = gPortValue[PORT_BUTTON];
	}

	for (i = 0; i < NUM_OF_PORTS; i++) {
		if (gReportMask & ((WORD)1 << (WORD)i)) {
			switch (gPortMode[i]) {
				case DIN_HIGH_Z:
				case DIN_PULL_UP:
				case DIN_PULL_DOWN:
					gPortValue[i] = DigitalRead(i);
					break;

				default:
					break;
			}

			if (gLastPortValue[i] != gPortValue[i]) {
				hasChanged = TRUE;
				break;
			}
		}
	}

	// Return immediately if there is no need to report
	if (gReportMode == REPORT_NONE) {
		return;
	} else if ((gReportMode == REPORT_WHEN_CHANGED) && (!hasChanged)) {
		return;
	}

	if (!gReadyToReport) {
		return;
	}

	UART_PutChar('R');
	UART_PutSHexInt(gReportMask);

	for (i = 0; i < NUM_OF_PORTS; i++) {
		if (gReportMask & ((WORD)1 << (WORD)i)) {
			UART_PutSHexByte(gPortValue[i]);
			gLastPortValue[i] = gPortValue[i];
		}
	}

	UART_PutChar('*');

	if (gReportMode == REPORT_ONCE) {
		gReportMode = REPORT_NONE;
	}

	for (i = 0; i < 8; i++) {
		gAnalogInputMax[i] = 0x00;
		gAnalogInputMin[i] = 0xFF;
	}
}


BOOL DigitalWrite(BYTE port, BYTE value)
{
	// JUST FOR DEBUGGING
#if 0
	UART_PutChar('w');
	UART_PutSHexByte(port);
	UART_PutChar(',');
	UART_PutSHexByte(value);
	UART_PutChar(' ');
#endif

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

	PRT0DR = gPort0Shadow;
	PRT1DR = gPort1Shadow;
	PRT2DR = gPort2Shadow;

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
	if (gPortMode[port] == AOUT_PSEUDO_ANALOG) {
		switch (port) {
			case 8:
				PRS8_0_WriteSeed(value);
				break;
			case 9:
				PRS8_1_WriteSeed(value);
				break;
			case 10:
				PRS8_2_WriteSeed(value);
				break;
			case 11:
				PRS8_3_WriteSeed(value);
				break;
			case 12:
				PRS8_4_WriteSeed(value);
				break;
			case 13:
				PRS8_5_WriteSeed(value);
				break;
			case 14:
				PRS8_6_WriteSeed(value);
				break;
			case 15:
				PRS8_7_WriteSeed(value);
				break;
			default:
				break;
		}
	} else if (gPortMode[port] == AOUT_SERVO) {
		switch (port) {
			case 8:
				Counter8_0_WriteCompareValue(value);
				break;
			case 9:
				Counter8_1_WriteCompareValue(value);
				break;
			case 10:
				Counter8_2_WriteCompareValue(value);
				break;
			case 11:
				Counter8_3_WriteCompareValue(value);
				break;
#if 0
			case 12:
				Counter8_4_Stop();
				Counter8_4_WriteCompareValue(value);
				Counter8_4_COUNTER_REG = Counter8_4_PERIOD;
				Counter8_4_Start();
				break;
			case 13:
				Counter8_5_Stop();
				Counter8_5_WriteCompareValue(value);
				Counter8_5_COUNTER_REG = Counter8_5_PERIOD;
				Counter8_5_Start();
				break;
			case 14:
				Counter8_6_Stop();
				Counter8_6_WriteCompareValue(value);
				Counter8_6_COUNTER_REG = Counter8_6_PERIOD;
				Counter8_6_Start();
				break;
			case 15:
				Counter8_7_Stop();
				Counter8_7_WriteCompareValue(value);
				Counter8_7_COUNTER_REG = Counter8_7_PERIOD;
				Counter8_7_Start();
				break;
#endif
			default:
				break;
		}
	} else {
		DigitalWrite(port, value);
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
			break;

		case AIN_ADC:
		case AIN_ADC_LPF:
		case AIN_ADC_HPF:
		case AIN_ADC_PEAK_HOLD:
			if (!is_valid_port_number(port, mode)) {
				return FALSE;
			}

			if (Isain_capsenseLoaded()) {
				return FALSE;
			}

			configure_port_adc(port, mode);
			break;

		case AIN_CAPSENSE:
			if (!is_valid_port_number(port, mode)) {
				return FALSE;
			}

			if (Isain_adcLoaded()) {
				return FALSE;
			}

			configure_port_capsense(port, mode);
			break;

		case DOUT_STRONG:
		case DOUT_OPEN_DRAIN_HIGH:
		case DOUT_OPEN_DRAIN_LOW:
			configure_port_dout(port, mode);
			break;

		case AOUT_PSEUDO_ANALOG:
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
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}


GPortMode GetPortMode(BYTE port)
{
	return gPortMode[port];
}


void Initialize(void)
{
	if (!IscommonLoaded()) {
		LoadConfig_common();
	}

	M8C_EnableGInt;

	UART_IntCntl(UART_ENABLE_RX_INT);
	UART_Start(UART_PARITY_NONE);

	SleepTimer_Start();
	SleepTimer_SetInterval(SleepTimer_512_HZ);
	SleepTimer_EnableInt();

#if 0
	// Should be called after starting Counter8 modules!?
	PWM16_1_WritePeriod(2550);
	PWM16_1_WritePulseWidth(2294);
	PWM16_1_Start();
#endif

	init_ports();
}


void Reboot(void)
{
	init_variables();

	// wait for 8/512 = 15.625ms
	SleepTimer_SyncWait(8, SleepTimer_WAIT_RELOAD);

	if (Isain_adcLoaded()) {
		// stop modules
		AMUX4_0_Stop();
		AMUX4_1_Stop();
		PGA_0_Stop();		// see the module datasheet
		PGA_1_Stop();		// might be good lpf!?
		DUALADC_Stop();	// try lower power later

		UnloadConfig_ain_adc();
	}
	
	if (Isain_capsenseLoaded()) {
		// stop modules
		Timer16_D_DisableInt();
		AMUX4_D_Stop();
		CMPPRG_D_Stop();
		Timer8_D_Stop();
		Timer16_D_Stop();
	
		UnloadConfig_ain_capsense();
	}
	
	if (Isaout_prspwm_aLoaded()) {
		// stop modules
		PRS8_0_Stop();
		PRS8_1_Stop();
		PRS8_2_Stop();
		PRS8_3_Stop();

		UnloadConfig_aout_prspwm_a();
	}
	
	if (Isaout_prspwm_bLoaded()) {
		// stop modules
		PRS8_4_Stop();
		PRS8_5_Stop();
		PRS8_6_Stop();
		PRS8_7_Stop();
	
		UnloadConfig_aout_prspwm_b();
	}
	
	if (Isaout_servo_aLoaded()) {
		// stop modules
		Counter8_0_Stop();
		Counter8_1_Stop();
		Counter8_2_Stop();
		Counter8_3_Stop();
	
		UnloadConfig_aout_servo_a();
	}
	
	if (Isaout_servo_bLoaded()) {
		// stop modules
		Counter8_4_Stop();
		Counter8_5_Stop();
		Counter8_6_Stop();
		Counter8_7_Stop();
	
		UnloadConfig_aout_servo_b();
	}

	init_ports();

	// wait for 8/512 = 15.625ms
	SleepTimer_SyncWait(8, SleepTimer_WAIT_RELOAD);
}
