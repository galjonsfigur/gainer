#ifndef GAINER_API_H
#define GAINER_API_H

/*
 * GAINER I/O module specific APIs.
 * @author Shigeru Kobayashi
 */

/**
 * port mode
 */
typedef enum {
	NOT_INITIALIZED = 0,	// not initialized (reset state)
	DIN_HIGH_Z,				// din: High Z
	DIN_PULL_UP,			// din: Pull Up
	DIN_PULL_DOWN,			// din: Pull Down
	AIN_NORMAL,				// ain: Normal
	AIN_LPF,				// ain: LPF
	AIN_HPF,				// ain: HPF
	AIN_PEAK_HOLD,			// ain: Peak Hold
	AIN_CAPSENSE,			// ain: CapSense
	DOUT_STRONG,			// dout: Strong
	DOUT_OPEN_DRAIN_HIGH,	// dout: Open Drain High
	DOUT_OPEN_DRAIN_LOW,	// dout: Open Drain Low
	AOUT_PRSPWM,			// aout: Pseudo Analog (PRS PWM)
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

void Reboot(void);

#endif // GAINER_API_H
