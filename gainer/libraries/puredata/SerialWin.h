#ifndef __SERIALWINH__
#define __SERIALWINH__

#include "Serial.h"
#include "SerialWin.h"
class CSerialWin :
	public CSerial
{
public:
	CSerialWin(string port);
	virtual ~CSerialWin(void);

	 void DEBUG();
	 void setVerbose(bool verbose);
	 bool getVerbose();
	 void configuration(int mode);
	 void turnOnLED();
	 void turnOffLED();
	 void peekDigitalInput();
	 void beginDigitalInput();
	 void endDigitalInput();
	 void digitalOutput(int chs);
	 void digitalOutput(bool* values);
	 void setHigh(int ch);
	 void setHigh(int* chs);
	 void setLow(int ch);
	 void setLow(int* chs);
	 void peekAnalogInput();
	 void beginAnalogInput();
	 void endAnalogInput();
	 void analogOutput(int ch,int value);
	 void analogOutput(int* value);
	 void analogHighSampling();
	 void analogAllSampling();
	 void ampGainAGND(int gain);
	 void ampGainDGND(int gain);
	 void reboot();
	 void scanLine(int line,int* values);
	 void scanMatrix(int* values);

};

#endif // __SERIALWINH__
