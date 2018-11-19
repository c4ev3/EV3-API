/** \file ev3_constants.h
 * \brief Constants for various EV3 modules
 *
 * ev3_constants.h contains eclarations for various EV3 C API functions.
 *
 * License:
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Initial Developer of this code is John Hansen.
 * Portions created by John Hansen are Copyright (C) 2009-2013 John Hansen.
 * All Rights Reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * \author John Hansen (bricxcc_at_comcast.net)
 * \date 2013-06-14
 * \version 1
 *
 * ----------------------------------------------------------------------------
 *
 * \author Simon Rodriguez (HAB)
 * \date 2015-02-24
 * \version 2
 * Add: - Input port constants
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_constants_h
#define ev3_constants_h

#include <stdint.h>

/** @addtogroup MiscConstants
 * @{
 */
#define TRUE true
#define FALSE false

#define NUM_INPUTS     4   /*!< Number of input  ports in the system */
#define NUM_LEDS       4   /*!< Number of LEDs in the system */
#define LCD_WIDTH      178 /*!< LCD horizontal pixels */
#define LCD_HEIGHT     128 /*!< LCD vertical pixels */
#define TOPLINE_HEIGHT 10  /*!< Top line vertical pixels */

#define OWNER_NONE 0x0000 /*!< The object id of the owner of a resource (no owner) */

// daisy chain constants
#define LAYER_MASTER 0x00 /*!< The master layer */
#define LAYER_SLAVE1 0x10 /*!< The first slave layer */
#define LAYER_SLAVE2 0x20 /*!< The second slave layer */
#define LAYER_SLAVE3 0x40 /*!< The third slave layer */
#define LAYER_MASK   0x70 /*!< The layer mask */

/** @} */  // end of MiscConstants group


/** @addtogroup OutputModule
 * @{
 */
/** @addtogroup OutputModuleConstants
 * @{
 */
/** @defgroup OutputPortConstants Output port constants
 * Output port constants are used when calling motor control API functions.
 * @{
 */
#define OUT_A    0x01 /*!< Output port A */
#define OUT_B    0x02 /*!< Output port B */
#define OUT_C    0x04 /*!< Output port C */
#define OUT_D    0x08 /*!< Output port D */
#define OUT_AB   0x03 /*!< Output ports A and B */
#define OUT_AC   0x05 /*!< Output ports A and C */
#define OUT_AD   0x09 /*!< Output ports A and D */
#define OUT_BC   0x06 /*!< Output ports B and C */
#define OUT_BD   0x0a /*!< Output ports B and D */
#define OUT_CD   0x0c /*!< Output ports C and D */
#define OUT_ABC  0x07 /*!< Output ports A, B, and C */
#define OUT_BCD  0x0e /*!< Output ports B, C, and D */
#define OUT_ABCD 0x0f /*!< Output ports A, B, C, and D */
#define OUT_ALL  0x0f /*!< All output ports (A-D) */

#define OUT_MASK 0x0f /*!< The output port mask */
/** @} */  // end of OutputPortConstants group

/** @defgroup InputPortConstants Input port constants
 * Input port constants are used when calling sensor API functions.
 * @{
 */
#define IN_1    0x0 /*!< Input port 1 */
#define IN_2    0x1 /*!< Input port 2 */
#define IN_3    0x2 /*!< Input port 3 */
#define IN_4    0x3 /*!< Input port 4 */

/** @} */  // end of InputPortConstants group


/** @defgroup OutModeConstants Output port mode constants
 * Use these constants to configure the desired mode for the
 * specified motor(s): coast, brake, or on.
 * @{
 */
#define OUT_FLOAT 0x00
#define OUT_OFF   0x40
#define OUT_ON    0x80
/** @} */  // end of OutModeConstants group

/** @defgroup OutDirConstants Output port direction constants
 * Use these constants to configure the desired direction for the
 * specified motor(s): reverse, forward, or toggle.
 * @{
 */
#define OUT_REV    0x00
#define OUT_TOGGLE 0x40
#define OUT_FWD    0x80
/** @} */  // end of OutDirConstants group

#define OUT_POWER_DEFAULT -127

/** @defgroup OutRegModeConstants Output port regulation mode constants
 * Use these constants to configure the desired regulation mode for the
 * specified motor(s): none, speed regulation, multi-motor synchronization.
 * For the EV3 only the speed regulation constant has any effect. If you set
 * power instead of speed then no regulation is in effect (equivalent to
 * using OUT_REGMODE_IDLE with the NXT.
 * @{
 */
#define OUT_REGMODE_IDLE  0 /*!< No motor regulation. */
#define OUT_REGMODE_SPEED 1 /*!< Regulate a motor's speed. */
#define OUT_REGMODE_SYNC  2 /*!< Synchronize the rotation of two motors. */
/** @} */  // end of OutRegModeConstants group

/** @defgroup TachoResetConstants Tachometer counter reset flags
 * Use these constants to specify which of the tachometer counters
 * should be reset. Reset constants can be combined with bitwise OR.
 * \sa OnFwd(), OnRev(), etc...
 * @{
 */
#define RESET_NONE           0x00 /*!< No counters will be reset */
#define RESET_COUNT          0x08 /*!< Reset the internal tachometer counter */
#define RESET_BLOCK_COUNT    0x20 /*!< Reset the block tachometer counter */
#define RESET_ROTATION_COUNT 0x40 /*!< Reset the rotation counter */
#define RESET_BLOCKANDTACHO  0x28 /*!< Reset both the internal counter and the block counter */
#define RESET_ALL            0x68 /*!< Reset all tachometer counters */
/** @} */  // end of TachoResetConstants group

#define NUM_OUTPUTS 4   //!< Number of output ports in the system

