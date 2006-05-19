#include <windows.h>

#include "GainerSerialWrapper.h"

int main( void )
{
	//超短期目標　以下のコードが動作すること まだしないよ。
	CGainerSerialWrapper* gainer = new CGainerSerialWrapper("COM6");
	gainer->configuration(kConfig0);
	gainer->turnOnLED();
	delete gainer;
	
	return 0;

}