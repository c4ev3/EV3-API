#include "../ev3_sensor.h"
#include "../../copied/lms2012/ev3_uart.h"
#include "nxt_temperature.h"

// TODO: Not tested yet
int ReadNXTTemperatureSensor(int port, NXTTemperatureUnit unit) {
    if (GetSensorName(port) != NXT_IR_SEEKER_AC) {
        SetSensorMode(port, NXT_IR_SEEKER_AC);
    }

    DATA16 data;
    readFromIIC(port, &data, 2);
    int temp = (data>>4) & 0x0FFF;
    // TODO: Clean
    if (unit == C) {
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
