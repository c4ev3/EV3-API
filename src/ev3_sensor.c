/*
* EV3 Sensor API
*
* Copyright (C) 2014 Carsten Zeiffert
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/
*
* ----------------------------------------------------------------------------
*
* \author Simón Rodriguez Perez(Hochschule Aschaffenburg)
* \date 2015-02-28
* \version 2
* \note Correct readout for Touch-, Sonar- and Lightsensor
*
* ----------------------------------------------------------------------------
*
* \author Simón Rodriguez Perez
* \date 2016-04-20
* \version 3
* \note Correct readout for Gyroscop and Infrared Sensor
*
*/
#include "ev3_sensor.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include "ev3.h"
#include "../copied/lms2012/ev3_analog.h"
#include "../copied/lms2012/ev3_iic.h"
#include "../copied/lms2012/ev3_uart.h"

/***********************************/
/**
 * The EV3 firmware uses a build-in device type list in order to know how to read
 * data from the various external devices.
 * The list of types can be found in the EV3 firmware source code or in the "EV3
 * Firmware Development Kit" document (downloadable from the Lego website).
 */

// TOUCH
#define TOUCH_TYPE 16
#define TOUCH_PRESS_MODE 0 	// Press


// Ultrasonic
#define US_TYPE 30
#define US_DIST_CM_MODE 0 	// Dist in cm
#define US_DIST_MM_MODE 0 	// Dist in mm
#define US_DIST_IN_MODE 1 	// Dist in inch
#define US_LISTEN_MODE  2   // Presence of other ultrasonic sensors

// Infrared
#define IR_TYPE 33
#define IR_PROX_MODE 0		// Proximity
#define IR_SEEK_MODE 1		// Seek
#define IR_REMOTE_MODE 2	// Remote Control

// IIC 
#define IIC_TYPE 100
#define IIC_BYTE_MODE 0

// NXT Compass
#define NXT_COMPASS_IIC_ADDRESS 0x01

// NXT IR
#define NXT_IR_IIC_ADDRESS 0x08
#define NXT_IR_REGISTER_DC 0x42
#define NXT_IR_REGISTER_AC 0x49

// NXT Temperture
#define NXT_TEMP_TYPE 6
#define NXT_TEMP_C_MODE 0	// Temperature in C
#define NXT_TEMP_F_MODE 1	// Temperature in F

// NXT Sound
#define NXT_SOUND_TYPE 3
#define NXT_SOUND_DB_MODE 0
#define NXT_SOUND_DBA_MODE 1



// Mode of ev3_inputs
int sensor_setup_NAME[INPUTS];
int ir_sensor_channel[INPUTS];

/********************************************************************************************/
/**
* Initialisation of the Sensorfunctions
* modified by: Simón Rodriguez Perez
* 		 date: 2015-02-28
*
*/
int SensorsInit()
{
	int i;
	for (i = 0; i < INPUTS; i++)
	{
		ir_sensor_channel[i] = 0;
	}

	return -1;
}


void exitNxtIRSensorsIfNeeded();

bool SensorsExit()
{

	exitNxtIRSensorsIfNeeded();
	/// TODO: Exit also the compass

	return true;
}





enum      IIC_STATE
{
  IIC_IDLE,
  IIC_INIT,
  IIC_RESTART,
  IIC_ENABLE,
  IIC_NXT_TEMP_START,
  IIC_NXT_TEMP_WRITE,
  IIC_NXT_TEMP_READ,
  IIC_MANUFACTURER_START,
  IIC_MANUFACTURER_WRITE,
  IIC_MANUFACTURER_READ,
  IIC_TYPE_START,
  IIC_TYPE_WRITE,
  IIC_TYPE_READ,
  IIC_SETUP_WAIT,
  IIC_SETUP_START,
  IIC_SETUP_WRITE,
  IIC_SETUP_READ,
  IIC_WAITING,
  IIC_WRITING,
  IIC_READING,
  IIC_REPEAT,
  IIC_ERROR,
  IIC_EXIT,
  IIC_STATES
};

DEVCON devCon;

int setSensorMode(int sensorPort, int name);

/********************************************************************************************/
/**
* Initialisation for one Sensor 
* modified by: Simón Rodriguez Perez
* 		 date: 2015-02-28
* 		 note: Sensors are working now, but only one sensor is working at once
*
*/
int SetSensorMode(int sensorPort, int modeName)
{

	if (sensorPort < 0 || sensorPort >= INPUTS)
		return -1;

	int res = setSensorMode(sensorPort, modeName);
	if (res == -1) {
	    return res;
	}
	applySensorMode();
	Wait(100);
}

