#include "inputs/ev3_input_iic.h"
#include "ev3_sensors.h"
#include "nxt_temperature.h"


SensorHandler * NXTTemperature = &(SensorHandler) {
    .Init = initNXTTemperatureSensor,
    .Exit = exitNXTTemperatureSensor,
    .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

bool initNXTTemperatureSensor (int port) {
    NXTSound->currentSensorMode[port] = NXT_TEMPERATURE_SENSOR_DEFAULT_MODE;
    return true;
}

// TODO: Not tested yet
int ReadNXTTemperatureSensor(int port, NXTTemperatureUnit unit) {
    DATA16 data;
    readFromIICSharedMemory(port, (DATA8 *) &data, 2);
    int temp = (data>>4) & 0x0FFF;
    if (unit == NXT_TEMPERATURE_C) {
        if(temp & 0x800) {
            temp = ((temp&0x7FF) ^ 0x7FF) + 1;
            return (-1)*(((temp>>4) & 0xFF)*10 + ((temp & 0xF) * 10 / 15));
        }
        return ((temp>>4) & 0xFF)*10 + ((temp & 0xF) * 10 / 15);
    } else {
        if(temp & 0x800) {
            temp = ((temp&0x7FF) ^ 0x7FF) + 1;
            return (-1)*(((temp>>4) & 0xFF)*10 + ((temp & 0xF) * 10 / 15)) * 9/5 + 320;
        }
        return (((temp>>4) & 0xFF)*10 + ((temp & 0xF) * 10 / 15)) * 9/5 + 320;
    }
}


void exitNXTTemperatureSensor (int port) {

}
