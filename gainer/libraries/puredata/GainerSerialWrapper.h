/*
	
	CGainerSerialWrapper
	
	遠藤さんのやつがc++で使えるようにすればいいんじゃね？
	ちょっとC++っぽくしたり手抜きして最低限動くようにしたら
	いいのでは？って感じでお送り。
	けどなんかそのままじゃ使えないよなあ。返り値とか。

	SAITA Kazuki

!!!!!!!!!!!!!!DONE                     !!!!!!!!!!!!!!
!!!!!!!!!!!!!!HACK: Implementing       !!!!!!!!!!!!!!
!!!!!!!!!!!!!!HACK: Not Yet Implemented!!!!!!!!!!!!!!
CgainerSerialWrapper(string port);
~CgainerSerialWrapper(string port);

void configuration(int mode)
void turnOnLED()
void turnOffLED()

DEBUG()
void setVerbose(boolean verbose)
boolean getVerbose()

void configuration(int mode)
void turnOnLED()
void turnOffLED()
void peekDigitalInput()
void beginDigitalInput()
void endDigitalInput()
void digitalOutput(int chs)
void digitalOutput(boolean[] values)
void setHigh(int ch)
void setHigh(int[] chs)
void setLow(int ch)
void setLow(int[] chs)
void peekAnalogInput()
void beginAnalogInput()
void endAnalogInput()
void analogOutput(int ch,int value)
void analogOutput(int[] value)
void analogHighSampling()
void analogAllSampling()
void ampGainAGND(int gain)
void ampGainDGND(int gain)
void reboot()
void scanLine(int line,int[] values)
void scanMatrix(int[] values)

*/

#ifdef    WIN32
#pragma once
#endif    

#include <string>
#include "Serial.h"




class CGainerSerialWrapper
{
	//abstract interface
	CSerial* serial;
public:
	CGainerSerialWrapper(string port);
	~CGainerSerialWrapper(void);

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
