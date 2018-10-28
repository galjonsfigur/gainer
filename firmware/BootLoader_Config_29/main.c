//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules


BYTE Boot_UART_cGetChar(void){
	BYTE bRxStatus;
	while (!(bRxStatus=Boot_RxD_bReadRxStatus() & RX8_RX_COMPLETE));
    return Boot_RxD_bReadRxData();
}

void Boot_UART_PutChar(char TxData){
	while (!(Boot_TxD_bReadTxStatus() & TX8_TX_BUFFER_EMPTY));
	Boot_TxD_SendData(TxData);     
}

void Boot_UART_CPutString(const char* pstr){
	while (*pstr != 0){
	   Boot_UART_PutChar(*pstr);
	   pstr++;
	}
}

const char leds[]={0x08,0x04,0x08,0x80,0x04,0x80,0x04,0x08,0x80};

void main()
{
    long int x;
    char i,z;
   // Initiate UART Module
	Boot_RxD_Start(RX8_PARITY_NONE);
	Boot_TxD_Start(TX8_PARITY_NONE);
	
	Boot_UART_CPutString("Hello from PSoC!");

    z=0x84;
    	
// 	while(1){
// 	   for (i=0;i<9;i++){
// 	    PRT1DR=leds[i] ;
//		for (x=0; x<20000; x++);
//       }
//  	}

	
	while(1){
       PRT1DR=z;
       z=~z;
	   i=Boot_RxD_cGetChar();
	   Boot_TxD_PutChar(i);
	}
}