// Reserved device types
#define DEVICE_TYPE_NXT_TOUCH           1  //!< Device is NXT touch sensor
#define DEVICE_TYPE_NXT_LIGHT           2  //!< Device is NXT light sensor
#define DEVICE_TYPE_NXT_SOUND           3  //!< Device is NXT sound sensor
#define DEVICE_TYPE_NXT_COLOR           4  //!< Device is NXT color sensor
#define DEVICE_TYPE_TACHO               7  //!< Device is a tacho motor
#define DEVICE_TYPE_MINITACHO           8  //!< Device is a mini tacho motor
#define DEVICE_TYPE_NEWTACHO            9  //!< Device is a new tacho motor
#define DEVICE_TYPE_THIRD_PARTY_START  50
#define DEVICE_TYPE_THIRD_PARTY_END    99
#define DEVICE_TYPE_IIC_UNKNOWN       100
#define DEVICE_TYPE_NXT_TEST          101  //!< Device is a NXT ADC test sensor
#define DEVICE_TYPE_NXT_IIC           123  //!< Device is NXT IIC sensor
#define DEVICE_TYPE_TERMINAL          124  //!< Port is connected to a terminal
#define DEVICE_TYPE_UNKNOWN           125  //!< Port not empty but type has not been determined
#define DEVICE_TYPE_NONE              126  //!< Port empty or not available
#define DEVICE_TYPE_ERROR             127  //!< Port not empty and type is invalid

#define MAX_DEVICE_DATALENGTH 32
#define MAX_DEVICE_MODES      8
#define UART_DATA_LENGTH      MAX_DEVICE_DATALENGTH
#define UART_BUFFER_SIZE      64
#define TYPE_NAME_LENGTH      11
#define SYMBOL_LENGTH         4
#define DEVICE_LOGBUF_SIZE    300
#define IIC_DATA_LENGTH       MAX_DEVICE_DATALENGTH
#define IIC_NAME_LENGTH       8

/** @addtogroup ButtonModule
 * @{
 */
/** @defgroup ButtonModuleConstants Button module constants
 * Constants that are part of the NXT firmware's Button module.
 * @{
 */

/** @defgroup ButtonNameConstants Button name constants
 * Constants to specify which button to use with button module functions.
 * \sa ButtonPressed(), ButtonState(), ReadButtonEx(),
 * @{
 */
// Button IDs
#define BUTTON_ID_UP     0x01 //!< The up button.
#define BUTTON_ID_ENTER  0x02 //!< The enter button.
#define BUTTON_ID_DOWN   0x04 //!< The down button.
#define BUTTON_ID_RIGHT  0x08 //!< The right button.
#define BUTTON_ID_LEFT   0x10 //!< The left button.
#define BUTTON_ID_ESCAPE 0x20 //!< The exit (escape) button.
#define BUTTON_ID_ALL    0x3f //!< All buttons mask.

#define BTN1 BUTTON_ID_ESCAPE //!< The exit (escape) button.
#define BTN2 BUTTON_ID_RIGHT  //!< The right button.
#define BTN3 BUTTON_ID_LEFT   //!< The left button.
#define BTN4 BUTTON_ID_ENTER  //!< The enter button.
#define BTN5 BUTTON_ID_UP     //!< The up button.
#define BTN6 BUTTON_ID_DOWN   //!< The down button.

#define BTNEXIT   BTN1 //!< The exit (escape) button.
#define BTNRIGHT  BTN2 //!< The right button.
#define BTNLEFT   BTN3 //!< The left button.
#define BTNCENTER BTN4 //!< The enter button.
#define BTNUP     BTN5 //!< The up button.
#define BTNDOWN   BTN6 //!< The down button.

#define NO_OF_BTNS 6  //!< The number of EV3 buttons.
#define NUM_BUTTONS 6 //!< Number of buttons in the system

/** @} */  // end of ButtonNameConstants group

/** @defgroup ButtonStateConstants ButtonState constants
 * Constants for use with the ButtonState() function. The _EV values can be
 * combined together using a bitwise OR operation.
 * \sa ButtonState()
 * @{
 */
#define BTNSTATE_PRESSED_EV         0x01 /*!< Button is in the pressed state. */
#define BTNSTATE_SHORT_RELEASED_EV  0x02 /*!< Button is in the short released state. */
#define BTNSTATE_LONG_PRESSED_EV    0x04 /*!< Button is in the long pressed state. */
#define BTNSTATE_LONG_RELEASED_EV   0x08 /*!< Button is in the long released state. */
#define BTNSTATE_PRESSED_STATE      0x80 /*!< A bitmask for the button pressed state */
#define BTNSTATE_NONE               0x10 /*!< The default button state. */
/** @} */  // end of ButtonStateConstants group

/** @} */  // end of ButtonModuleConstants group
/** @} */  // end of ButtonModule group


#define NXTCOLOR_IDX_RED    0 //!< Access the red value from color sensor value arrays
#define NXTCOLOR_IDX_GREEN  1 //!< Access the green value from color sensor value arrays
#define NXTCOLOR_IDX_BLUE   2 //!< Access the blue value from color sensor value arrays
#define NXTCOLOR_IDX_BLANK  3 //!< Access the blank value from color sensor value arrays
#define NUM_NXTCOLOR_IDX    4 //!< The number of entries in the color sensor value arrays

#define NXTCOLOR_CAL_POINT_0    0 //!< Calibration point 0
#define NXTCOLOR_CAL_POINT_1    1 //!< Calibration point 1
#define NXTCOLOR_CAL_POINT_2    2 //!< Calibration point 2
#define NUM_NXTCOLOR_CAL_POINTS 3 //!< The number of calibration points

#define INPUT_BLACKCOLOR  1 //!< The color value is black
#define INPUT_BLUECOLOR   2 //!< The color value is blue
#define INPUT_GREENCOLOR  3 //!< The color value is green
#define INPUT_YELLOWCOLOR 4 //!< The color value is yellow
#define INPUT_REDCOLOR    5 //!< The color value is red
#define INPUT_WHITECOLOR  6 //!< The color value is white

#define FILETYPE_UNKNOWN  0x00
#define FILETYPE_FOLDER   0x01
#define FILETYPE_SOUND    0x02
#define FILETYPE_BYTECODE 0x03
#define FILETYPE_GRAPHICS 0x04
#define FILETYPE_DATALOG  0x05
#define FILETYPE_PROGRAM  0x06
#define FILETYPE_TEXT     0x07
#define FILETYPE_SDCARD   0x10
#define FILETYPE_USBSTICK 0x20
#define NUM_FILETYPES     0x21 //!< Maximum icon types supported by the VM

#define FONTTYPE_NORMAL 0
#define FONTTYPE_SMALL  1
#define FONTTYPE_LARGE  2
#define FONTTYPE_TINY   3
#define NUM_FONTTYPES   4 //!< Maximum font types supported by the VM

