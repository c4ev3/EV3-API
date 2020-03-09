/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/
 */

#ifndef EV3_TYPEDATA_H_
#define EV3_TYPEDATA_H_

#include "ev3_basictypes.h"

#define	   MAX_DEVICE_MODES		8                     //!< Max number of modes in one device
#define    INPUTS				4
#define    OUTPUTS				4
#define    MAX_DEVICE_DATALENGTH 32
#define    DEVICE_LOGBUF_SIZE	300

/*! \struct TYPES
 *          Device type data
 */
#define     TYPE_NAME_LENGTH    11
#define     SYMBOL_LENGTH       4       //!< Symbol length (not including zero)

typedef   struct // if data type changes - remember to change "cInputTypeDataInit" !
{
	int8_t     Name[TYPE_NAME_LENGTH + 1]; //!< Device name
	DATA8     Type;                       //!< Device type
	DATA8     Connection;
	DATA8     Mode;                       //!< Device mode
	DATA8     DataSets;
	DATA8     Format;
	DATA8     Figures;
	DATA8     Decimals;
	DATA8     Views;
	DATAF     RawMin;                     //!< Raw minimum value      (e.c. 0.0)
	DATAF     RawMax;                     //!< Raw maximum value      (e.c. 1023.0)
	DATAF     PctMin;                     //!< Percent minimum value  (e.c. -100.0)
	DATAF     PctMax;                     //!< Percent maximum value  (e.c. 100.0)
	DATAF     SiMin;                      //!< SI unit minimum value  (e.c. -100.0)
	DATAF     SiMax;                      //!< SI unit maximum value  (e.c. 100.0)
	uint16_t     InvalidTime;                //!< mS from type change to valid data
	uint16_t     IdValue;                    //!< Device id value        (e.c. 0 ~ UART)
	DATA8     Pins;                       //!< Device pin setup
	int8_t     Symbol[SYMBOL_LENGTH + 1];  //!< SI unit symbol
	uint16_t     Align;
}
TYPES;

/*! \page types

<hr size="1"/>

TYPE VALUES FOR BYTE CODES:
\n
    \verbatim */

typedef   enum
{
  MODE_KEEP                     =  -1,  //!< Mode value that won't change mode in byte codes (convenient place to define)
  TYPE_KEEP                     =   0,  //!< Type value that won't change type in byte codes

  // Types defined in "typedata.rcf"
  TYPE_NXT_TOUCH                =   1,  //!< Device is NXT touch sensor
  TYPE_NXT_LIGHT                =   2,  //!< Device is NXT light sensor
  TYPE_NXT_SOUND                =   3,  //!< Device is NXT sound sensor
  TYPE_NXT_COLOR                =   4,  //!< Device is NXT color sensor
  TYPE_NXT_ULTRASONIC           =   5,  //!< Device is NXT ultra sonic sensor
  TYPE_NXT_TEMPERATURE          =   6,  //!< Device is NXT temperature sensor
  TYPE_TACHO                    =   7,  //!< Device is EV3/NXT tacho motor
  TYPE_MINITACHO                =   8,  //!< Device is EV3 mini tacho motor
  TYPE_NEWTACHO                 =   9,  //!< Device is EV3 new tacho motor

  TYPE_TOUCH                    =  16,  //!< Device is EV3 touch sensor

  // Types defined in known EV3 digital devices
  TYPE_COLOR                    =  29,  //!< Device is EV3 color sensor
  TYPE_ULTRASONIC               =  30,  //!< Device is EV3 ultra sonic sensor
  TYPE_GYRO                     =  32,  //!< Device is EV3 gyro sensor
  TYPE_IR                       =  33,  //!< Device is EV3 IR sensor

  // Type range reserved for third party devices
  TYPE_THIRD_PARTY_START        =  50,
  TYPE_THIRD_PARTY_END          =  98,

  // Special types
  TYPE_ENERGYMETER              =  99,  //!< Device is energy meter
  TYPE_IIC_UNKNOWN              = 100,  //!< Device type is not known yet
  TYPE_NXT_TEST                 = 101,  //!< Device is a NXT ADC test sensor

  TYPE_NXT_IIC                  = 123,  //!< Device is NXT IIC sensor
  TYPE_TERMINAL                 = 124,  //!< Port is connected to a terminal
  TYPE_UNKNOWN                  = 125,  //!< Port not empty but type has not been determined
  TYPE_NONE                     = 126,  //!< Port empty or not available
  TYPE_ERROR                    = 127,  //!< Port not empty and type is invalid
}
TYPE;

/*  \endverbatim */


/*! \page connections Connections
 *
 *  \anchor connectiontypes
 *
 *  Following defines sets the input and output port connection type\n
 *
 *  These connection types are evaluated in the DCM driver \ref DcmDriver "Device Detection Manager Driver"
 *
 *  \verbatim
 */

typedef   enum
{
	CONN_UNKNOWN                  = 111,  //!< Connection is fake (test)

	CONN_DAISYCHAIN               = 117,  //!< Connection is daisy chained
	CONN_NXT_COLOR                = 118,  //!< Connection type is NXT color sensor
	CONN_NXT_DUMB                 = 119,  //!< Connection type is NXT analog sensor
	CONN_NXT_IIC                  = 120,  //!< Connection type is NXT IIC sensor

	CONN_INPUT_DUMB               = 121,  //!< Connection type is LMS2012 input device with ID resistor
	CONN_INPUT_UART               = 122,  //!< Connection type is LMS2012 UART sensor

	CONN_OUTPUT_DUMB              = 123,  //!< Connection type is LMS2012 output device with ID resistor
	CONN_OUTPUT_INTELLIGENT       = 124,  //!< Connection type is LMS2012 output device with communication
	CONN_OUTPUT_TACHO             = 125,  //!< Connection type is LMS2012 tacho motor with series ID resistance

	CONN_NONE                     = 126,  //!< Port empty or not available
	CONN_ERROR                    = 127,  //!< Port not empty and type is invalid
}
CONN;

typedef   struct
{
	DATA8     Connection[INPUTS];
	DATA8     Type[INPUTS];
	DATA8     Mode[INPUTS];
}
DEVCON;

#define   UART_SET_CONN           _IOWR('u',0,DEVCON)
//#define   UART_READ_MODE_INFO     _IOWR('u',1,UARTCTL)
//#define   UART_NACK_MODE_INFO     _IOWR('u',2,UARTCTL)
#define   UART_CLEAR_CHANGED      _IOWR('u',3,UARTCTL)
#define   IIC_SET_CONN            _IOWR('i',2,DEVCON)
//#define   IIC_READ_TYPE_INFO      _IOWR('i',3,IICCTL)
#define   IIC_SETUP               _IOWR('i',5,IICDAT)
//#define   IIC_SET                 _IOWR('i',6,IICSTR)

#endif //EV3_TYPEDATA_H_
