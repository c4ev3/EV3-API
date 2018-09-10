
#ifndef EV3_TYPEDATA_H_
#define EV3_TYPEDATA_H_

#include <ev3_basictypes.h>

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
//#define   UART_CLEAR_CHANGED      _IOWR('u',3,UARTCTL)
#define   IIC_SET_CONN            _IOWR('i',2,DEVCON)
//#define   IIC_READ_TYPE_INFO      _IOWR('i',3,IICCTL)
//#define   IIC_SETUP               _IOWR('i',5,IICDAT)
//#define   IIC_SET                 _IOWR('i',6,IICSTR)

#endif //EV3_TYPEDATA_H_
