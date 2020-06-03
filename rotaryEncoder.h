/*
 * rotaryEncoder.h
 *
 * Created: 12-Jan-19 10:12:15 AM
 *  Author: Vlad
 */ 


#ifndef ROTARYENCODER_H_
#define ROTARYENCODER_H_


// Enable this to emit codes twice per step.
//#define HALF_STEP

// Enable weak pullups
#define ENABLE_PULLUPS

#define KEY_NONE 0x00 // No key pressed
#define KEY_A       4
#define KEY_B       5
#define KEY_C       6
#define KEY_D       7
#define KEY_E       8
#define KEY_F       9
#define KEY_G       10
#define KEY_H       11
#define KEY_I       12
#define KEY_J       13
#define KEY_K       14
#define KEY_L       15
#define KEY_M       16
#define KEY_N       17
#define KEY_O       18
#define KEY_P       19
#define KEY_Q       20
#define KEY_R       21
#define KEY_S       22
#define KEY_T       23
#define KEY_U       24
#define KEY_V       25
#define KEY_W       26
#define KEY_X       27
#define KEY_Y       28
#define KEY_Z       29
#define KEY_1       30
#define KEY_2       31
#define KEY_3       32
#define KEY_4       33
#define KEY_5       34
#define KEY_6       35
#define KEY_7       36
#define KEY_8       37
#define KEY_9       38
#define KEY_0       39

#define KEYCODE_ARROW_RIGHT		0x4F
#define KEYCODE_ARROW_LEFT		0x50
#define KEYCODE_ARROW_DOWN		0x51
#define KEYCODE_ARROW_UP		0x52

#define KEY_F1      58
#define KEY_F2      59
#define KEY_F3      60
#define KEY_F4      61
#define KEY_F5      62
#define KEY_F6      63
#define KEY_F7      64
#define KEY_F8      65
#define KEY_F9      66
#define KEY_F10     67
#define KEY_F11     68
#define KEY_F12     69

// multimedia keys
#define MMKEY_KB_VOL_UP			0x80 // do not use
#define MMKEY_KB_VOL_DOWN		0x81 // do not use
#define MMKEY_VOL_UP			0xE9
#define MMKEY_VOL_DOWN			0xEA
#define MMKEY_SCAN_NEXT_TRACK	0xB5
#define MMKEY_SCAN_PREV_TRACK	0xB6
#define MMKEY_STOP				0xB7
#define MMKEY_PLAYPAUSE			0xCD
#define MMKEY_MUTE				0xE2
#define MMKEY_BASSBOOST			0xE5
#define MMKEY_LOUDNESS			0xE7
#define MMKEY_KB_EXECUTE		0x74
#define MMKEY_KB_HELP			0x75
#define MMKEY_KB_MENU			0x76
#define MMKEY_KB_SELECT			0x77
#define MMKEY_KB_STOP			0x78
#define MMKEY_KB_AGAIN			0x79
#define MMKEY_KB_UNDO			0x7A
#define MMKEY_KB_CUT			0x7B
#define MMKEY_KB_COPY			0x7C
#define MMKEY_KB_PASTE			0x7D
#define MMKEY_KB_FIND			0x7E
#define MMKEY_KB_MUTE			0x7F // do not use

// system control keys
#define SYSCTRLKEY_POWER		0x01
#define SYSCTRLKEY_SLEEP		0x02
#define SYSCTRLKEY_WAKE			0x03

typedef enum _ENCODER_SPIN
{
	ENCODER_SPIN_DIRECTION_NONE = 0x00,
	ENCODER_SPIN_DIRECTION_LEFT	= 0x10,
	ENCODER_SPIN_DIRECTION_RIGHT= 0x20,
	ENCODER2_SPIN_DIRECTION_LEFT	= 0x11,
	ENCODER2_SPIN_DIRECTION_RIGHT= 0x21,
} ENCODER_SPIN_DIRECTION;

void rotaryEncoder_init();
void rotaryEncoder_process();
ENCODER_SPIN_DIRECTION rotaryEncoder_get_direction();




#endif /* ROTARYENCODER_H_ */