/********************************************************************************************/
/**
* Initialisation of all Sensors for working parallel 
* author: Simón Rodriguez Perez
* note: the function can only be called once at the beginning
*
*/
int SetAllSensorMode(int name_1, int name_2, int name_3, int name_4)
{
	int sensorPort = 0;

	int name[4] = {0};

	name[0] = name_1;
	name[1] = name_2;
	name[2] = name_3;
	name[3] = name_4;

	// Setup of Input
	for(sensorPort=0; sensorPort<4; sensorPort++)
	{
	    int res = setSensorMode(sensorPort, name[sensorPort]);
	    if (res == -1) {
	    	return -1;
	    }
	}
	applySensorMode();

	return 0;
}

int setSensorMode(int sensorPort, int name) {
    sensor_setup_NAME[sensorPort] = name;
    char pins;
    switch (name) {
        case NO_SEN:
            break;
            // Ultrasonic
        case US_DIST_CM:
            devCon.Connection[sensorPort] = CONN_INPUT_UART;
            devCon.Type[sensorPort] = US_TYPE;
            devCon.Mode[sensorPort] = US_DIST_CM_MODE;
            break;
        case US_DIST_MM:
            devCon.Connection[sensorPort] = CONN_INPUT_UART;
            devCon.Type[sensorPort] = US_TYPE;
            devCon.Mode[sensorPort] = US_DIST_MM_MODE;
            break;
        case US_DIST_IN:
            devCon.Connection[sensorPort] = CONN_INPUT_UART;
            devCon.Type[sensorPort] = US_TYPE;
            devCon.Mode[sensorPort] = US_DIST_IN_MODE;
            break;
        case US_LISTEN:
            devCon.Connection[sensorPort] = CONN_INPUT_UART;
            devCon.Type[sensorPort] = US_TYPE;
            devCon.Mode[sensorPort] = US_LISTEN_MODE;
            break;
            // Infrared
        case IR_PROX:
            devCon.Connection[sensorPort] = CONN_INPUT_UART;
            devCon.Type[sensorPort] = IR_TYPE;
            devCon.Mode[sensorPort] = IR_PROX_MODE;
            break;
        case IR_SEEK:
            devCon.Connection[sensorPort] = CONN_INPUT_UART;
            devCon.Type[sensorPort] = IR_TYPE;
            devCon.Mode[sensorPort] = IR_SEEK_MODE;
            break;
        case IR_REMOTE:
            devCon.Connection[sensorPort] = CONN_INPUT_UART;
            devCon.Type[sensorPort] = IR_TYPE;
            devCon.Mode[sensorPort] = IR_REMOTE_MODE;
            break;
            // NXT
        case NXT_IR_SEEKER_DC:
        case NXT_IR_SEEKER_AC:
            devCon.Connection[sensorPort] = CONN_NXT_IIC;
            devCon.Type[sensorPort] = IIC_TYPE;
            devCon.Mode[sensorPort] = IIC_BYTE_MODE;
            break;
        case NXT_TEMP_C:
            devCon.Connection[sensorPort] = CONN_NXT_IIC;
            devCon.Type[sensorPort] = NXT_TEMP_TYPE;
            devCon.Mode[sensorPort] = NXT_TEMP_C_MODE;
            break;
        case NXT_TEMP_F:
            devCon.Connection[sensorPort] = CONN_NXT_IIC;
            devCon.Type[sensorPort] = NXT_TEMP_TYPE;
            devCon.Mode[sensorPort] = NXT_TEMP_F_MODE;
            break;
        case NXT_SOUND_DB:
            devCon.Connection[sensorPort] = CONN_NXT_DUMB;
            devCon.Type[sensorPort] = NXT_SOUND_TYPE;
            devCon.Mode[sensorPort] = NXT_SOUND_DB_MODE;
            break;
        case NXT_SOUND_DBA:
            devCon.Connection[sensorPort] = CONN_NXT_DUMB;
            devCon.Type[sensorPort] = NXT_SOUND_TYPE;
            devCon.Mode[sensorPort] = NXT_SOUND_DBA_MODE;
            break;
        case NXT_COMPASS_COMPASS:
        case NXT_COMPASS_ANGLE:
            devCon.Connection[sensorPort] = CONN_NXT_IIC;
            devCon.Type[sensorPort] = IIC_TYPE;
            devCon.Mode[sensorPort] = IIC_BYTE_MODE;
            break;
        default:
            return -1;
    }
    return 0;
}