#define ICONTYPE_NORMAL 0
#define ICONTYPE_SMALL  1
#define ICONTYPE_LARGE  2
#define ICONTYPE_MENU   3
#define ICONTYPE_ARROW  4
#define NUM_ICONTYPES   5 //!< Maximum icon types supported by the VM

#define NICON_NONE         -1
#define NICON_RUN          0
#define NICON_FOLDER       1
#define NICON_FOLDER2      2
#define NICON_USB          3
#define NICON_SD           4
#define NICON_SOUND        5
#define NICON_IMAGE        6
#define NICON_SETTINGS     7
#define NICON_ONOFF        8
#define NICON_SEARCH       9
#define NICON_WIFI         10
#define NICON_CONNECTIONS  11
#define NICON_ADD_HIDDEN   12
#define NICON_TRASHBIN     13
#define NICON_VISIBILITY   14
#define NICON_KEY          15
#define NICON_CONNECT      16
#define NICON_DISCONNECT   17
#define NICON_UP           18
#define NICON_DOWN         19
#define NICON_WAIT1        20
#define NICON_WAIT2        21
#define NICON_BLUETOOTH    22
#define NICON_INFO         23
#define NICON_TEXT         24
#define NICON_QUESTIONMARK 27
#define NICON_INFO_FILE    28
#define NICON_DISC         29
#define NICON_CONNECTED    30
#define NICON_OBP          31
#define NICON_OBD          32
#define NICON_OPENFOLDER   33
#define NICON_BRICK1       34
#define NUM_NICONS         35

#define PATH_SIZE     84  //!< Max path size excluding trailing forward slash including zero termination
#define NAME_SIZE     32  //!< Max name size including zero termination (must be divideable by 4)
#define EXT_SIZE      5   //!< Max extension size including dot and zero termination
#define FILENAME_SIZE 120 //!< Max filename size including path, name, extension and termination (must be divideable by 4)
#define MAC_SIZE      18  //!< Max WIFI MAC size including zero termination
#define IP_SIZE       16  //!< Max WIFI IP size including zero termination
#define BT_SIZE       13  //!< Max bluetooth address size including zero termination

#define LED_BLACK        0  //!< LED black pattern
#define LED_GREEN        1  //!< LED green pattern
#define LED_RED          2  //!< LED red pattern
#define LED_ORANGE       3  //!< LED orange pattern
#define LED_GREEN_FLASH  4  //!< LED green flash pattern
#define LED_RED_FLASH    5  //!< LED red flash pattern
#define LED_ORANGE_FLASH 6  //!< LED orange flash pattern
#define LED_GREEN_PULSE  7  //!< LED green pulse pattern
#define LED_RED_PULSE    8  //!< LED red pulse pattern
#define LED_ORANGE_PULSE 9  //!< LED orange pulse pattern
#define NUM_LED_PATTERNS 10 //!< The number of LED patterns


/** @addtogroup CommandModuleConstants
 * @{
 */
/** @defgroup TimeConstants Time constants
 * Constants for use with the Wait() function.
 * \sa Wait()
 * @{
 */
#define MS_1        1 /*!< 1 millisecond */
#define MS_2        2 /*!< 2 milliseconds */
#define MS_3        3 /*!< 3 milliseconds */
#define MS_4        4 /*!< 4 milliseconds */
#define MS_5        5 /*!< 5 milliseconds */
#define MS_6        6 /*!< 6 milliseconds */
#define MS_7        7 /*!< 7 milliseconds */
#define MS_8        8 /*!< 8 milliseconds */
#define MS_9        9 /*!< 9 milliseconds */
#define MS_10      10 /*!< 10 milliseconds */
#define MS_20      20 /*!< 20 milliseconds */
#define MS_30      30 /*!< 30 milliseconds */
#define MS_40      40 /*!< 40 milliseconds */
#define MS_50      50 /*!< 50 milliseconds */
#define MS_60      60 /*!< 60 milliseconds */
#define MS_70      70 /*!< 70 milliseconds */
#define MS_80      80 /*!< 80 milliseconds */
#define MS_90      90 /*!< 90 milliseconds */
#define MS_100    100 /*!< 100 milliseconds */
#define MS_150    150 /*!< 150 milliseconds */
#define MS_200    200 /*!< 200 milliseconds */
#define MS_250    250 /*!< 250 milliseconds */
#define MS_300    300 /*!< 300 milliseconds */
#define MS_350    350 /*!< 350 milliseconds */
#define MS_400    400 /*!< 400 milliseconds */
#define MS_450    450 /*!< 450 milliseconds */
#define MS_500    500 /*!< 500 milliseconds */
#define MS_600    600 /*!< 600 milliseconds */
#define MS_700    700 /*!< 700 milliseconds */
#define MS_800    800 /*!< 800 milliseconds */
#define MS_900    900 /*!< 900 milliseconds */
#define SEC_1    1000 /*!< 1 second */
#define SEC_2    2000 /*!< 2 seconds */
#define SEC_3    3000 /*!< 3 seconds */
#define SEC_4    4000 /*!< 4 seconds */
#define SEC_5    5000 /*!< 5 seconds */
#define SEC_6    6000 /*!< 6 seconds */
#define SEC_7    7000 /*!< 7 seconds */
#define SEC_8    8000 /*!< 8 seconds */
#define SEC_9    9000 /*!< 9 seconds */
#define SEC_10  10000 /*!< 10 seconds */
#define SEC_15  15000 /*!< 15 seconds */
#define SEC_20  20000 /*!< 20 seconds */
#define SEC_30  30000 /*!< 30 seconds */
#define MIN_1   60000 /*!< 1 minute */

/** @} */  // end of TimeConstants group
/** @} */ // end of CommandModuleConstants group

/** @addtogroup SoundModule
 * @{
 */

/** @defgroup SoundModuleConstants Sound module constants
 * Constants that are part of the EV3 firmware's Sound module.
 * @{
 */

/** @defgroup SoundStateConstants Sound State constants
 * Constants for use with the Sound module state functions.
 * @{
 */
