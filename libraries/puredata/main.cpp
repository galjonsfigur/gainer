#include <windows.h>
#include "GainerSerialWrapper.h"

using namespace std;

int main( void )
{
	//���Z���ڕW�@�ȉ��̃R�[�h�����삷�邱�� �܂����Ȃ���B
	CGainerSerialWrapper* gainer = new CGainerSerialWrapper("COM6");
	gainer->configuration(kConfig1);
	gainer->setVerbose(true);
	gainer->turnOnLED();
	gainer->turnOffLED();
	delete gainer;
	
	return 0;

}