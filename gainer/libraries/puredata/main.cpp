#include <windows.h>

#include "GainerSerialWrapper.h"

int main( void )
{
	//���Z���ڕW�@�ȉ��̃R�[�h�����삷�邱�� �܂����Ȃ���B
	CGainerSerialWrapper* gainer = new CGainerSerialWrapper("COM6");
	gainer->configuration(kConfig0);
	gainer->turnOnLED();
	delete gainer;
	
	return 0;

}