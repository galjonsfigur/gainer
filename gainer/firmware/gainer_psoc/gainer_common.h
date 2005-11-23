#ifndef _GAINER_COMMON_H
#define _GAINER_COMMON_H

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "psocdynamic.h"

#define SERIAL_DEBUG_ENABLED 0

/**
 * common I/O Pin Definitions
 */
#define GET_DIN_1() (PRT0DR&0x40)	// P0[6]
#define GET_DIN_2() (PRT0DR&0x10)	// P0[4]
#define GET_DIN_3() (PRT0DR&0x04)	// P0[2]
#define GET_DIN_4() (PRT0DR&0x01)	// P0[0]

// NOTE: P1[5] is pull-downed, so should be always ZERO!!!
#define SET_DOUT_1_H() (PRT2DR|=0x01)				// P2[0]
#define SET_DOUT_2_H() (PRT1DR=(PRT1DR&0xDF)|0x40)	// P1[6]
#define SET_DOUT_3_H() (PRT1DR=(PRT1DR&0xDF)|0x10)	// P1[4]
#define SET_DOUT_4_H() (PRT1DR=(PRT1DR&0xDF)|0x04)	// P1[2]

// NOTE: P1[5]  is pull-downed, so should be always ZERO!!!
#define SET_DOUT_1_L() (PRT2DR&=0xFE)	// P2[0]
#define SET_DOUT_2_L() (PRT1DR&=0x9F)	// P1[6]
#define SET_DOUT_3_L() (PRT1DR&=0xCF)	// P1[4]
#define SET_DOUT_4_L() (PRT1DR&=0xDB)	// P1[2]

#define GET_BUTTON() (PRT1DR&0x20)				// P1[5]

// NOTE: P1[5]  is pull-downed, so should be always ZERO!!!
#define SET_LED_H() (PRT1DR=(PRT1DR&0xDF)|0x80)	// P1[7]
#define SET_LED_L() (PRT1DR&=0x5F)				// P1[7]

/**
 * global definitions
 */
enum {
	CONFIG_INIT = 0,
	CONFIG_START,
	CONFIG_A,
	CONFIG_B
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
extern WORD bContinuousAinMask;
extern BOOL bContinuousDinRequested;
extern BOOL bQuitRequested;
extern BYTE bCurrentConfig;
extern BYTE bRequestedConfig;
extern char cReplyBuffer[32];
extern const BYTE bGainTable[16];
extern WORD bAdcValue[16];
extern BYTE bAdcChannelNumber;
extern BYTE bAdcFlags;

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

#endif