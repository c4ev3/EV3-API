#include "../ev3_inputs/ev3_input_iic.h"
#include "../ev3_wait.h"
#include "../ev3_sensor.h"
#include "ht_ir.h"

#define HT_IR_SENSOR_IIC_ADDRESS 0x08

#define HT_IR_SENSOR_DC_MODE_REGISTER 0x42
#define HT_IR_SENSOR_AC_MODE_REGISTER 0x49

static bool htIrInitialized[NUM_INPUTS] = {false, false, false, false};
static HTIrSensorMode htIrCurrentModes[NUM_INPUTS];

SensorHandler * HTIr = &(SensorHandler){
    .Init = initHTIrSensor,
    .Exit = exitHTIrSensor,
};

bool initHTIrSensor(int port) {
    initIICPort(port);
    htIrInitialized[port] = true;
    switchHTIrSensorMode(port, Modulated);
    return true;
}


// TODO: Untested modulated mode
int ReadHTIrSensor(int port, HTIrSensorMode mode) {
    switchHTIrSensorModeIfNeeded(port, mode);
    DATA8 data;
    readFromIIC(port, &data, 1);
    return ((uint8_t)data) & 0x0Fu;
}

void exitHTIrSensor(int port) {
    /**
     * When the ev3 app "Port view" starts, it shows the IR sensor values in DC mode by default.
     * If the user used this library and read the IR sensor in AC mode, the EV3 will show the AC values, saying that
     * they are DC values. This is why we need to switch back to the DC mode if we are in AC mode
    */
    int i;
    for (i = 0; i < NUM_INPUTS; i++) {
        switchHTIrSensorModeIfNeeded(i, Unmodulated);
    }
}

void switchHTIrSensorModeIfNeeded(int port, HTIrSensorMode mode) {
    if (htIrInitialized[port] && htIrCurrentModes[port] != mode) {
        switchHTIrSensorMode(port, mode);
        htIrCurrentModes[port] = mode;
    }
}

void switchHTIrSensorMode(int port, HTIrSensorMode mode) {
    int sensorRegister = mode == Modulated ? HT_IR_SENSOR_AC_MODE_REGISTER : HT_IR_SENSOR_DC_MODE_REGISTER;
    writeIicRequestToHTIrToReadRegister(port, sensorRegister);
}

void writeIicRequestToHTIrToReadRegister(int sensorPort, int registerAddress) {
    DATA8 request[] = {registerAddress};
    Wait(100); // TODO: investigate. Without this it doesn't always work. Maybe we need to receive at least one value?
    writeIicRequestUsingIoctl(sensorPort, HT_IR_SENSOR_IIC_ADDRESS, request, 1, 0, 100, 1);
}
