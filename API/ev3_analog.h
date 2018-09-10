
#ifndef EV3_ANALOG_H_
#define EV3_ANALOG_H_

#include <ev3_typedata.h>

/*! \page NxtColorMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *
 *  \verbatim
 */

#define   COLORS                        4
#define   CALPOINTS                     3

typedef   struct
{
	uint32_t   Calibration[CALPOINTS][COLORS];
	uint16_t   CalLimits[CALPOINTS - 1];
	uint16_t   Crc;
	uint16_t   ADRaw[COLORS];
	uint16_t   SensorRaw[COLORS];
}
COLORSTRUCT;

/*! \page AnalogModuleMemory
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the raw analogue values for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

typedef   struct
{
	DATA16  InPin1[INPUTS];         //!< Analog value at input port connection 1
	DATA16  InPin6[INPUTS];         //!< Analog value at input port connection 6
	DATA16  OutPin5[OUTPUTS];       //!< Analog value at output port connection 5
	DATA16  BatteryTemp;            //!< Battery temperature
	DATA16  MotorCurrent;           //!< Current flowing to motors
	DATA16  BatteryCurrent;         //!< Current flowing from the battery
	DATA16  Cell123456;             //!< Voltage at battery cell 1, 2, 3,4, 5, and 6
#ifndef DISABLE_FAST_DATALOG_BUFFER
	DATA16  Pin1[INPUTS][DEVICE_LOGBUF_SIZE];      //!< Raw value from analog device
	DATA16  Pin6[INPUTS][DEVICE_LOGBUF_SIZE];      //!< Raw value from analog device
	uint16_t   Actual[INPUTS];
	uint16_t   LogIn[INPUTS];
	uint16_t   LogOut[INPUTS];
#endif
#ifndef   DISABLE_OLD_COLOR
	COLORSTRUCT  NxtCol[INPUTS];
#endif
	DATA16  OutPin5Low[OUTPUTS];    //!< Analog value at output port connection 5 when connection 6 is low

	DATA8   Updated[INPUTS];

	DATA8   InDcm[INPUTS];          //!< Input port device types
	DATA8   InConn[INPUTS];

	DATA8   OutDcm[OUTPUTS];        //!< Output port device types
	DATA8   OutConn[OUTPUTS];
#ifndef DISABLE_PREEMPTED_VM
	uint16_t   PreemptMilliSeconds;
#endif
}
ANALOG;

#endif //EV3_ANALOG_H_
