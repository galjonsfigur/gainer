#ifndef __SERIALH__
#define __SERIALH__

#include <string>
using namespace std;

  

//CSerial Class as interface
class CSerial
{
public:
	CSerial(void){}
	virtual ~CSerial(void){}
	
	virtual void DEBUG(){}
	virtual void setVerbose(bool verbose){}
	virtual bool getVerbose(){return true;}
	virtual void configuration(int mode){}
	virtual void turnOnLED(){}
	virtual void turnOffLED(){}
	virtual void peekDigitalInput(){}
	virtual void beginDigitalInput(){}
	virtual void endDigitalInput(){}
	virtual void digitalOutput(int chs){}
	virtual void digitalOutput(bool* values){}
	virtual void setHigh(int ch){}
	virtual void setHigh(int* chs){}
	virtual void setLow(int ch){}
	virtual void setLow(int* chs){}
	virtual void peekAnalogInput(){}
	virtual void beginAnalogInput(){}
	virtual void endAnalogInput(){}
	virtual void analogOutput(int ch,int value){}
	virtual void analogOutput(int* value){}
	virtual void analogHighSampling(){}
	virtual void analogAllSampling(){}
	virtual void ampGainAGND(int gain){}
	virtual void ampGainDGND(int gain){}
	virtual void reboot(){}
	virtual void scanLine(int line,int* values){}
	virtual void scanMatrix(int* values){}
};

#endif // __SERIALH__