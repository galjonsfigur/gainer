#ifndef GAINER_PORTS_H
#define GAINER_PORTS_H

/**
 * Port related definitons and macros.
 * @author Shigeru Kobayashi
 */

#define PIN_0	0x01
#define PIN_1	0x02
#define PIN_2	0x04
#define PIN_3	0x08
#define PIN_4	0x10
#define PIN_5	0x20
#define PIN_6	0x40
#define PIN_7	0x80

#define digital_read_00() (((PRT0DR & PIN_7) == 0) ? 0 : 1)	// P0[7]
#define digital_read_01() (((PRT0DR & PIN_5) == 0) ? 0 : 1)	// P0[5]
#define digital_read_02() (((PRT0DR & PIN_3) == 0) ? 0 : 1)	// P0[3]
#define digital_read_03() (((PRT0DR & PIN_1) == 0) ? 0 : 1)	// P0[1]
#define digital_read_04() (((PRT0DR & PIN_6) == 0) ? 0 : 1)	// P0[6]
#define digital_read_05() (((PRT0DR & PIN_4) == 0) ? 0 : 1)	// P0[4]
#define digital_read_06() (((PRT0DR & PIN_2) == 0) ? 0 : 1)	// P0[2]
#define digital_read_07() (((PRT0DR & PIN_0) == 0) ? 0 : 1)	// P0[0]
#define digital_read_08() (((PRT2DR & PIN_7) == 0) ? 0 : 1)	// P2[7]
#define digital_read_09() (((PRT2DR & PIN_5) == 0) ? 0 : 1)	// P2[5]
#define digital_read_10() (((PRT2DR & PIN_3) == 0) ? 0 : 1)	// P2[3]
#define digital_read_11() (((PRT2DR & PIN_1) == 0) ? 0 : 1)	// P2[1]
#define digital_read_12() (((PRT2DR & PIN_0) == 0) ? 0 : 1)	// P2[0]
#define digital_read_13() (((PRT1DR & PIN_6) == 0) ? 0 : 1)	// P1[6]
#define digital_read_14() (((PRT1DR & PIN_4) == 0) ? 0 : 1)	// P1[4]
#define digital_read_15() (((PRT1DR & PIN_2) == 0) ? 0 : 1)	// P1[2]
#define digital_read_16() (((PRT1DR & PIN_5) == 0) ? 0 : 1)	// P1[5], Button
#define digital_read_17() (((PRT1DR & PIN_7) == 0) ? 0 : 1)	// P1[7], LED

#define digital_write_00(b) (gPort0Shadow = (b == 0) ? (gPort0Shadow & ~PIN_7) : (gPort0Shadow | PIN_7))
#define digital_write_01(b) (gPort0Shadow = (b == 0) ? (gPort0Shadow & ~PIN_5) : (gPort0Shadow | PIN_5))
#define digital_write_02(b) (gPort0Shadow = (b == 0) ? (gPort0Shadow & ~PIN_3) : (gPort0Shadow | PIN_3))
#define digital_write_03(b) (gPort0Shadow = (b == 0) ? (gPort0Shadow & ~PIN_1) : (gPort0Shadow | PIN_1))
#define digital_write_04(b) (gPort0Shadow = (b == 0) ? (gPort0Shadow & ~PIN_6) : (gPort0Shadow | PIN_6))
#define digital_write_05(b) (gPort0Shadow = (b == 0) ? (gPort0Shadow & ~PIN_4) : (gPort0Shadow | PIN_4))
#define digital_write_06(b) (gPort0Shadow = (b == 0) ? (gPort0Shadow & ~PIN_2) : (gPort0Shadow | PIN_2))
#define digital_write_07(b) (gPort0Shadow = (b == 0) ? (gPort0Shadow & ~PIN_0) : (gPort0Shadow | PIN_0))
#define digital_write_08(b) (gPort2Shadow = (b == 0) ? (gPort2Shadow & ~PIN_7) : (gPort2Shadow | PIN_7))
#define digital_write_09(b) (gPort2Shadow = (b == 0) ? (gPort2Shadow & ~PIN_5) : (gPort2Shadow | PIN_5))
#define digital_write_10(b) (gPort2Shadow = (b == 0) ? (gPort2Shadow & ~PIN_3) : (gPort2Shadow | PIN_3))
#define digital_write_11(b) (gPort2Shadow = (b == 0) ? (gPort2Shadow & ~PIN_1) : (gPort2Shadow | PIN_1))
#define digital_write_12(b) (gPort2Shadow = (b == 0) ? (gPort2Shadow & ~PIN_0) : (gPort2Shadow | PIN_0))
#define digital_write_13(b) (gPort1Shadow = (b == 0) ? (gPort1Shadow & ~PIN_6) : (gPort1Shadow | PIN_6))
#define digital_write_14(b) (gPort1Shadow = (b == 0) ? (gPort1Shadow & ~PIN_4) : (gPort1Shadow | PIN_4))
#define digital_write_15(b) (gPort1Shadow = (b == 0) ? (gPort1Shadow & ~PIN_2) : (gPort1Shadow | PIN_2))
#define digital_write_16(b) (gPort1Shadow = (b == 0) ? (gPort1Shadow & ~PIN_5) : (gPort1Shadow | PIN_5))
#define digital_write_17(b) (gPort1Shadow = (b == 0) ? (gPort1Shadow & ~PIN_7) : (gPort1Shadow | PIN_7))

#define AIN_PORT_MIN	0
#define AIN_PORT_MAX	7
#define AOUT_PORT_MIN	8
#define AOUT_PORT_MAX	15

#define NUM_OF_PORTS	18

#endif // GAINER_PORTS_H
