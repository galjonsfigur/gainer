#ifndef __SERIALH__
#define __SERIALH__

#include <string>
using namespace std;

  
enum{
	kConfig0 =0,
	kConfig1,
	kConfig2,
	kConfig3,
	kConfig4,
	kConfig5,
	kConfig6,
	kConfig7,

	kNumConfig
};

//CSerial Class as interface
class CSerial
{
public:
	CSerial(void){}
	virtual ~CSerial(void){}
	
	virtual void DEBUG() = 0;
	virtual void setVerbose(bool verbose) = 0;
	virtual bool getVerbose() = 0;
	virtual void configuration(int mode) =0;
	virtual void turnOnLED() = 0;
	virtual void turnOffLED() =0;
	virtual void peekDigitalInput() =0;
	virtual void beginDigitalInput() = 0;
	virtual void endDigitalInput() =0;
	virtual void digitalOutput(int chs) =0;
	virtual void digitalOutput(bool* values) =0;
	virtual void setHigh(int ch) = 0;
	virtual void setHigh(int* chs) = 0;
	virtual void setLow(int ch) =0;
	virtual void setLow(int* chs) = 0;
	virtual void peekAnalogInput() =0;
	virtual void beginAnalogInput() =0;
	virtual void endAnalogInput() =0;
	virtual void analogOutput(int ch,int value) =0;
	virtual void analogOutput(int* value) =0;
	virtual void analogHighSampling() =0;
	virtual void analogAllSampling() =0;
	virtual void ampGainAGND(int gain)=0;
	virtual void ampGainDGND(int gain)=0;
	virtual void reboot() =0;
	virtual void scanLine(int line,int* values) =0;
	virtual void scanMatrix(int* values) =0;

private:
	virtual void initSerial(string port) = 0;
	virtual void sendSerial(string command) = 0;
	virtual void getSerial() = 0;
	
};

#endif // __SERIALH__