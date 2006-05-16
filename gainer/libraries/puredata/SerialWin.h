#ifndef __SERIALWINH__
#define __SERIALWINH__

#include "Serial.h"
#include "SerialWin.h"
class CSerialWin :
	public CSerial
{
public:
	CSerialWin(string port);
	~CSerialWin(void);

};

#endif // __SERIALWINH__
