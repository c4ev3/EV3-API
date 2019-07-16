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
#include "ev3_analog.h"
#include "ev3_iic.h"
#include "ev3_uart.h"

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

// Light
#define COL_TYPE 29
#define COL_REFLECT_MODE 0 	// Reflect
#define COL_AMBIENT_MODE 1 	// Ambient
#define COL_COLOR_MODE 2 	// Color
#define COL_RGB_MODE 4

// Ultrasonic
#define US_TYPE 30
#define US_DIST_CM_MODE 0 	// Dist in cm
#define US_DIST_MM_MODE 0 	// Dist in mm
#define US_DIST_IN_MODE 1 	// Dist in inch
#define US_LISTEN_MODE  2   // Presence of other ultrasonic sensors

// Gyroskop
#define GYRO_TYPE 				32
#define GYRO_ANG_MODE 			0 // angle
#define GYRO_RATE_MODE 			1 // rate
#define GYRO_ANG_AND_RATE_MODE 	3 // both

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

/***********************************/

int g_uartFile = 0;
int g_iicFile = 0;
int g_analogFile = 0;
UART* g_uartSensors = 0;
IIC* g_iicSensors = 0;
ANALOG* g_analogSensors = 0;

// Mode of inputs
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
	if (SensorsInitialized())
		return 0;
    
	g_uartFile = open("/dev/lms_uart", O_RDWR | O_SYNC);
	g_iicFile =  open("/dev/lms_iic", O_RDWR | O_SYNC);
	g_analogFile = open("/dev/lms_analog", O_RDWR | O_SYNC);

	g_uartSensors = (UART*)mmap(0, sizeof(UART), PROT_READ | PROT_WRITE,
								MAP_FILE | MAP_SHARED, g_uartFile, 0);
	g_iicSensors = (IIC*)mmap(0, sizeof(IIC), PROT_READ | PROT_WRITE,
							  MAP_FILE | MAP_SHARED, g_iicFile, 0);
	g_analogSensors = (ANALOG*)mmap(0, sizeof(ANALOG), PROT_READ | PROT_WRITE,
									MAP_FILE | MAP_SHARED, g_analogFile, 0);
	int i;
	for (i = 0; i < INPUTS; i++)
	{
		ir_sensor_channel[i] = 0;
	}

	if (g_uartFile && g_iicFile && g_analogFile &&
		g_uartSensors && g_iicSensors && g_analogSensors)
		return 0;
	return -1;
}

bool SensorsInitialized()
{
	return g_uartFile && g_iicFile && g_analogFile &&
		g_uartSensors && g_iicSensors && g_analogSensors;
}

void exitNxtIRSensorsIfNeeded();

bool SensorsExit()
{
	if (!SensorsInitialized()) {
        return false;
	}

	exitNxtIRSensorsIfNeeded();
	/// TODO: Exit also the compass

	munmap(g_uartSensors, sizeof(UART));
	munmap(g_iicSensors, sizeof(IIC));
	munmap(g_analogSensors, sizeof(ANALOG));

	close(g_uartFile);
	close(g_iicFile);
	close(g_analogFile);

	g_uartFile = g_iicFile = g_analogFile = 0;
	g_uartSensors = NULL;
	g_iicSensors = NULL;
	g_analogSensors = NULL;

	return true;
}


/*DATA8 getSensorConnectionType(int sensorPort)
{
	if (!g_analogSensors || sensorPort < 0 || sensorPort >= INPUTS)
		return 0;
	return g_analogSensors->InConn[sensorPort];
}*/

/********************************************************************************************/
/**
* Getting the Data from a Uartport
* modified by: Simón Rodriguez Perez
* 		 date: 2015-02-28
* 		 note: Readout of Uart-Port
*
*/
void* readUartSensor(int sensorPort)
{
	if (!g_uartSensors)
		return 0;
	return g_uartSensors->Raw[sensorPort][g_uartSensors->Actual[sensorPort]];
}

void* readIicSensor(int sensorPort)
{
	if (!g_iicSensors)
		return 0;
	uint16_t currentSensorSlot = g_iicSensors->Actual[sensorPort];
	return g_iicSensors->Raw[sensorPort][currentSensorSlot];
}

