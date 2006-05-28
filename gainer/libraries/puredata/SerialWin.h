#ifndef __SERIALWINH__
#define __SERIALWINH__

#include <windows.h>
#include "Serial.h"
#include "SerialWin.h"
class CSerialWin :
	public CSerial
{
public:
	CSerialWin(string port);
	virtual ~CSerialWin(void);

	 void DEBUG(void);
	 void setVerbose(bool verbose);
	 bool getVerbose(void);
	 void configuration(int mode);
	 void turnOnLED(void);
	 void turnOffLED(void);
	 void peekDigitalInput(void);
	 void beginDigitalInput(void);
	 void endDigitalInput(void);
	 void digitalOutput(int chs);
	 void digitalOutput(bool* values);
	 void setHigh(int ch);
	 void setHigh(int* chs);
	 void setLow(int ch);
	 void setLow(int* chs);
	 void peekAnalogInput(void);
	 void beginAnalogInput(void);
	 void endAnalogInput(void);
	 void analogOutput(int ch,int value);
	 void analogOutput(int* value);
	 void analogHighSampling(void);
	 void analogAllSampling(void);
	 void ampGainAGND(int gain);
	 void ampGainDGND(int gain);
	 void reboot(void);
	 void scanLine(int line,int* values);
	 void scanMatrix(int* values);

private:
	void initSerial(string port);
	void sendSerial(string command);
	void getSerial(void);

	//シリアルポート設定関係
	HANDLE hCom;
	DCB dcb;

	//コマンド
	string command;

};

#endif // __SERIALWINH__
