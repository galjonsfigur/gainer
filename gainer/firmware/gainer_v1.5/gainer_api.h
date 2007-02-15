#ifndef GAINER_API_H
#define GAINER_API_H

/*
 * GAINER I/O module specific APIs.
 * @author Shigeru Kobayashi
 */

/**
 * Error code definitions for Gainer I/O module
 */
enum {
	NO_ERROR = 0,
	SYNTAX_ERROR,
	CHECKSUM_ERROR,
	PORT_MODE_ERROR
};

/**
 * Serial report mode
 */
enum {
	REPORT_NONE = 0,
	REPORT_ONCE,
	REPORT_ALWAYS,
	REPORT_WHEN_CHANGED
};

/**
 * Parameter ID
 */
enum {
	PGA_GAIN = 0,	// both A and B
	PGA_REFERENCE	// both A and B
};

/**
 * Definitions for special ports
 */
enum port_number {
	PORT_BUTTON = 16,
	PORT_LED
};

/**
 * port mode
 */
typedef enum {
	NOT_INITIALIZED = 0,	//  0: not initialized (reset state)
	DIN_HIGH_Z,				//  1: din: High Z
	DIN_PULL_UP,			//  2: din: Pull Up
	DIN_PULL_DOWN,			//  3: din: Pull Down
	AIN_ADC,				//  4: ain: A/D Converter
	AIN_ADC_LPF,			//  5: ain: A/D Converter with LPF
	AIN_ADC_HPF,			//  6: ain: A/D Converter with HPF
	AIN_ADC_PEAK_HOLD,		//  7: ain: A/D Converter with Peak Hold
	AIN_CAPSENSE,			//  8: ain: CapSense
	DOUT_STRONG,			//  9: dout: Strong
	DOUT_OPEN_DRAIN_HIGH,	// 10: dout: Open Drain High
	DOUT_OPEN_DRAIN_LOW,	// 11: dout: Open Drain Low
	AOUT_PSEUDO_ANALOG,		// 12: aout: Pseudo Analog (PRS PWM)
	AOUT_SERVO,				// 13: aout: RC Servo (50Hz PWM)
} GPortMode;

void SyncWait(void);
void ScanInputs(void);
void ProcessCommands(void);
void ReportToHost(void);

BOOL DigitalWrite(BYTE port, BYTE value);
BYTE DigitalRead(BYTE port);
BOOL AnalogWrite(BYTE port, BYTE value);
BYTE AnalogRead(BYTE port);

BOOL SetPortMode(BYTE port, GPortMode mode);
GPortMode GetPortMode(BYTE port);

void Initialize(void);
void Reboot(void);

#endif // GAINER_API_H