#define SOUND_STATE_IDLE         0 /*!< Idle, ready for start sound */
#define SOUND_STATE_SETUP_FILE   1 /*!< Transient state when playing a sound file */
#define SOUND_STATE_FILE         2 /*!< Playing a sound file */
#define SOUND_STATE_FILE_LOOPING 3 /*!< Playing a sound file with looping */
#define SOUND_STATE_TONE         4 /*!< Playing a tone */
#define SOUND_STATE_TONE_LOOPING 5 /*!< Playing a tone with looping (not possible) */
#define SOUND_STATE_STOP         6 /*!< Stopping a previous operation */
/** @} */  // end of SoundStateConstants group

/** @defgroup RCXSoundConstants RCX and Scout sound constants
 * Constants for use when playing standard RCX and Scout sounds.
 * @{
 */
#define SOUND_CLICK       0 /*!< Play the standard key click sound */
#define SOUND_DOUBLE_BEEP 1 /*!< Play the standard double beep sound */
#define SOUND_DOWN        2 /*!< Play the standard sweep down sound */
#define SOUND_UP          3 /*!< Play the standard sweep up sound */
#define SOUND_LOW_BEEP    4 /*!< Play the standard low beep sound */
#define SOUND_FAST_UP     5 /*!< Play the standard fast up sound */
/** @} */  // end of RCXSoundConstants group

/** @defgroup ToneConstants Tone constants
 * Constants for use in the PlayTone() API function.
 * \sa PlayTone()
 * @{
 */
#define TONE_C2      65 /*!< Second octave C */
#define TONE_CS2     69 /*!< Second octave C sharp */
#define TONE_D2      73 /*!< Second octave D */
#define TONE_DS2     78 /*!< Second octave D sharp */
#define TONE_E2      82 /*!< Second octave E */
#define TONE_F2      87 /*!< Second octave F */
#define TONE_FS2     92 /*!< Second octave F sharp */
#define TONE_G2      98 /*!< Second octave G */
#define TONE_GS2     104 /*!< Second octave G sharp */
#define TONE_A2      110 /*!< Second octave A */
#define TONE_AS2     117 /*!< Second octave A sharp */
#define TONE_B2      123 /*!< Second octave B */
#define TONE_C3      131 /*!< Third octave C */
#define TONE_CS3     139 /*!< Third octave C sharp */
#define TONE_D3      147 /*!< Third octave D */
#define TONE_DS3     156 /*!< Third octave D sharp */
#define TONE_E3      165 /*!< Third octave E */
#define TONE_F3      175 /*!< Third octave F */
#define TONE_FS3     185 /*!< Third octave F sharp */
#define TONE_G3      196 /*!< Third octave G */
#define TONE_GS3     208 /*!< Third octave G sharp */
#define TONE_A3      220 /*!< Third octave A */
#define TONE_AS3     233 /*!< Third octave A sharp */
#define TONE_B3      247 /*!< Third octave B */
#define TONE_C4      262 /*!< Fourth octave C */
#define TONE_CS4     277 /*!< Fourth octave C sharp */
#define TONE_D4      294 /*!< Fourth octave D */
#define TONE_DS4     311 /*!< Fourth octave D sharp */
#define TONE_E4      330 /*!< Fourth octave E */
#define TONE_F4      349 /*!< Fourth octave F */
#define TONE_FS4     370 /*!< Fourth octave F sharp */
#define TONE_G4      392 /*!< Fourth octave G */
#define TONE_GS4     415 /*!< Fourth octave G sharp */
#define TONE_A4      440 /*!< Fourth octave A */
#define TONE_AS4     466 /*!< Fourth octave A sharp */
#define TONE_B4      494 /*!< Fourth octave B */
#define TONE_C5      523 /*!< Fifth octave C */
#define TONE_CS5     554 /*!< Fifth octave C sharp */
#define TONE_D5      587 /*!< Fifth octave D */
#define TONE_DS5     622 /*!< Fifth octave D sharp */
#define TONE_E5      659 /*!< Fifth octave E */
#define TONE_F5      698 /*!< Fifth octave F */
#define TONE_FS5     740 /*!< Fifth octave F sharp */
#define TONE_G5      784 /*!< Fifth octave G */
#define TONE_GS5     831 /*!< Fifth octave G sharp */
#define TONE_A5      880 /*!< Fifth octave A */
#define TONE_AS5     932 /*!< Fifth octave A sharp */
#define TONE_B5      988 /*!< Fifth octave B */
#define TONE_C6     1047 /*!< Sixth octave C */
#define TONE_CS6    1109 /*!< Sixth octave C sharp */
#define TONE_D6     1175 /*!< Sixth octave D */
#define TONE_DS6    1245 /*!< Sixth octave D sharp */
#define TONE_E6     1319 /*!< Sixth octave E */
#define TONE_F6     1397 /*!< Sixth octave F */
#define TONE_FS6    1480 /*!< Sixth octave F sharp */
#define TONE_G6     1568 /*!< Sixth octave G */
#define TONE_GS6    1661 /*!< Sixth octave G sharp */
#define TONE_A6     1760 /*!< Sixth octave A */
#define TONE_AS6    1865 /*!< Sixth octave A sharp */
#define TONE_B6     1976 /*!< Sixth octave B */
#define TONE_C7     2093 /*!< Seventh octave C */
#define TONE_CS7    2217 /*!< Seventh octave C sharp */
#define TONE_D7     2349 /*!< Seventh octave D */
#define TONE_DS7    2489 /*!< Seventh octave D sharp */
#define TONE_E7     2637 /*!< Seventh octave E */
#define TONE_F7     2794 /*!< Seventh octave F */
#define TONE_FS7    2960 /*!< Seventh octave F sharp */
#define TONE_G7     3136 /*!< Seventh octave G */
#define TONE_GS7    3322 /*!< Seventh octave G sharp */
#define TONE_A7     3520 /*!< Seventh octave A */
#define TONE_AS7    3729 /*!< Seventh octave A sharp */
#define TONE_B7     3951 /*!< Seventh octave B */

#define NOTE_WHOLE   1000            /*!< The duration of a whole note (ms) */
#define NOTE_HALF    (NOTE_WHOLE/2)  /*!< The duration of a half note (ms) */
#define NOTE_QUARTER (NOTE_WHOLE/4)  /*!< The duration of a quarter note (ms) */
#define NOTE_EIGHT   (NOTE_WHOLE/8)  /*!< The duration of an eighth note (ms) */
#define NOTE_SIXTEEN (NOTE_WHOLE/16) /*!< The duration of an sixteenth note (ms) */

