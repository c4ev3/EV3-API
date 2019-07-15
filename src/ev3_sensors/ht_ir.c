#include "../ev3_sensor.h"
#include "ht_ir.h"

// TODO: Untested
int ReadHTIrSensorAC(int port) {
    if (GetSensorName(port) != NXT_IR_SEEKER_AC) {
        SetSensorMode(port, NXT_IR_SEEKER_AC);
    }

    DATA8 data;
    readFromIIC(port, &data, 1);
    return ((uint8_t)data) & 0x0Fu;
}

int ReadHTIrSensorDC(int port) {
    if (GetSensorName(port) != NXT_IR_SEEKER_DC) {
        SetSensorMode(port, NXT_IR_SEEKER_DC);
    }
    DATA8 data;
    readFromIIC(port, &data, 1);
    return ((uint8_t)data) & 0x0Fu;
}

