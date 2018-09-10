
#ifndef EV3_IIC_H_
#define EV3_IIC_H_

#include "ev3_basictypes.h"
#include "ev3_typedata.h"

/*! \page IicModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the iic values for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

#define   IIC_DATA_LENGTH         MAX_DEVICE_DATALENGTH
#define   IIC_NAME_LENGTH         8

typedef   enum
{
	OK            = 0,                    //!< No errors to report
	BUSY          = 1,                    //!< Busy - try again
	FAIL          = 2,                    //!< Something failed
	STOP          = 4                     //!< Stopped
}
RESULT;
typedef   struct
{
	TYPES   TypeData[INPUTS][MAX_DEVICE_MODES]; //!< TypeData

#ifndef DISABLE_FAST_DATALOG_BUFFER
	uint16_t   Repeat[INPUTS][DEVICE_LOGBUF_SIZE];
	DATA8   Raw[INPUTS][DEVICE_LOGBUF_SIZE][IIC_DATA_LENGTH];      //!< Raw value from IIC device
	uint16_t   Actual[INPUTS];
	uint16_t   LogIn[INPUTS];
#else
	DATA8   Raw[INPUTS][IIC_DATA_LENGTH];      //!< Raw value from IIC device
#endif
	DATA8   Status[INPUTS];                     //!< Status
	DATA8   Changed[INPUTS];
	DATA8   Output[INPUTS][IIC_DATA_LENGTH];    //!< Bytes to IIC device
	DATA8   OutputLength[INPUTS];
}
IIC;

typedef   struct
{
	RESULT  Result;
	DATA8   Port;
	DATA8   Repeat;
	DATA16  Time;
	DATA8   WrLng;
	DATA8   WrData[IIC_DATA_LENGTH];
	DATA8   RdLng;
	DATA8   RdData[IIC_DATA_LENGTH];
}
IICDAT;



#endif //EV3_IIC_H_