/** @} */  // end of ToneConstants group

/** @} */  // end of SoundModuleConstants group
/** @} */  // end of SoundModule group


/** @addtogroup DisplayModuleConstants
 * @{
 */
/** @defgroup LineConstants Line number constants
 * Line numbers for use with DrawText system function.
 * \sa TextOut(), NumOut()
 * @{
 */
#define LCD_LINE8 112 //*!< The 8th line of the LCD screen
#define LCD_LINE7 96  //*!< The 7th line of the LCD screen
#define LCD_LINE6 80  //*!< The 6th line of the LCD screen
#define LCD_LINE5 64  //*!< The 5th line of the LCD screen
#define LCD_LINE4 48  //*!< The 4th line of the LCD screen
#define LCD_LINE3 32  //*!< The 3rd line of the LCD screen
#define LCD_LINE2 16  //*!< The 2nd line of the LCD screen
#define LCD_LINE1 0   //*!< The 1st line of the LCD screen
/** @} */  // end of LineConstants group
/** @} */  // end of DisplayModuleConstants group

/** @addtogroup DisplayModule
 * @{
 */
/** @defgroup DisplayModuleConstants Display module constants
 * Constants that are part of the NXT firmware's Display module.
 * @{
 */
/** @defgroup DisplayExecuteFunctionConstants DisplayExecuteFunction constants
 * Constants that are for use with the DisplayExecuteFunction system call.
 * \warning These options require the enhanced NBC/NXC firmware
 * @{
 */
#define DISPLAY_ERASE_ALL       0x00     /*!< W - erase entire screen     (CMD,x,x,x,x,x) */
#define DISPLAY_PIXEL           0x01     /*!< W - set pixel (on/off)      (CMD,true/false,X,Y,x,x) */
#define DISPLAY_HORIZONTAL_LINE 0x02     /*!< W - draw horizontal line    (CMD,true/false,X1,Y1,X2,x) */
#define DISPLAY_VERTICAL_LINE   0x03     /*!< W - draw vertical line      (CMD,true/false,X1,Y1,x,Y2) */
#define DISPLAY_CHAR            0x04     /*!< W - draw char (actual font) (CMD,true,X1,Y1,Char,x) */
#define DISPLAY_ERASE_LINE      0x05     /*!< W - erase a single line     (CMD,x,LINE,x,x,x) */
#define DISPLAY_FILL_REGION     0x06     /*!< W - fill screen region      (CMD,true/false,X1,Y1,X2,Y2) */
#define DISPLAY_FRAME           0x07     /*!< W - draw a frame (on/off)   (CMD,true/false,X1,Y1,X2,Y2) */
/** @} */  // end of DisplayExecuteFunctionConstants group

/** @defgroup DisplayDrawOptionConstants Drawing option constants
 * Constants that are for specifying drawing options in several display module API functions.
 * Bits 0 & 1 (values 0,1,2,3) control screen clearing behaviour (Not within RIC files).
 * Bit 2 (value 4) controls the NOT operation, i.e. draw in white or invert text/graphics.
 * Bits 3 & 4 (values 0,8,16,24) control pixel logical combinations (COPY/AND/OR/XOR).
 * Bit 5 (value 32) controls shape filling, or overrides text/graphic bitmaps with set pixels.
 * These may be ORed together for the full instruction
 * (e.g., DRAW_OPT_NORMAL|DRAW_OPT_LOGICAL_XOR)
 * These operations are resolved into the separate, common parameters
 * defined in 'c_display.iom' before any drawing function is called.
 * Note that when drawing a RIC file, the initial 'DrawingOptions' parameter
 * supplied in the drawing instruction controls screen clearing, but nothing else.
 * The 'CopyOptions' parameter from each instruction in the RIC file then controls
 * graphic operations, but the screen-clearing bits are ignored.
 * \sa TextOut(), NumOut(), PointOut(), LineOut(), CircleOut(), RectOut(),
 * PolyOut(), EllipseOut(), FontTextOut(), FontNumOut(), GraphicOut(),
 * GraphicArrayOut()
 * \warning These options require the enhanced NBC/NXC firmware
 * @{
 */
#define DRAW_OPT_NORMAL                     (0x0000) /*!< Normal drawing */
#define DRAW_OPT_CLEAR_WHOLE_SCREEN         (0x0001) /*!< Clear the entire screen before drawing */
#define DRAW_OPT_CLEAR_EXCEPT_STATUS_SCREEN (0x0002) /*!< Clear the screen except for the status line before drawing */

#define DRAW_OPT_CLEAR_PIXELS               (0x0004) /*!< Clear pixels while drawing (aka draw in white) */
#define DRAW_OPT_CLEAR                      (0x0004) /*!< Clear pixels while drawing (aka draw in white) */
#define DRAW_OPT_INVERT                     (0x0004) /*!< Invert text or graphics */

#define DRAW_OPT_LOGICAL_COPY               (0x0000) /*!< Draw pixels using a logical copy operation */
#define DRAW_OPT_LOGICAL_AND                (0x0008) /*!< Draw pixels using a logical AND operation */
#define DRAW_OPT_LOGICAL_OR                 (0x0010) /*!< Draw pixels using a logical OR operation */
#define DRAW_OPT_LOGICAL_XOR                (0x0018) /*!< Draw pixels using a logical XOR operation */

#define DRAW_OPT_FILL_SHAPE                 (0x0020) /*!< Fill the shape while drawing (rectangle, circle, ellipses, and polygon) */

#define DRAW_OPT_CLEAR_SCREEN_MODES         (0x0003) /*!< Bit mask for the clear screen modes */
#define DRAW_OPT_LOGICAL_OPERATIONS         (0x0018) /*!< Bit mask for the logical drawing operations */

#define DRAW_OPT_POLYGON_POLYLINE           (0x0400) /*!< When drawing polygons, do not close (i.e., draw a polyline instead) */

#define DRAW_OPT_CLEAR_LINE                 (0x0800) /*!< When drawing text, clear the entire line before drawing the text */
#define DRAW_OPT_CLEAR_EOL                  (0x1000) /*!< When drawing text, clear to the end of the line after drawing the text */

/** @addtogroup TimerModule
 * @{
 */
/** @addtogroup TimerModuleConstants
 * @{
 */
