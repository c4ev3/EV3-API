#ifndef EV3_API_EV3_IR_H
#define EV3_API_EV3_IR_H

#include "sensors.h"

#define EV3_IR_CHANNELS 4

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


#endif //EV3_API_EV3_IR_H