extern int g_iicFile;


/**
 * Writes to the IIC device connected to the sensor port.
 * If repeatTimes is zero, the message will be sent for ever until a new call this function is made.
 * When reèeatTimes is 1, the message will be sent only one time.
*/
void writeIicRequestUsingIoctl(int sensorPort, int address, DATA8 toWrite[], int toWriteLength, int repeatTimes, int repeatInterval,  int responseLength) {
	static IICDAT IicDat;
	IicDat.Port = sensorPort;
	IicDat.Time = repeatInterval;
	IicDat.Repeat = repeatTimes;
	IicDat.RdLng = -responseLength;

	// the first byte of data is the length of data to send
	IicDat.WrLng = toWriteLength + 1;
	IicDat.WrData[0] = address;
	
	int i;
	for (i = 0; i < toWriteLength; i++) {
		IicDat.WrData[i + 1] = toWrite[i];
	}

	IicDat.Result = -1; 
	while (IicDat.Result) {
		ioctl(g_iicFile, IIC_SETUP, &IicDat);
	}
}

void writeIicRequestToNxtIRToReadRegister(int sensorPort, int registerAddress) {
	DATA8 request[] = {registerAddress};
	Wait(100); // TODO: investigate. Without this it doesn't always work. Maybe we need to receive at least one value? 
	writeIicRequestUsingIoctl(sensorPort, NXT_IR_IIC_ADDRESS, request, 1, 0, 100, 1);
}

void setupNxtIRSensorIfNeeded (int sensorPort) {
	int sensorName = sensor_setup_NAME[sensorPort];
	if (sensorName == NXT_IR_SEEKER_DC) {
		writeIicRequestToNxtIRToReadRegister(sensorPort, NXT_IR_REGISTER_DC);
	} else if (sensorName == NXT_IR_SEEKER_AC) {
		writeIicRequestToNxtIRToReadRegister(sensorPort, NXT_IR_REGISTER_AC);
	}
}

void setupNxtIRSensorsIfNeeded () {
	int sensorPort;
	for(sensorPort = 0; sensorPort < 4; sensorPort++) {
	    setupNxtIRSensorIfNeeded(sensorPort);
	}
}

extern int g_uartFile;

void applySensorMode(){
	// Set actual device mode

/*
    g_uartSensors->Status[0]      &= ~UART_DATA_READY;


    UARTCTL uartCtrl;
    uartCtrl.Port = 0;
    uartCtrl.Mode = devCon.Mode[0];
    // write setup string to "UART Device Controller" driver
    ioctl(g_uartFile, UART_SET_CONN, &devCon);

    int status = wait_no_zero_status(0);
    if (status & UART_PORT_CHANGED){
        // Clear the port changed flag for the current port.
        ioctl(g_uartFile, UART_CLEAR_CHANGED, &uartCtrl);
        g_uartSensors->Status[0] &= ~UART_PORT_CHANGED;
    }

*/

	ioctl(g_iicFile, IIC_SET_CONN, &devCon);
	setupNxtIRSensorsIfNeeded();
}

/**
 * When the ev3 app "Port view" starts, it shows the IR sensor values in DC mode by default.
 * If the user used this library and read the IR sensor in AC mode, the EV3 will show the AC values, saying
 * that they are DC values. We need to switch back to the DC mode if we are in AC mode while exiting the program.
 */
void exitNxtIRSensorIfNeeded (int sensorPort) {
	int sensorName = sensor_setup_NAME[sensorPort];
	if (sensorName == NXT_IR_SEEKER_AC) {
		writeIicRequestToNxtIRToReadRegister(sensorPort, NXT_IR_REGISTER_DC);
	}
}


void exitNxtIRSensorsIfNeeded(){
	int sensorPort;
	for(sensorPort = 0; sensorPort < 4; sensorPort++) {
	    exitNxtIRSensorIfNeeded(sensorPort);
	}
}

/********************************************************************************************/
/**
* Selection of the Beacon channel for IR Sensor
* author: Simón Rodriguez Perez
* note: channel can be modified while running
*
*/
int SetIRBeaconCH(int sensorPort, int channel)
{
	ir_sensor_channel[sensorPort] = channel;

	return 0;
}

int GetSensorName (int port) {
	return sensor_setup_NAME[port];
}


// TODO: Careful with mode and name
void SwitchSensorToModeIfNeeded (int port, int mode) {
    if (GetSensorName(port) != mode) {
        SetSensorMode(port, mode);
        Wait(5000);
    }
}