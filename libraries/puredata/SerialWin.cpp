#include "SerialWin.h"

// test
#include <iostream>
CSerialWin::CSerialWin(string port)
{
	initSerial(port);
	
}

CSerialWin::~CSerialWin()
{
	if(hCom){
		CloseHandle(hCom);
		hCom = NULL;
	}

}

void CSerialWin::DEBUG()
{ 
}

void CSerialWin::setVerbose(bool verbose)
{
	if (verbose){
		command = "V1*";
	}
	else command = "V0*";

	sendSerial(command);

	return;

}

bool CSerialWin::getVerbose()
{
	//not implemented
	return false;

}

void CSerialWin:: configuration(int mode)
{
	
	switch (mode){
		case (kConfig0):
			command = "KONFIGURATION_0*";
			break;
		case (kConfig1):
			command = "KONFIGURATION_1*";
			break;
		case (kConfig2):
			command = "KONFIGURATION_2*";
			break;
		case (kConfig3):
			command = "KONFIGURATION_3*";
			break;
		case (kConfig4):
			command = "KONFIGURATION_4*";
			break;
		case (kConfig5):
			command = "KONFIGURATION_5*";
			break;
		case (kConfig6):
			command = "KONFIGURATION_6*";
			break;
		case (kConfig7):
			command = "KONFIGURATION_7*";
			break;
	}
	sendSerial(command);//モード名
}

void CSerialWin:: turnOnLED()
{
	sendSerial("h*");
	return;
}

void CSerialWin:: turnOffLED()
{ 
	sendSerial("l*");
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

void CSerialWin::initSerial(string port)
{
	//とりあえず開く
	hCom = ::CreateFile(
		(LPCSTR)port.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,//FILE_ATTRIBUTE_NORMAL,//|FILE_FLAG_OVERLAPPED, //必要かも
		NULL
	);
	if (hCom == INVALID_HANDLE_VALUE){
		cout << "invalid handle" << std::endl;
	}

	//開いたら設定
	GetCommState(hCom, &dcb); /* DCB を取得 */
	BuildCommDCB(TEXT("38400, N, 8, 1"),&dcb);
	SetCommState(hCom, &dcb); /* DCB を設定 */



}

void CSerialWin::sendSerial(string command)
{
	
	//エラー処理とかしてないのがすごいよね
	DWORD written;

	WriteFile(
		hCom,
		(LPCSTR)command.c_str(),
		lstrlen((LPCSTR)command.c_str()),
		&written,
		NULL
	);

	if (written != lstrlen(command.c_str()) ) {
		cout << "error" << endl;
	}


	return;
}

void CSerialWin::getSerial()
{
}