#define CS_TIMER_1    0 //!< Centisecond timer number 1 (100 ticks per second)
#define CS_TIMER_2    1 //!< Centisecond timer number 2 (100 ticks per second)
#define CS_TIMER_3    2 //!< Centisecond timer number 3 (100 ticks per second)
#define CS_TIMER_4    3 //!< Centisecond timer number 4 (100 ticks per second)
#define NUM_CS_TIMERS 4 //!< The number of centisecond timers

#define MS_TIMER_1    0 //!< Millisecond timer number 1 (1000 ticks per second)
#define MS_TIMER_2    1 //!< Millisecond timer number 2 (1000 ticks per second)
#define MS_TIMER_3    2 //!< Millisecond timer number 3 (1000 ticks per second)
#define MS_TIMER_4    3 //!< Millisecond timer number 4 (1000 ticks per second)
#define NUM_MS_TIMERS 4 //!< The number of millisecond timers

#define US_TIMER_1    0 //!< Microsecond timer number 1 (1000000 ticks per second)
#define US_TIMER_2    1 //!< Microsecond timer number 2 (1000000 ticks per second)
#define US_TIMER_3    2 //!< Microsecond timer number 3 (1000000 ticks per second)
#define US_TIMER_4    3 //!< Microsecond timer number 4 (1000000 ticks per second)
#define NUM_US_TIMERS 4 //!< The number of microsecond timers

/** @} */  // end of TimerModuleConstants group
/** @} */  // end of TimerModule group


/** @defgroup DeviceNameConstants Device name constants
 * Constants for the device names of each EV3 firmware module.
 * @{
 */
#define LMS_PWM_DEVICE            "lms_pwm"             /*!< PWM device name */
#define LMS_PWM_DEVICE_NAME       "/dev/lms_pwm"        /*!< PWM device file name */
#define LMS_MOTOR_DEVICE          "lms_motor"           /*!< TACHO device name */
#define LMS_MOTOR_DEVICE_NAME     "/dev/lms_motor"      /*!< TACHO device file name */
#define LMS_ANALOG_DEVICE         "lms_analog"          /*!< ANALOG device name */
#define LMS_ANALOG_DEVICE_NAME    "/dev/lms_analog"     /*!< ANALOG device file name */
#define LMS_POWER_DEVICE          "lms_power"           /*!< POWER device name */
#define LMS_POWER_DEVICE_NAME     "/dev/lms_power"      /*!< POWER device file name */
#define LMS_DCM_DEVICE            "lms_dcm"             /*!< DCM device name */
#define LMS_DCM_DEVICE_NAME       "/dev/lms_dcm"        /*!< DCM device file name */
#define LMS_UI_DEVICE             "lms_ui"              /*!< UI device name */
#define LMS_UI_DEVICE_NAME        "/dev/lms_ui"         /*!< UI device file name */
#define LMS_LCD_DEVICE            "lms_display"         /*!< DISPLAY device name */
#define LMS_LCD_DEVICE_NAME       "/dev/fb0"            /*!< DISPLAY device file name */
#define LMS_UART_DEVICE           "lms_uart"            /*!< UART device name */
#define LMS_UART_DEVICE_NAME      "/dev/lms_uart"       /*!< UART device file name */
#define LMS_USBDEV_DEVICE         "lms_usbdev"          /*!< USB device */
#define LMS_USBDEV_DEVICE_NAME    "/dev/lms_usbdev"     /*!< USB device */
#define LMS_USBHOST_DEVICE        "lms_usbhost"         /*!< USB host */
#define LMS_USBHOST_DEVICE_NAME   "/dev/lms_usbhost"    /*!< USB host */
#define LMS_SOUND_DEVICE          "lms_sound"           /*!< SOUND device name */
#define LMS_SOUND_DEVICE_NAME     "/dev/lms_sound"      /*!< SOUND device */
#define LMS_IIC_DEVICE            "lms_iic"             /*!< IIC device name */
#define LMS_IIC_DEVICE_NAME       "/dev/lms_iic"        /*!< IIC device */
#define LMS_BT_DEVICE             "lms_bt"              /*!< BT device name */
#define LMS_BT_DEVICE_NAME        "/dev/lms_bt"         /*!< BT device */
#define LMS_UPDATE_DEVICE         "lms_update"          /*!< UPDATE device name */
#define LMS_UPDATE_DEVICE_NAME    "/dev/lms_update"     /*!< UPDATE device */
#define LMS_TEST_PIN_DEVICE       "lms_tst_pin"         /*!< TEST pin device name */
#define LMS_TEST_PIN_DEVICE_NAME  "/dev/lms_tst_pin"    /*!< TEST pin device file name */
#define LMS_TEST_UART_DEVICE      "lms_tst_uart"        /*!< TEST UART device name */
#define LMS_TEST_UART_DEVICE_NAME "/dev/lms_tst_uart"   /*!< TEST UART device file name */
/** @} */  // end of DeviceNameConstants group

/** @defgroup OpcodeConstants Opcode constants
 * Constants for the EV3 firmware VM opcodes.
 * @{
 */
