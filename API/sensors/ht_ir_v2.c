#include "inputs/ev3_input_iic.h"
#include "ev3_sensors.h"
#include "ev3_command.h"
#include "ht_ir_v2.h"

#define HT_IR_V2_SENSOR_IIC_ADDRESS 0x08

#define HT_IR_V2_SENSOR_DC_READING_MODE_REGISTER 0x42
#define HT_IR_V2_SENSOR_AC_READING_MODE_REGISTER 0x49

SensorHandler * HTIrV2 = &(SensorHandler) {
        .Init = initHTIrV2Sensor,
        .Exit = exitHTIrV2Sensor,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

bool initHTIrV2Sensor(int port) {
    initIICPort(port, HT_IR_V2_SENSOR_IIC_ADDRESS);
    HTIrV2->currentSensorMode[port] = HT_IR_V2_SENSOR_DEFAULT_MODE;
    return true;
}


int ReadHTIrV2Sensor(int port, HTIrV2ReadingMode mode) {
    DATA8 data;
    if(readFromIIC(port, getHTIrV2RegisterForReadingMode(mode), &data, 1) == -1) {
        return -1;
    }
    return ((uint8_t) data) & 0x0Fu;
}

int getHTIrV2RegisterForReadingMode(HTIrV2ReadingMode mode) {
    return mode == Modulated ? HT_IR_V2_SENSOR_AC_READING_MODE_REGISTER : HT_IR_V2_SENSOR_DC_READING_MODE_REGISTER;
}


void exitHTIrV2Sensor(int port) {
    /**
     * When the ev3 app "Port view" starts, it shows the IR sensor values in DC mode by default.
     * If the user used this library and read the IR sensor in AC mode, the EV3 will show the AC values, saying that
     * they are DC values. This is why we need to switch back to the DC mode if we are in AC mode
    */
    /*
     * // TODO: NOT Working
    int i;
    for (i = 0; i < NUM_INPUTS; i++) {
        if (htIrInitialized[port]) {
            startPollingFromIIC(port, HT_IR_SENSOR_DC_READING_MODE_REGISTER, 100);
        }
    }*/
}

