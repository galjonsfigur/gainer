#include "SerialWin.h"

// test
#include <iostream>
CSerialWin::CSerialWin(string port)
{
	std::cout << port << "open!" << std::endl;
}

CSerialWin::~CSerialWin()
{
}

void CSerialWin::DEBUG()
{ 
}

void CSerialWin::setVerbose(bool verbose)
{
}

bool CSerialWin::getVerbose()
{
	//not implemented
	return false;

}

void CSerialWin:: configuration(int mode)
{
	std::cout<< "mode" << mode << "configuration" << std::endl;
}

void CSerialWin:: turnOnLED()
{
	std::cout << "turn LED On" << std::endl;
}

void CSerialWin:: turnOffLED()
{ 
}

void CSerialWin:: peekDigitalInput()
{
}

void CSerialWin:: beginDigitalInput()
{ 
}

void CSerialWin:: endDigitalInput()
{ 
}

void CSerialWin:: digitalOutput(int chs)
{ 
}

void CSerialWin:: digitalOutput(bool* values)
{ 
}

void CSerialWin:: setHigh(int ch)
{

}

void CSerialWin:: setHigh(int* chs)
{
}

void CSerialWin:: setLow(int ch)
{ 
}

void CSerialWin:: setLow(int* chs)
{ 
}

void CSerialWin:: peekAnalogInput()
{
}

void CSerialWin:: beginAnalogInput()
{ 
}

void CSerialWin:: endAnalogInput()
{
}

void CSerialWin:: analogOutput(int ch,int value)
{ 
}

void CSerialWin:: analogOutput(int* value)
{ 
}

void CSerialWin:: analogHighSampling()
{
}

void CSerialWin:: analogAllSampling()
{ 
}

void CSerialWin:: ampGainAGND(int gain)
{
}

void CSerialWin:: ampGainDGND(int gain)
{
}

void CSerialWin:: reboot()
{
}

void CSerialWin:: scanLine(int line,int* values)
{ 
}

void CSerialWin:: scanMatrix(int* values)
{
}
