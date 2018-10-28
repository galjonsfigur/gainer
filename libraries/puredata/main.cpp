#include <windows.h>
#include "GainerSerialWrapper.h"

using namespace std;

int main( void )
{
	//超短期目標　以下のコードが動作すること まだしないよ。
	CGainerSerialWrapper* gainer = new CGainerSerialWrapper("COM6");
	gainer->configuration(kConfig1);
	gainer->setVerbose(true);
	gainer->turnOnLED();
	gainer->turnOffLED();
	delete gainer;
	
	return 0;

}