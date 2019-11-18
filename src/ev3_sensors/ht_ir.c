#include "../ev3_inputs/ev3_input_iic.h"
#include "../../include/ev3_wait.h"
#include "ht_ir.h"

#define HT_IR_SENSOR_IIC_ADDRESS 0x08

#define HT_IR_SENSOR_DC_READING_MODE_REGISTER 0x42
#define HT_IR_SENSOR_AC_READING_MODE_REGISTER 0x49

SensorHandler * HTIr = &(SensorHandler){
    .Init = initHTIrSensor,
    .Exit = exitHTIrSensor,
    .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

bool initHTIrSensor(int port) {
    initIICPort(port, HT_IR_SENSOR_IIC_ADDRESS);
    HTIr->currentSensorMode[port] = HT_IR_SENSOR_DEFAULT_MODE;
    return true;
}


// TODO: Untested modulated mode
int ReadHTIrSensor(int port, HTIrReadingMode mode) {
    DATA8 data;
    HTIr->currentSensorMode[port] = mode;
    readFromIIC(port, getHTIrRegisterForReadingMode(mode), &data, 1);
    return ((uint8_t)data) & 0x0Fu;
}

int getHTIrRegisterForReadingMode(HTIrReadingMode mode) {
    return mode == Modulated ? HT_IR_SENSOR_AC_READING_MODE_REGISTER : HT_IR_SENSOR_DC_READING_MODE_REGISTER;
}


void exitHTIrSensor(int port) {
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

