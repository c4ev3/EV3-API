#include "inputs/ev3_input_uart.h"
#include "ev3_command.h"
#include "ev3_sensors/ev3_ir.h"
#include "utility.h"

//
// PRIVATE DECLARATIONS
//

static  bool initEV3IrSensor (int port);

static void setEV3IrSensorMode(int port, int mode);

//
// GLOBAL DATA
//

SensorHandler * EV3Ir = &(SensorHandler){
        .Init = initEV3IrSensor,
        .Exit = NULL,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

//
// IMPLEMENTATION
//

bool initEV3IrSensor (int port) {
    setEV3IrSensorMode(port, EV3_IR_SENSOR_DEFAULT_MODE);
    return true;
}


int ReadEV3IrSensorSeek (int port, EV3IrSeekResult* result) {
    setEV3IrSensorMode(port, EV3_IR_SENSOR_SEEK_MODE);

    /**
     * The first byte of data contains the bearing, the second the distance.
     * This pattern repeats 4 times, two bytes for every channel.
     * When no beacon is found for a channel the distance is set to -128.
     */
    DATA8 data[8];
    readFromUART(port, data, sizeof(data) / sizeof(DATA8));

    int i;
    for (i = 0; i < EV3_IR_CHANNELS; i++) {
        int direction = data[i * 2];
        int distance =  data[(i * 2) + 1];
        if (distance == -128) {
            result->directions[i] = 0;
            result->distances[i] = 128;
        } else {
            result->directions[i] = direction;
            result->distances[i] = distance;
        }
    }
    return 0;
}

int ReadEV3IrSensorProximity (int port) {
    setEV3IrSensorMode(port, EV3_IR_SENSOR_PROXIMITY_MODE);

    DATA8 data;
    readFromUART(port, &data, 1);
    return data;
}

void setEV3IrSensorMode(int port, int mode) {
    setUARTSensorHandlerMode(EV3Ir, port, EV3_IR_SENSOR_TYPE, mode);
}

EV3IrRemoteButton ReadEV3IrSensorRemote (int port, int channel) {
    setEV3IrSensorMode(port, EV3_IR_SENSOR_REMOTE_MODE);
    DATA8 data[EV3_IR_CHANNELS];
    readFromUART(port, data, EV3_IR_CHANNELS);
    return data[channel];
}
