/*
	
	CGainerSerialWrapper
	
	遠藤さんのやつがc++で使えるようにすればいいんじゃね？
	ちょっとC++っぽくしたり手抜きして最低限動くようにしたら
	いいのでは？って感じでお送り。

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


class CGainerSerialWrapper
{
	//abstract interface
	CSerial* serial;
public:
	CGainerSerialWrapper(string port);
	~CGainerSerialWrapper(void);

};
