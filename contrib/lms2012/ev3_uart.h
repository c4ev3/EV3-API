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

#ifndef EV3_UART_H_
#define EV3_UART_H_

#include "ev3_basictypes.h"
#include "ev3_typedata.h"

#define    UART_DATA_LENGTH     MAX_DEVICE_DATALENGTH
#define    UART_BUFFER_SIZE     64
#define    UART_PORT_CHANGED    0x01                    //!< Input port changed
#define    UART_DATA_READY      0x08                    //!< Data is ready

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

typedef   struct
{
    TYPES   TypeData;
    DATA8   Port;
    DATA8   Mode;
}
        UARTCTL;

#endif //EV3_UART_H_
