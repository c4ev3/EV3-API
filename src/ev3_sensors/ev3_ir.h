#ifndef EV3_API_EV3_IR_H
#define EV3_API_EV3_IR_H

#include "ev3_sensors.h"

#define EV3_IR_CHANNELS 4

#define EV3_IR_SENSOR_PROXIMITY_MODE    0
#define EV3_IR_SENSOR_SEEK_MODE         1
#define EV3_IR_SENSOR_REMOTE_MODE       2


#define BEACON_CH_1 0
#define BEACON_CH_2 1
#define BEACON_CH_3 2
#define BEACON_CH_4 3

// Infrared Beacon Buttons
#define BEACON_OFF 			    0
#define BEACON_UP_LEFT 		    1
#define BEACON_DOWN_LEFT 	    2
#define BEACON_UP_RIGHT 	    3
#define BEACON_DOWN_RIGHT 	    4
#define BEACON_UP			    5
#define BEACON_DIAG_UP_LEFT	    6
#define BEACON_DIAG_UP_RIGHT    7
#define BEACON_DOWN 		    8
#define BEACON_ON			    9
#define BEACON_LEFT 		    10
#define BEACON_RIGHT 		    11

extern SensorHandler * EV3Ir;

typedef struct EV3IrSeekResult {
    int directions[EV3_IR_CHANNELS];
    int distances[EV3_IR_CHANNELS];
} EV3IrSeekResult;

bool initEV3IrSensor (int port);

EV3IrSeekResult ReadEV3IrSensorSeek (int port);

int ReadEV3IrSensorProximity (int port);

int ReadEV3IrSensorRemote (int port, int channel);

void exitEV3IrSensor ();

/* Utility functions */
void setEV3IrSensorMode(int port, int mode);


#endif //EV3_API_EV3_IR_H
