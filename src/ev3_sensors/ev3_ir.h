#ifndef EV3_API_EV3_IR_H
#define EV3_API_EV3_IR_H

#include "ev3_sensors.h"

#define EV3_IR_CHANNELS 4

#define EV3_IR_SENSOR_PROXIMITY_MODE    0
#define EV3_IR_SENSOR_SEEK_MODE         1
#define EV3_IR_SENSOR_REMOTE_MODE       2

extern SensorHandler * EV3Ir;

typedef struct EV3IrSeekResult {
    int directions[EV3_IR_CHANNELS];
    int distances[EV3_IR_CHANNELS];
} EV3IrSeekResult;

bool initEV3IrSensor (int port);


EV3IrSeekResult ReadEV3IrSensorSeek (int port);

int ReadEV3IrSensorProximity (int port);

// TODO: Implement remote mode
//int ReadEV3IrSensorRemote (int port);

void exitEV3IrSensor ();

/* Utility functions */
void setEV3IrSensorMode(int port, int mode);


#endif //EV3_API_EV3_IR_H
