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
 */
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "ev3sensor.h"
#include "uart.h"
#include "iic.h"
#include "analog.h"

#include <stdint.h>

int g_uartFile = 0;
int g_iicFile = 0;
int g_analogFile = 0;
UART* g_uartSensors = 0;
IIC* g_iicSensors = 0;
ANALOG* g_analogSensors = 0;

// Mode of inputs
int sensor_setup_NAME[INPUTS];
DATA8 sensor_setup_CONN[INPUTS];
DATA8 sensor_setup_TYPE[INPUTS];
DATA8 sensor_setup_MODE[INPUTS];

/********************************************************************************************/
/**
* Initialisation of the Sensorfunctions
* modified by: Simón Rodriguez Perez
* 		 date: 2015-02-28
*
*/
int initSensors()
{
    g_uartFile = open("/dev/lms_uart", O_RDWR | O_SYNC);
	g_iicFile =  open("/dev/lms_iic", O_RDWR | O_SYNC);
	g_analogFile = open("/dev/lms_analog", O_RDWR | O_SYNC);

    g_uartSensors = (UART*)mmap(0, sizeof(UART), PROT_READ | PROT_WRITE,
                                MAP_FILE | MAP_SHARED, g_uartFile, 0);
    g_iicSensors = (IIC*)mmap(0, sizeof(UART), PROT_READ | PROT_WRITE,
                              MAP_FILE | MAP_SHARED, g_iicFile, 0);
	g_analogSensors = (ANALOG*)mmap(0, sizeof(ANALOG), PROT_READ | PROT_WRITE,
									MAP_FILE | MAP_SHARED, g_analogFile, 0);
	int i;
	for (i = 0; i < INPUTS; i++)
	{
		sensor_setup_CONN[i] = 0;
		sensor_setup_TYPE[i] = 0;
		sensor_setup_MODE[i] = 0;
	}

	if (g_uartFile && g_iicFile && g_analogFile &&
		g_uartSensors && g_iicSensors && g_analogSensors)
		return 0;
	return -1;
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
* 		 note: Uartport
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
    UWORD currentSensorSlot = g_iicSensors->Actual[sensorPort];
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

/********************************************************************************************/
/**
* Getting the data for the Touchsensor 
* author: Simón Rodriguez Perez
* note: reading the data from a analog port
*
*/
void* readSensorTOUCH(int sensorPort)
{
	return (void*)&(g_analogSensors->InPin6[sensorPort]);
}

/********************************************************************************************/
/**
* Getting the data for the Sonar and Colorsensor 
* author: Simón Rodriguez Perez
* note: reading the data from a uart port
*
*/
void* readSensorUS(sensorPort)
{
	if (!g_uartSensors)
		return 0;
	return g_uartSensors->Raw[sensorPort][g_uartSensors->Actual[sensorPort]];

}

/********************************************************************************************/
/**
* Get the Data from the Sensor
* modified by: Simón Rodriguez Perez
* 		 date: 2015-02-28
* 		 note: changed for Touch-, Sonar- and Lightsensor
*
*/
void* readSensorData(int sensorPort)
{
	if (!g_analogSensors || sensorPort < 0 || sensorPort >= INPUTS)
		return 0;


	switch (sensor_setup_NAME[sensorPort])
	{
		case CONN_NONE: case CONN_ERROR: case NO_SEN: return 0;
		case TOUCH_PRESS:return readSensorTOUCH(sensorPort);
		case COL_REFLECT: return readSensorUS(sensorPort);
		case COL_AMBIENT: return readSensorUS(sensorPort);
		case COL_COLOR: return readSensorUS(sensorPort);
		case US_DIST_CM: return readSensorUS(sensorPort);
		case US_DIST_MM: return readSensorUS(sensorPort);
		case GYRO_ANG: return readSensorUS(sensorPort);
		default: return readOldDumbSensor(sensorPort);
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
*/
int readSensor(int sensorPort)
{
	uint64_t* data = readSensorData(sensorPort);
	uint16_t help=0;
	uint16_t help_1 = 1;
	uint16_t help_2 = 100;
	if (!data)
		return -1;

	switch (sensor_setup_NAME[sensorPort])
	{
		case NO_SEN:
			return -1;
		case TOUCH_PRESS:
			help = *data;
			help = help/256;
			if (help==0)
				return 0;
			else if(help==0xD)
				return 1;
			else
				return -1;
		case COL_REFLECT:
			return *((DATA16*)data)&0x00FF;
		case COL_AMBIENT:
			return *((DATA16*)data)&0x00FF;
		case COL_COLOR:
			return *((DATA16*)data)&0x000F;
		case US_DIST_CM:
			return (*((DATA16*)data)&0x0FFF)/10;
		case US_DIST_MM:
			return *((DATA16*)data)&0x0FFF;
		case GYRO_ANG:
		//help_1 = *(data)/0x80;
		//help_2 = *(data)/0x90;
		//help = (help_1*256) + (help_2/100);
			return -1;
		default: break;
	}
	return *((DATA16*)data);
}

/********************************************************************************************/
/**
* Initialisation for one Sensor 
* modified by: Simón Rodriguez Perez
* 		 date: 2015-02-28
* 		 note: Sensors are working now, but only one sensor is working at once
*
*/
int setSensorMode(int sensorPort, int name)
{
	static DEVCON devCon;

	if (!g_analogSensors || sensorPort < 0 || sensorPort >= INPUTS)
		return -1;

	sensor_setup_NAME[sensorPort] = name;
	// Setup of Input
	switch (name)
	{
		case TOUCH_PRESS:
			sensor_setup_CONN[sensorPort] = TOUCH_PRESS_CON;
			sensor_setup_TYPE[sensorPort] = TOUCH_PRESS_TYPE;
			sensor_setup_MODE[sensorPort] = TOUCH_PRESS_MODE;
			break;
		case COL_REFLECT:
			sensor_setup_CONN[sensorPort] = COL_REFLECT_CON;
			sensor_setup_TYPE[sensorPort] = COL_REFLECT_TYPE;
			sensor_setup_MODE[sensorPort] = COL_REFLECT_MODE;
			devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
			devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
			devCon.Mode[sensorPort] 		= 0;//sensor_setup_MODE[sensorPort];
			ioctl(g_uartFile, UART_SET_CONN, &devCon);
			break;
		case COL_AMBIENT:
			sensor_setup_CONN[sensorPort] = COL_AMBIENT_CON;
			sensor_setup_TYPE[sensorPort] = COL_AMBIENT_TYPE;
			sensor_setup_MODE[sensorPort] = COL_AMBIENT_MODE;
			devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
			devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
			devCon.Mode[sensorPort] 		= sensor_setup_MODE[sensorPort];
			ioctl(g_uartFile, UART_SET_CONN, &devCon);
			break;
		case COL_COLOR:
			sensor_setup_CONN[sensorPort] = COL_COLOR_CON;
			sensor_setup_TYPE[sensorPort] = COL_COLOR_TYPE;
			sensor_setup_MODE[sensorPort] = COL_COLOR_MODE;
			devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
			devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
			devCon.Mode[sensorPort] 		= sensor_setup_MODE[sensorPort];
			ioctl(g_uartFile, UART_SET_CONN, &devCon);
			break;
		case US_DIST_CM:
			sensor_setup_CONN[sensorPort] = US_DIST_CM_CON;
			sensor_setup_TYPE[sensorPort] = US_DIST_CM_TYPE;
			sensor_setup_MODE[sensorPort] = US_DIST_CM_MODE;
			devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
			devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
			devCon.Mode[sensorPort] 		= sensor_setup_MODE[sensorPort];
			ioctl(g_uartFile, UART_SET_CONN, &devCon);
			break;
		case US_DIST_MM:
			sensor_setup_CONN[sensorPort] = US_DIST_MM_CON;
			sensor_setup_TYPE[sensorPort] = US_DIST_MM_TYPE;
			sensor_setup_MODE[sensorPort] = US_DIST_MM_MODE;
			devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
			devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
			devCon.Mode[sensorPort] 		= sensor_setup_MODE[sensorPort];
			break;
		default: return -1;
	}

	return 0;
}

/********************************************************************************************/
/**
* Initialisation of all Sensors for working parallel 
* author: Simón Rodriguez Perez
* note: the function can only be called once at the beginning
*
*/
int setAllSensorMode(int name_1, int name_2, int name_3, int name_4)
{
	static DEVCON devCon;
	int sensorPort = 0;
	
	int name[4] = {};

	name[0] = name_1;
	name[1] = name_2;
	name[2] = name_3;
	name[3] = name_4;

	if (!g_analogSensors)
	{
		return -1;
	}
	
	// Setup of Input
	for(sensorPort=0; sensorPort<4; sensorPort++)
	{
		sensor_setup_NAME[sensorPort] = name[sensorPort];
		switch (name[sensorPort])
		{
			case NO_SEN:
				break;
			case TOUCH_PRESS:
				sensor_setup_CONN[sensorPort] = TOUCH_PRESS_CON;
				sensor_setup_TYPE[sensorPort] = TOUCH_PRESS_TYPE;
				sensor_setup_MODE[sensorPort] = TOUCH_PRESS_MODE;
				break;
			case COL_REFLECT:
				sensor_setup_CONN[sensorPort] = COL_REFLECT_CON;
				sensor_setup_TYPE[sensorPort] = COL_REFLECT_TYPE;
				sensor_setup_MODE[sensorPort] = COL_REFLECT_MODE;
				devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
				devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
				devCon.Mode[sensorPort] 		= 0;//sensor_setup_MODE[sensorPort];
				
				break;
			case COL_AMBIENT:
				sensor_setup_CONN[sensorPort] = COL_AMBIENT_CON;
				sensor_setup_TYPE[sensorPort] = COL_AMBIENT_TYPE;
				sensor_setup_MODE[sensorPort] = COL_AMBIENT_MODE;
				devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
				devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
				devCon.Mode[sensorPort] 		= sensor_setup_MODE[sensorPort];
				break;
			case COL_COLOR:
				sensor_setup_CONN[sensorPort] = COL_COLOR_CON;
				sensor_setup_TYPE[sensorPort] = COL_COLOR_TYPE;
				sensor_setup_MODE[sensorPort] = COL_COLOR_MODE;
				devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
				devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
				devCon.Mode[sensorPort] 		= sensor_setup_MODE[sensorPort];
				break;
			case US_DIST_CM:
				sensor_setup_CONN[sensorPort] = US_DIST_CM_CON;
				sensor_setup_TYPE[sensorPort] = US_DIST_CM_TYPE;
				sensor_setup_MODE[sensorPort] = US_DIST_CM_MODE;
				devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
				devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
				devCon.Mode[sensorPort] 		= sensor_setup_MODE[sensorPort];
				break;
			case US_DIST_MM:
				sensor_setup_CONN[sensorPort] = US_DIST_MM_CON;
				sensor_setup_TYPE[sensorPort] = US_DIST_MM_TYPE;
				sensor_setup_MODE[sensorPort] = US_DIST_MM_MODE;
				devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
				devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
				devCon.Mode[sensorPort] 		= sensor_setup_MODE[sensorPort];
				break;
			case GYRO_ANG:
				sensor_setup_CONN[sensorPort] = GYRO_ANG_CON;
				sensor_setup_TYPE[sensorPort] = GYRO_ANG_TYPE;
				sensor_setup_MODE[sensorPort] = GYRO_ANG_MODE;
				devCon.Connection[sensorPort] 	= sensor_setup_CONN[sensorPort];
				devCon.Type[sensorPort] 		= sensor_setup_TYPE[sensorPort];
				devCon.Mode[sensorPort] 		= sensor_setup_MODE[sensorPort];
				break;
			default: return -1;
		}
	}
	// Set actual device mode
	ioctl(g_uartFile, UART_SET_CONN, &devCon);
	//ioctl(g_iicFile, IIC_SET_CONN, &devCon);
	return 0;
}
