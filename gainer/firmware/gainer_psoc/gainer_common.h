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
	CONFIG_8,			// C8:     :  *8:   0:   8: CapSense(4) + Digital In(4)
};

/**
 * public functions of CONFIG_START
 * implemented in main.c
 */
void Enter_Config_Start(void);
void Exit_Config_Start(void);
void Main_Config_Start(void);

/**
 * public functions of CONFIG_A (C1~C4)
 * implemented in config_a.c
 */
void Enter_Config_A(void);
void Exit_Config_A(void);
void Main_Config_A(void);

/**
 * public functions of CONFIG_B (C5~C6)
 * implemented in config_b.c
 */
void Enter_Config_B(void);
void Exit_Config_B(void);
void Main_Config_B(void);

/**
 * public functions of CONFIG_C (C7)
 * implemented in config_c.c
 */
void Enter_Config_C(void);
void Exit_Config_C(void);
void Main_Config_C(void);

/**
 * public functions of CONFIG_D (C8)
 * implemented in config_d.c
 */
void Enter_Config_D(void);
void Exit_Config_D(void);
void Main_Config_D(void);

typedef struct {
	BYTE bCurrentConfig;
	BYTE bRequestedConfig;

	BOOL bQuitRequested;

	BOOL bVerboseMode;

	char cReplyBuffer[32];

	BYTE bChannels_AIN;
	BYTE bChannels_AOUT;
	BYTE bChannels_DIN;
	BYTE bChannels_DOUT;

	BOOL bContinuousAinRequested;
	BYTE bContinuousAinMask;
	BOOL bContinuousDinRequested;

	char cLocalRxBuffer[128];
	BYTE bCommandLength;
} global_parameters;

extern global_parameters _gainer;

/**
 * utility functions
 */
#define HEX_TO_BYTE(X) (((X)>='0'&&(X)<='9')?((X)-'0'):((X)-'A'+10))
void ByteToHex(BYTE value, char *str);

void WaitForBriefSpells(void);
void PutErrorStringToReplyBuffer(void);

#endif