void* readNewDumbSensor(int sensorPort)
{
	return (void*)&(g_analogSensors->InPin6[sensorPort]);
}

void* readOldDumbSensor(int sensorPort)
{
	return (void*)&(g_analogSensors->InPin1[sensorPort]);
}

void* readNxtColor(int sensorPort, DATA8 index)
{
	return 0; // Not supported
/*
	DATAF result = DATAF_NAN;
	cInputCalibrateColor(g_analogSensors->NxtCol[sensorPort], g_analogSensors->NxtCol[sensorPort].SensorRaw);

	switch (g_sensorMode[sensorPort])
	{
	case 2: return cInputCalculateColor(g_analogSensors->NxtCol[sensorPort]); //NXT-COL-COL
	case 1: return g_analogSensors->NxtCol[sensorPort].ADRaw[BLANK]; // NXT-COL-AMB
	case 0: return g_analogSensors->NxtCol[sensorPort].ADRaw[RED]; // NXT-COL-RED
	case 3: return g_analogSensors->NxtCol[sensorPort].ADRaw[GREEN]; // NXT-COL-GRN
	case 4: return g_analogSensors->NxtCol[sensorPort].ADRaw[BLUE]; // NXT-COL-BLU
	case 5: return g_analogSensors->NxtCol[sensorPort].SensorRaw[Index]; // NXT-COL-RAW
	}
	return result;
*/
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



/********************************************************************************************/
/**
* Get the Data from the Sensor
* modified by: Simón Rodriguez Perez
* 		 date: 2015-02-28
* 		 note: changed for Touch-, Sonar- and Lightsensor
*
*----------------------------------------------------------------------------------
*
* modified by: Simón Rodriguez Perez
* 		 date: 2016-04-21
* 		 note: readout for Gyro and Infrared Sensor
*
*/
void* ReadSensorData(int sensorPort)
{
	if (!g_analogSensors || sensorPort < 0 || sensorPort >= INPUTS)
		return 0;

	switch (sensor_setup_NAME[sensorPort])
	{
		case CONN_NONE:
		case CONN_ERROR:
		case NO_SEN:
			return 0;
			// Touchsensor
		case TOUCH_PRESS:
			return readNewDumbSensor(sensorPort);
			// Lightsensor
		case COL_REFLECT:
		case COL_AMBIENT:
		case COL_COLOR:
		case COL_RGB:
			return readUartSensor(sensorPort);
			// Ultrasonic
		case US_DIST_CM:
		case US_DIST_MM:
        case US_DIST_IN:
        case US_LISTEN:
			return readUartSensor(sensorPort);
			// Gyroskop
		case GYRO_ANG:
        case GYRO_RATE:
			return readUartSensor(sensorPort);
			// Infrared
		case IR_PROX:
		case IR_SEEK:
		case IR_REMOTE:
			return readUartSensor(sensorPort);
			// NXT
		case NXT_IR_SEEKER_DC:
		case NXT_IR_SEEKER_AC:
        case NXT_TEMP_C:
		case NXT_TEMP_F:
		case NXT_COMPASS_COMPASS:
		case NXT_COMPASS_ANGLE:
			return readIicSensor(sensorPort);
		case NXT_SOUND_DB:
		case NXT_SOUND_DBA:
			return readOldDumbSensor(sensorPort);
		default: return 0;
	}

	return 0;
}

/********************************************************************************************/
/**
* Usercall for actual value of one Sensor
* modified by: Simón Rodriguez Perez
* 		 date: 2015-02-28
* 		 note: now working for Touch-, Sonar- and Lightsensor
*			   with calculation of the correct values
*
*----------------------------------------------------------------------------------
*
* modified by: Simón Rodriguez Perez
* 		 date: 2016-04-21
* 		 note: readout for Gyroscop and Infrared Sensor
*
*/
int ReadSensor(int sensorPort)
{
	uint64_t* data = (uint64_t*) ReadSensorData(sensorPort);
	int32_t temp = 0;
	if (!data) {
		return -1;
	}

	switch (sensor_setup_NAME[sensorPort])
	{
		case NO_SEN:
			return -1;
			// Touchsensor
		case TOUCH_PRESS:
			temp = *((DATA16*)data);
			temp = temp/256;
			if (temp==0)
				return 0;
			else if(temp==0xD)
				return 1;
			else
				return -1;
			// Lightsensor
		case COL_REFLECT:
			return *((DATA16*)data)&0x00FF;
		case COL_AMBIENT:
			return *((DATA16*)data)&0x00FF;
		case COL_COLOR:
			return *((DATA16*)data)&0x000F;
		case COL_RGB:
			/**
			 * The first 6 bytes in data are the colors: 2 byte for each color.
			 * The range of each color value is from 0 to 1023. We convert those
			 * values in 3 bytes (0-255), to be able to return it as a int
			 */
			temp = 0;
			int r = (int) ((((*data) & 0xFFFF) / 1024.0) * 255.0);
			int g = (int) (((((*data) >> 16) & 0xFFFF) / 1024.0) * 255.0);
			int b = (int) (((((*data) >> 32) & 0xFFFF) / 1024.0) * 255.0);
			return r | (g << 8) | (b << 16);
			// Ultrasonic
		case US_DIST_CM:
			return (*((DATA16*)data)&0x0FFF)/10;
		case US_DIST_MM:
			return *((DATA16*)data)&0x0FFF;
		case US_DIST_IN:
			return *((DATA16*)data)&0x0FFF;
		case US_LISTEN:
			return *((DATA16*)data)&0x0FFF;
			// Gyroskop
			// The first 16bits are the angle ...
		case GYRO_ANG:
			temp = *(data)&0xFFFF;

			if(temp & 0x8000) // handles negative values
			{
				temp = ((temp&0x7FFF) - 0x7FFF);
			}
			return temp;
			// ... the next 16bits are the rate
		case GYRO_RATE:
			temp = ((*(data))>>16)&0xFFFF;
			if(temp & 0x8000)  // handles negative values
			{
				temp = ((temp&0x7FFF) - 0x7FFF);
			}
			return temp;
			// Infrared
		case IR_PROX:
			return *((DATA16*)data)&0x00FF;
		case IR_SEEK:
			temp = (*(data) >> (16*ir_sensor_channel[sensorPort]))& 0xFF;
			if(temp & 0x80)
			{
				temp = ((temp&0x7F) - 0x7F);
			}
			return temp;
		case IR_REMOTE:
			temp = *(data)&0xFFFFFFFF;
			temp = (temp >> (8*ir_sensor_channel[sensorPort]))& 0xFF;
			return temp;
			// NXT
		case NXT_IR_SEEKER_DC:
		case NXT_IR_SEEKER_AC:
			return *((DATA16*)data)&0x000F;
		case NXT_TEMP_C:
			temp = (*data>>4) & 0x0FFF;
			if(temp & 0x800)
			{
				temp = ((temp&0x7FF) ^ 0x7FF) + 1;
				return (-1)*(((temp>>4) & 0xFF)*10 + ((temp & 0xF) * 10 / 15));
			}
			return ((temp>>4) & 0xFF)*10 + ((temp & 0xF) * 10 / 15);
		case NXT_TEMP_F:
			temp = (*data>>4) & 0x0FFF;
			if(temp & 0x800)
			{
				temp = ((temp&0x7FF) ^ 0x7FF) + 1;
				return (-1)*(((temp>>4) & 0xFF)*10 + ((temp & 0xF) * 10 / 15)) * 9/5 + 320;
			}
			return (((temp>>4) & 0xFF)*10 + ((temp & 0xF) * 10 / 15)) * 9/5 + 320;
		case NXT_SOUND_DB:
		case NXT_SOUND_DBA:
			temp = *((DATA16*)data);
			return (int)((1.0 - (temp/4095.0)) * 100.0); // ADC_RES = 4095

		case NXT_COMPASS_COMPASS:
			return ((*data & 0xFF) << 1);
		case NXT_COMPASS_ANGLE:
			temp = ((*data & 0xFF) << 1);
			return  temp < 180 ? (-temp) : (360 - temp);
		default: break;
	}
	return *((DATA16*)data);
}


DEVCON devCon;

int setSensorMode(int sensorPort, int name);
void applySensorMode();

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
	if (!g_analogSensors)
		InitSensors();

	if (sensorPort < 0 || sensorPort >= INPUTS)
		return -1;

	int res = setSensorMode(sensorPort, modeName);
	if (res == -1) {
	    return res;
	}
	applySensorMode();
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

	if (!g_analogSensors)
		InitSensors();

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
    switch (name) {
        case NO_SEN:
            break;
            // Touchsensor
        case TOUCH_PRESS:
            devCon.Connection[sensorPort] = CONN_INPUT_DUMB;
            devCon.Type[sensorPort] = TOUCH_TYPE;
            devCon.Mode[sensorPort] = TOUCH_PRESS_MODE;
            break;
            // Lightsensor
        case COL_REFLECT:
            devCon.Connection[sensorPort] = CONN_INPUT_UART;
            devCon.Type[sensorPort] = COL_TYPE;
            devCon.Mode[sensorPort] = COL_REFLECT_MODE;
            break;
        case COL_AMBIENT:
            devCon.Connection[sensorPort] = CONN_INPUT_UART;
            devCon.Type[sensorPort] = COL_TYPE;
            devCon.Mode[sensorPort] = COL_AMBIENT_MODE;
            break;
		case COL_COLOR:
			devCon.Connection[sensorPort] = CONN_INPUT_UART;
			devCon.Type[sensorPort] = COL_TYPE;
			devCon.Mode[sensorPort] = COL_COLOR_MODE;
			break;
		case COL_RGB:
			devCon.Connection[sensorPort] = CONN_INPUT_UART;
			devCon.Type[sensorPort] = COL_TYPE;
			devCon.Mode[sensorPort] = COL_RGB_MODE;
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
            // Gyroskop
        case GYRO_ANG:
        case GYRO_RATE:
            devCon.Connection[sensorPort] = CONN_INPUT_UART;
            devCon.Type[sensorPort] = GYRO_TYPE;
            devCon.Mode[sensorPort] = GYRO_ANG_AND_RATE_MODE;
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

void applySensorMode(){
	// Set actual device mode
	ioctl(g_uartFile, UART_SET_CONN, &devCon);
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

/**
 * Switches from the mode to read both values to the mode to read only one value,
 * so the sensor resets it self.
 * NOTE: The sensor should not move while switching mode
 */
void ResetGyroSensor(int port) {
    devCon.Connection[port] = CONN_INPUT_UART;
    devCon.Type[port] = GYRO_TYPE;
    devCon.Mode[port] = GYRO_RATE_MODE;
    applySensorMode();
    Wait(200);

    devCon.Connection[port] = CONN_INPUT_UART;
    devCon.Type[port] = GYRO_TYPE;
    devCon.Mode[port] = GYRO_ANG_AND_RATE_MODE;
    applySensorMode();
    Wait(200);
}


int GetRFromRGB(int rgb) {
	return rgb & 0x00FF;
}

int GetGFromRGB(int rgb) {
	return (rgb >> 8 ) & 0x00FF;
}

int GetBFromRGB(int rgb) {
	return (rgb >> 16 ) & 0x00FF;
}

int invertSignIfOverHalfByte (int n) {
    if(n & 0x80) {
        return ((n & 0x7F) - 0x7F);
    }
    return n;
}

int * ReadIRSeekAllChannels(int port) {
	if(GetSensorName(port) != IR_SEEK) {
		return NULL;
	}
	uint64_t data = *((uint64_t*)ReadSensorData(port));
	static int results[IR_CHANNELS * 2];
	int i;
	for (i = 0; i < IR_CHANNELS; i++) {
		int channelData = (int) (data >> (i * 16));
		int measurement = channelData & 0xFF;
		int raw =  (channelData >> 8) & 0xFF;
		// TODO: Fix, sometimes it goes to -inf
		results[i * 2] = invertSignIfOverHalfByte(measurement);
		results[(i * 2) + 1] = raw;
	}
	return results;
}

void StartHTCompassCalibration(int sensorPort) {
	DATA8 request[] = {0x41, 0x43};
	writeIicRequestUsingIoctl(sensorPort, NXT_COMPASS_IIC_ADDRESS, request, 2, 1, 0, 1);
}

void StopHTCompassCalibration(int sensorPort) {
	DATA8 request[] = {0x41, 0x00};
	writeIicRequestUsingIoctl(sensorPort, NXT_COMPASS_IIC_ADDRESS, request, 2, 0, 100, 1);
}
