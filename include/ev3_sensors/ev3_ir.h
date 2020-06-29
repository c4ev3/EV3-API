#ifndef EV3_API_EV3_IR_H
#define EV3_API_EV3_IR_H

#include "ev3_sensors.h"

#define EV3_IR_CHANNELS 4

#define EV3_IR_SENSOR_TYPE              33

#define EV3_IR_SENSOR_PROXIMITY_MODE    0
#define EV3_IR_SENSOR_SEEK_MODE         1
#define EV3_IR_SENSOR_REMOTE_MODE       2
#define EV3_IR_SENSOR_DEFAULT_MODE      EV3_IR_SENSOR_PROXIMITY_MODE


#define BEACON_CH_1 0
#define BEACON_CH_2 1
#define BEACON_CH_3 2
#define BEACON_CH_4 3

// Infrared Beacon Buttons. Macros are kept for back-compatibility, and are now
// replaced with the EV3IrRemoteButton enum
#define BEACON_OFF              0
#define BEACON_UP_LEFT          1
#define BEACON_DOWN_LEFT        2
#define BEACON_UP_RIGHT         3
#define BEACON_DOWN_RIGHT       4
#define BEACON_UP               5
#define BEACON_DIAG_UP_LEFT     6
#define BEACON_DIAG_UP_RIGHT    7
#define BEACON_DOWN             8
#define BEACON_ON               9
#define BEACON_LEFT             10
#define BEACON_RIGHT            11

typedef enum EV3IrRemoteButton {
    EV3_IR_REMOTE_OFF,
    EV3_IR_REMOTE_UP_LEFT,
    EV3_IR_REMOTE_DOWN_LEFT,
    EV3_IR_REMOTE_UP_RIGHT,
    EV3_IR_REMOTE_DOWN_RIGHT,
    EV3_IR_REMOTE_UP,
    EV3_IR_REMOTE_DIAG_UP_LEFT,
    EV3_IR_REMOTE_DIAG_UP_RIGHT,
    EV3_IR_REMOTE_DOWN,
    EV3_IR_REMOTE_ON,
    EV3_IR_REMOTE_LEFT,
    EV3_IR_REMOTE_RIGHT
} EV3IrRemoteButton;

extern SensorHandler * EV3Ir;

typedef struct EV3IrSeekResult {
    int directions[EV3_IR_CHANNELS];
    int distances[EV3_IR_CHANNELS];
} EV3IrSeekResult;

/**
 * @param port port to which the sensor is connected
 * @param result struct to which the result will be written  (output variable)
 * @return -1 if there was an error while reading the sensor
 */
int ReadEV3IrSensorSeek (int port, EV3IrSeekResult* result);

/**
 * Read the distance measured by the IR sensor in proximity mode
 * @param port port to which the sensor is connected
 * @return distance
 */
int ReadEV3IrSensorProximity (int port);

/**
 * Reads which button on the EV3 IR Remote is pressed
 * @param port port to which the sensor is connected
 * @param channel IR channel to read
 * @return enum value of the pressed button
 */
EV3IrRemoteButton ReadEV3IrSensorRemote (int port, int channel);


#endif //EV3_API_EV3_IR_H
