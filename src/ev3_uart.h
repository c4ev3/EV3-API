
#ifndef EV3_UART_H_
#define EV3_UART_H_

#include "ev3_basictypes.h"
#include "ev3_typedata.h"

/*! \page UartModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the uart values for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

#define    UART_DATA_LENGTH		MAX_DEVICE_DATALENGTH
#define    UART_BUFFER_SIZE		64

typedef   struct
{
	TYPES   TypeData[INPUTS][MAX_DEVICE_MODES]; //!< TypeData

#ifndef DISABLE_FAST_DATALOG_BUFFER
	uint16_t   Repeat[INPUTS][DEVICE_LOGBUF_SIZE];
	DATA8   Raw[INPUTS][DEVICE_LOGBUF_SIZE][UART_DATA_LENGTH];      //!< Raw value from UART device
	uint16_t   Actual[INPUTS];
	uint16_t   LogIn[INPUTS];
#else
	DATA8   Raw[INPUTS][UART_DATA_LENGTH];      //!< Raw value from UART device
#endif
	DATA8   Status[INPUTS];                     //!< Status
	DATA8   Output[INPUTS][UART_DATA_LENGTH];   //!< Bytes to UART device
	DATA8   OutputLength[INPUTS];
}
UART;

#endif //EV3_UART_H_
