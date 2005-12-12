#ifndef _GAINER_COMMON_H
#define _GAINER_COMMON_H

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "psocdynamic.h"

#define SERIAL_DEBUG_ENABLED 0

/**
 * common I/O Pin Definitions
 */
#define GET_BUTTON() (PRT1DR&0x20)				// P1[5]

// NOTE: P1[5] is pull-downed, so should be always ZERO!!!
#define SET_LED_H() (PRT1DR=(PRT1DR&0xDF)|0x80)	// P1[7]
#define SET_LED_L() (PRT1DR&=0x5F)				// P1[7]

/**
 * global definitions
 */
enum {
						//     ain :din :aout:dout
						//     ----:----:----:----
	CONFIG_START = 0,	// C0:    0:   0:   0:   0
	CONFIG_1,			// C1:    4:   4:   4:   4
	CONFIG_2,			// C2:    8:   0:   4:   4
	CONFIG_3,			// C3:    4:   4:   8:   0
	CONFIG_4,			// C4:    8:   0:   8:   0
	CONFIG_5,			// C5:    0:  16:   0:   0
	CONFIG_6,			// C6:    0:   0:   0:  16
	CONFIG_7,			// C7: LED matrix control mode???
	CONFIG_8,			// C8: CapSense mode???
	CONFIG_9			// C9: R/C servo control???
};

/**
 * public functions of CONFIG_START
 * implemented in main.c
 */
void Enter_Config_Start(void);
void Exit_Config_Start(void);
void Main_Config_Start(void);

/**
 * public functions of CONFIG_A
 * implemented in config_a.c
 */
void Enter_Config_A(void);
void Exit_Config_A(void);
void Main_Config_A(void);

/**
 * public functions of CONFIG_B
 * implemented in config_b.c
 */
void Enter_Config_B(void);
void Exit_Config_B(void);
void Main_Config_B(void);

extern BOOL bContinuousAinRequested;
extern BYTE bContinuousAinMask;
extern BOOL bContinuousDinRequested;
extern BOOL bQuitRequested;
extern BYTE bCurrentConfig;
extern BYTE bRequestedConfig;
extern char cReplyBuffer[32];
extern const BYTE bGainTable[16];
extern WORD wAdcValue[16];
extern BYTE bAdcChannelNumber;
extern BYTE bAdcFlags;
extern BOOL bVerboseMode;

extern BYTE bChannels_AIN;
extern BYTE bChannels_AOUT;
extern BYTE bChannels_DIN;
extern BYTE bChannels_DOUT;

/**
 * utility functions
 */
#define HEX_TO_BYTE(X) (((X)>='0'&&(X)<='9')?((X)-'0'):((X)-'A'+10))
void ByteToHex(BYTE value, char *str);

void WaitForBriefSpells(void);
void PutErrorStringToReplyBuffer(void);

#endif
