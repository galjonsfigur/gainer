#include "GainerSerialWrapper.h"

//concrete instance (OS depending)
#ifdef    WIN32
#include "SerialWin.h"
#endif    


CGainerSerialWrapper::CGainerSerialWrapper(string port)
{
	

//concrete instance (OS depending)
#ifdef    WIN32
	serial = new CSerialWin(port);
#endif    // WIN32

#ifdef OSX
//OSX implementation
#endif //OSX

#ifdef __linux__
//linux implementation
#endif // __linux__

}

CGainerSerialWrapper::~CGainerSerialWrapper(void)
{
	if (serial) delete serial;
}


void CGainerSerialWrapper::configuration(int mode)
{
	serial->configuration(mode);
}

void CGainerSerialWrapper::turnOnLED()
{
	serial->turnOnLED();
}
void CGainerSerialWrapper::turnOffLED()
{
	serial->turnOffLED();
}

void CGainerSerialWrapper::DEBUG()
{
	serial->DEBUG();
}
void CGainerSerialWrapper::setVerbose(bool verbose)
{
	serial->setVerbose(verbose);
}
bool CGainerSerialWrapper::getVerbose()
{
	return serial->getVerbose();
}

void CGainerSerialWrapper::peekDigitalInput()
{
	serial->peekDigitalInput();
}
void CGainerSerialWrapper::beginDigitalInput()
{
	serial->beginDigitalInput();
}
void CGainerSerialWrapper::endDigitalInput()
{
	serial->endDigitalInput();
}
void CGainerSerialWrapper::digitalOutput(int chs)
{
	serial->digitalOutput(chs);
}

void CGainerSerialWrapper::digitalOutput(bool* values)
{
	serial->digitalOutput(values);
}
void CGainerSerialWrapper::setHigh(int ch)
{
	serial->setHigh(ch);
}
void CGainerSerialWrapper:: setHigh(int* chs)
{
	serial->setHigh(chs);
}
void CGainerSerialWrapper::setLow(int ch)
{
	serial->setLow(ch);
}

void CGainerSerialWrapper::setLow(int* chs)
{
	serial->setLow(chs);
}

void CGainerSerialWrapper::peekAnalogInput()
{
	serial->peekAnalogInput();
}
void CGainerSerialWrapper::beginAnalogInput()
{
	serial->beginAnalogInput();
}
void CGainerSerialWrapper::endAnalogInput()
{
	serial->endAnalogInput();
}

void CGainerSerialWrapper::analogOutput(int ch,int value)
{
	serial->analogOutput(ch, value);
}
void CGainerSerialWrapper::analogOutput(int* value)
{
	serial->analogOutput(value);
}
void CGainerSerialWrapper:: analogHighSampling()
{
	serial->analogHighSampling();
}	
void CGainerSerialWrapper::analogAllSampling()
{
	serial->analogAllSampling();
}
void CGainerSerialWrapper::ampGainAGND(int gain)
{
	serial->ampGainAGND(gain);
}
void CGainerSerialWrapper::reboot()
{
	serial->reboot();
}
void CGainerSerialWrapper::scanLine(int line,int* values)
{
	serial->scanLine(line, values);
}
void CGainerSerialWrapper::scanMatrix(int* values)
{
	serial->scanMatrix(values);
}