// opcodes
#define opError                     0x00 /*!< opError */
#define opNop                       0x01 /*!< opNop */
#define opProgramStop               0x02 /*!< opProgramStop */
#define opProgramStart              0x03 /*!< opProgramStart */
#define opObjectStop                0x04 /*!< opObjectStop */
#define opObjectStart               0x05 /*!< opObjectStart */
#define opObjectTrigger             0x06 /*!< opObjectTrigger */
#define opObjectWait                0x07 /*!< opObjectWait */
#define opReturn                    0x08 /*!< opReturn */
#define opCall                      0x09 /*!< opCall */
#define opObjectEnd                 0x0A /*!< opObjectEnd */
#define opSleep                     0x0B /*!< opSleep */
#define opProgramInfo               0x0C /*!< opProgramInfo */
#define opLabel                     0x0D /*!< opLabel */
#define opProbe                     0x0E /*!< opProbe */
#define opDo                        0x0F /*!< opDo */
#define opAdd1                      0x10 /*!< opAdd1 */
#define opAdd2                      0x11 /*!< opAdd2 */
#define opAdd4                      0x12 /*!< opAdd4 */
#define opAddF                      0x13 /*!< opAddF */
#define opSub1                      0x14 /*!< opSub1 */
#define opSub2                      0x15 /*!< opSub2 */
#define opSub4                      0x16 /*!< opSub4 */
#define opSubF                      0x17 /*!< opSubF */
#define opMul1                      0x18 /*!< opMul1 */
#define opMul2                      0x19 /*!< opMul2 */
#define opMul4                      0x1A /*!< opMul4 */
#define opMulF                      0x1B /*!< opMulF */
#define opDiv1                      0x1C /*!< opDiv1 */
#define opDiv2                      0x1D /*!< opDiv2 */
#define opDiv4                      0x1E /*!< opDiv4 */
#define opDivF                      0x1F /*!< opDivF */
#define opOr1                       0x20 /*!< opOr1 */
#define opOr2                       0x21 /*!< opOr2 */
#define opOr4                       0x22 /*!< opOr4 */
#define opAnd1                      0x24 /*!< opAnd1 */
#define opAnd2                      0x25 /*!< opAnd2 */
#define opAnd4                      0x26 /*!< opAnd4 */
#define opXor1                      0x28 /*!< opXor1 */
#define opXor2                      0x29 /*!< opXor2 */
#define opXor4                      0x2A /*!< opXor4 */
#define opRl1                       0x2C /*!< opRl1 */
#define opRl2                       0x2D /*!< opRl2 */
#define opRl4                       0x2E /*!< opRl4 */
#define opInitBytes                 0x2F /*!< opInitBytes */
#define opMove11                    0x30 /*!< opMove11 */
#define opMove12                    0x31 /*!< opMove12 */
#define opMove14                    0x32 /*!< opMove14 */
#define opMove1F                    0x33 /*!< opMove1F */
#define opMove21                    0x34 /*!< opMove21 */
#define opMove22                    0x35 /*!< opMove22 */
#define opMove24                    0x36 /*!< opMove24 */
#define opMove2F                    0x37 /*!< opMove2F */
#define opMove41                    0x38 /*!< opMove41 */
#define opMove42                    0x39 /*!< opMove42 */
#define opMove44                    0x3A /*!< opMove44 */
#define opMove4F                    0x3B /*!< opMove4F */
#define opMoveF1                    0x3C /*!< opMoveF1 */
#define opMoveF2                    0x3D /*!< opMoveF2 */
#define opMoveF4                    0x3E /*!< opMoveF4 */
#define opMoveFF                    0x3F /*!< opMoveFF */
#define opJmp                       0x40 /*!< opJmp */
#define opJmpFalse                  0x41 /*!< opJmpFalse */
#define opJmpTrue                   0x42 /*!< opJmpTrue */
#define opJmpNan                    0x43 /*!< opJmpNan */
#define opCmpLT1                    0x44 /*!< opCmpLT1 */
#define opCmpLT2                    0x45 /*!< opCmpLT2 */
#define opCmpLT4                    0x46 /*!< opCmpLT4 */
#define opCmpLTF                    0x47 /*!< opCmpLTF */
#define opCmpGT1                    0x48 /*!< opCmpGT1 */
#define opCmpGT2                    0x49 /*!< opCmpGT2 */
#define opCmpGT4                    0x4A /*!< opCmpGT4 */
#define opCmpGTF                    0x4B /*!< opCmpGTF */
#define opCmpEQ1                    0x4C /*!< opCmpEQ1 */
#define opCmpEQ2                    0x4D /*!< opCmpEQ2 */
#define opCmpEQ4                    0x4E /*!< opCmpEQ4 */
#define opCmpEQF                    0x4F /*!< opCmpEQF */
#define opCmpNEQ1                   0x50 /*!< opCmpNEQ1 */
#define opCmpNEQ2                   0x51 /*!< opCmpNEQ2 */
#define opCmpNEQ4                   0x52 /*!< opCmpNEQ4 */
#define opCmpNEQF                   0x53 /*!< opCmpNEQF */
#define opCmpLTEQ1                  0x54 /*!< opCmpLTEQ1 */
#define opCmpLTEQ2                  0x55 /*!< opCmpLTEQ2 */
#define opCmpLTEQ4                  0x56 /*!< opCmpLTEQ4 */
#define opCmpLTEQF                  0x57 /*!< opCmpLTEQF */
#define opCmpGTEQ1                  0x58 /*!< opCmpGTEQ1 */
#define opCmpGTEQ2                  0x59 /*!< opCmpGTEQ2 */
#define opCmpGTEQ4                  0x5A /*!< opCmpGTEQ4 */
#define opCmpGTEQF                  0x5B /*!< opCmpGTEQF */
#define opSelect1                   0x5C /*!< opSelect1 */
#define opSelect2                   0x5D /*!< opSelect2 */
#define opSelect4                   0x5E /*!< opSelect4 */
#define opSelectF                   0x5F /*!< opSelectF */
#define opSystem                    0x60 /*!< opSystem */
#define opPortConvertOutput         0x61 /*!< opPortConvertOutput */
#define opPortConvertInput          0x62 /*!< opPortConvertInput */
#define opNote2Freq                 0x63 /*!< opNote2Freq */
#define opJmpLT1                    0x64 /*!< opJmpLT1 */
#define opJmpLT2                    0x65 /*!< opJmpLT2 */
#define opJmpLT4                    0x66 /*!< opJmpLT4 */
#define opJmpLTF                    0x67 /*!< opJmpLTF */
#define opJmpGT1                    0x68 /*!< opJmpGT1 */
#define opJmpGT2                    0x69 /*!< opJmpGT2 */
#define opJmpGT4                    0x6A /*!< opJmpGT4 */
#define opJmpGTF                    0x6B /*!< opJmpGTF */
#define opJmpEQ1                    0x6C /*!< opJmpEQ1 */
#define opJmpEQ2                    0x6D /*!< opJmpEQ2 */
#define opJmpEQ4                    0x6E /*!< opJmpEQ4 */
#define opJmpEQF                    0x6F /*!< opJmpEQF */
#define opJmpNEQ1                   0x70 /*!< opJmpNEQ1 */
#define opJmpNEQ2                   0x71 /*!< opJmpNEQ2 */
#define opJmpNEQ4                   0x72 /*!< opJmpNEQ4 */
#define opJmpNEQF                   0x73 /*!< opJmpNEQF */
#define opJmpLTEQ1                  0x74 /*!< opJmpLTEQ1 */
#define opJmpLTEQ2                  0x75 /*!< opJmpLTEQ2 */
#define opJmpLTEQ4                  0x76 /*!< opJmpLTEQ4 */
#define opJmpLTEQF                  0x77 /*!< opJmpLTEQF */
#define opJmpGTEQ1                  0x78 /*!< opJmpGTEQ1 */
#define opJmpGTEQ2                  0x79 /*!< opJmpGTEQ2 */
#define opJmpGTEQ4                  0x7A /*!< opJmpGTEQ4 */
#define opJmpGTEQF                  0x7B /*!< opJmpGTEQF */
#define opInfo                      0x7C /*!< opInfo */
#define opStrings                   0x7D /*!< opStrings */
#define opMemoryWrite               0x7E /*!< opMemoryWrite */
#define opMemoryRead                0x7F /*!< opMemoryRead */
#define opUIFlush                   0x80 /*!< opUIFlush */
#define opUIRead                    0x81 /*!< opUIRead */
#define opUIWrite                   0x82 /*!< opUIWrite */
#define opUIButton                  0x83 /*!< opUIButton */
#define opUIDraw                    0x84 /*!< opUIDraw */
#define opTimerWait                 0x85 /*!< opTimerWait */
#define opTimerReady                0x86 /*!< opTimerReady */
#define opTimerRead                 0x87 /*!< opTimerRead */
#define opBreakpoint0               0x88 /*!< opBreakpoint0 */
#define opBreakpoint1               0x89 /*!< opBreakpoint1 */
#define opBreakpoint2               0x8A /*!< opBreakpoint2 */
#define opBreakpoint3               0x8B /*!< opBreakpoint3 */
#define opBreakpointSet             0x8C /*!< opBreakpointSet */
#define opMath                      0x8D /*!< opMath */
#define opRandom                    0x8E /*!< opRandom */
#define opTimerReadUS               0x8F /*!< opTimerReadUS */
#define opKeepAlive                 0x90 /*!< opKeepAlive */
#define opComRead                   0x91 /*!< opComRead */
#define opComWrite                  0x92 /*!< opComWrite */
#define opSound                     0x94 /*!< opSound */
#define opSoundTest                 0x95 /*!< opSoundTest */
#define opSoundReady                0x96 /*!< opSoundReady */
#define opInputSample               0x97 /*!< opInputSample */
#define opInputDeviceList           0x98 /*!< opInputDeviceList */
#define opInputDevice               0x99 /*!< opInputDevice */
#define opInputRead                 0x9A /*!< opInputRead */
#define opInputTest                 0x9B /*!< opInputTest */
#define opInputReady                0x9C /*!< opInputReady */
#define opInputReadSI               0x9D /*!< opInputReadSI */
#define opInputReadext              0x9E /*!< opInputReadext */
#define opInputWrite                0x9F /*!< opInputWrite */
#define opOutputGetType             0xA0 /*!< opOutputGetType */
#define opOutputSetType             0xA1 /*!< opOutputSetType */
#define opOutputReset               0xA2 /*!< opOutputReset */
#define opOutputStop                0xA3 /*!< opOutputStop */
#define opOutputPower               0xA4 /*!< opOutputPower */
#define opOutputSpeed               0xA5 /*!< opOutputSpeed */
#define opOutputStart               0xA6 /*!< opOutputStart */
#define opOutputPolarity            0xA7 /*!< opOutputPolarity */
#define opOutputRead                0xA8 /*!< opOutputRead */
#define opOutputTest                0xA9 /*!< opOutputTest */
#define opOutputReady               0xAA /*!< opOutputReady */
#define opOutputPosition            0xAB /*!< opOutputPosition */
#define opOutputStepPower           0xAC /*!< opOutputStepPower */
#define opOutputTimePower           0xAD /*!< opOutputTimePower */
#define opOutputStepSpeed           0xAE /*!< opOutputStepSpeed */
#define opOutputTimeSpeed           0xAF /*!< opOutputTimeSpeed */
#define opOutputStepSync            0xB0 /*!< opOutputStepSync */
#define opOutputTimeSync            0xB1 /*!< opOutputTimeSync */
#define opOutputClearCount          0xB2 /*!< opOutputClearCount */
#define opOutputGetCount            0xB3 /*!< opOutputGetCount */
#define opOutputProgramStop         0xB4 /*!< opOutputProgramStop */
#define opFile                      0xC0 /*!< opFile */
#define opArray                     0xC1 /*!< opArray */
#define opArrayWrite                0xC2 /*!< opArrayWrite */
#define opArrayRead                 0xC3 /*!< opArrayRead */
#define opArrayAppend               0xC4 /*!< opArrayAppend */
#define opMemoryUsage               0xC5 /*!< opMemoryUsage */
#define opFilename                  0xC6 /*!< opFilename */
#define opRead1                     0xC8 /*!< opRead1 */
#define opRead2                     0xC9 /*!< opRead2 */
#define opRead4                     0xCA /*!< opRead4 */
#define opReadF                     0xCB /*!< opReadF */
#define opWrite1                    0xCC /*!< opWrite1 */
#define opWrite2                    0xCD /*!< opWrite2 */
#define opWrite4                    0xCE /*!< opWrite4 */
#define opWriteF                    0xCF /*!< opWriteF */
#define opComReadY                  0xD0 /*!< opComReadY */
#define opComReadData               0xD1 /*!< opComReadData */
#define opComWriteData              0xD2 /*!< opComWriteData */
#define opComGet                    0xD3 /*!< opComGet */
#define opComSet                    0xD4 /*!< opComSet */
#define opComTest                   0xD5 /*!< opComTest */
#define opComRemove                 0xD6 /*!< opComRemove */
#define opComWriteFile              0xD7 /*!< opComWriteFile */
#define opMailboxOpen               0xD8 /*!< opMailboxOpen */
#define opMailboxWrite              0xD9 /*!< opMailboxWrite */
#define opMailboxRead               0xDA /*!< opMailboxRead */
#define opMailboxTest               0xDB /*!< opMailboxTest */
#define opMailboxReady              0xDC /*!< opMailboxReady */
#define opMailboxClose              0xDD /*!< opMailboxClose */
#define opTest                      0xFF /*!< opTest */
/** @} */  // end of OpcodeConstants group

#endif  // ev3_constants_h

#ifdef __cplusplus
}
#endif
