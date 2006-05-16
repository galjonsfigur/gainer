#include "GainerSerialWrapper.h"

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
