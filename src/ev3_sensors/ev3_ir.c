#include "../ev3_sensor.h"
#include "ev3_ir.h"


EV3IrSeekResult ReadEV3IrSensorSeek (int port) {
    SwitchSensorToModeIfNeeded(port, IR_SEEK);


    /**
     * The first byte of data contains the bearing, the second the distance.
     * This pattern repeats 4 times, two bytes for every channel.
     * When no beacon is found for a channel the distance is set to -128.
     */
    DATA8 data[8];
    readFromUART(port, data, 8);

    EV3IrSeekResult result;
    int i;
    for (i = 0; i < EV3_IR_CHANNELS; i++) {
        int direction = data[i * 2];
        int distance =  data[(i * 2) + 1];
        if (distance == -128) {
            result.directions[i] = 0;
            result.distances[i] = 128;
        } else {
            result.directions[i] = direction;
            result.distances[i] = distance;
        }
    }
    return result;
}

int ReadEV3IrSensorProximity (int port) {
    SwitchSensorToModeIfNeeded(port, IR_PROX);

    DATA8 data;
    readFromUART(port, &data, 1);
    return data;
}

/*
		case IR_REMOTE:
			temp = *(data)&0xFFFFFFFF;
			temp = (temp >> (8*ir_sensor_channel[sensorPort]))& 0xFF;
			return temp;
 */