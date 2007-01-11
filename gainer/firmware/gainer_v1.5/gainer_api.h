#ifndef GAINER_API_H
#define GAINER_API_H

/*
 * GAINER I/O module specific APIs.
 * @author Shigeru Kobayashi
 */

enum port_number {
	PORT_BUTTON = 16,
	PORT_LED
};

/**
 * port mode
 */
typedef enum {
	NOT_INITIALIZED = 0,	// not initialized (reset state)
	DIN_HIGH_Z,				// din: High Z
	DIN_PULL_UP,			// din: Pull Up
	DIN_PULL_DOWN,			// din: Pull Down
	AIN_ADC,				// ain: A/D Converter
	AIN_ADC_LPF,			// ain: A/D Converter with LPF
	AIN_ADC_HPF,			// ain: A/D Converter with HPF
	AIN_ADC_PEAK_HOLD,		// ain: A/D Converter with Peak Hold
	AIN_CAPSENSE,			// ain: CapSense
	DOUT_STRONG,			// dout: Strong
	DOUT_OPEN_DRAIN_HIGH,	// dout: Open Drain High
	DOUT_OPEN_DRAIN_LOW,	// dout: Open Drain Low
	AOUT_PSEUDO_ANALOG,		// aout: Pseudo Analog (PRS PWM)
	AOUT_SERVO,				// aout: RC Servo (50Hz PWM)
} GPortMode;

void SyncWait(void);
void ScanInputs(void);
void ProcessCommands(void);

BOOL DigitalWrite(BYTE port, BYTE value);
BYTE DigitalRead(BYTE port);
BOOL AnalogWrite(BYTE port, BYTE value);
BYTE AnalogRead(BYTE port);

BOOL SetPortMode(BYTE port, GPortMode mode);
GPortMode GetPortMode(BYTE port);

void Initialize(void);
void Reboot(void);

#endif // GAINER_API_H
