#include <stddef.h>
#include "ev3_sensors.h"
#include "back_compatibility.h"

static int sensorModeNames[NUM_INPUTS] = {NO_SEN};
static int ev3IrSensorChannel[NUM_INPUTS] = {0};
static SensorHandler * sensorModeHandlersByModeName[100];

int SetAllSensorMode(int name_1, int name_2, int name_3, int name_4) {
    sensorModeNames[IN_1] = name_1;
    sensorModeNames[IN_2] = name_2;
    sensorModeNames[IN_3] = name_3;
    sensorModeNames[IN_4] = name_4;
    initializeAllSensors();
    return 0;
}

void initializeAllSensors () {
    initializeBackCompatibilityIfNeeded();
    int i;
    for (i = 0; i < NUM_INPUTS; i++) {
        int modeName = sensorModeNames[i];
        if (modeName != NO_SEN) {
            SensorHandler * handler = sensorModeHandlersByModeName[modeName];
            handler->Init(i);
        }
    }
}


static bool backCompatibilityInitialized = false;

void initializeBackCompatibilityIfNeeded () {
    if (backCompatibilityInitialized) {
        return;
    }

    sensorModeHandlersByModeName[TOUCH_PRESS]   = EV3Touch;
    sensorModeHandlersByModeName[COL_REFLECT]   = EV3Color;
    sensorModeHandlersByModeName[COL_AMBIENT]   = EV3Color;
    sensorModeHandlersByModeName[COL_COLOR]     = EV3Color;
    sensorModeHandlersByModeName[US_DIST_CM]    = EV3Ultrasonic;
    sensorModeHandlersByModeName[US_DIST_MM]    = EV3Ultrasonic;
    sensorModeHandlersByModeName[US_DIST_IN]    = EV3Ultrasonic;
    sensorModeHandlersByModeName[GYRO_ANG]      = EV3Gyro;
    sensorModeHandlersByModeName[GYRO_RATE]     = EV3Gyro;
    sensorModeHandlersByModeName[IR_PROX]       = EV3Ir;
    sensorModeHandlersByModeName[IR_SEEK]       = EV3Ir;
    sensorModeHandlersByModeName[IR_REMOTE]     = EV3Ir;
    sensorModeHandlersByModeName[NXT_IR_SEEKER] = HTIr;
    sensorModeHandlersByModeName[NXT_TEMP_C]    = NXTTemperature;
    sensorModeHandlersByModeName[NXT_TEMP_F]    = NXTTemperature;

    backCompatibilityInitialized = true;
}

int ReadSensor(int port) {
    int modeName = sensorModeNames[port];
    switch (modeName) {
        case TOUCH_PRESS:
            return ReadEV3TouchSensor(port);
        case COL_REFLECT:
            return ReadEV3ColorSensorLight(port, ReflectedLight);
        case COL_AMBIENT:
            return ReadEV3ColorSensorLight(port, AmbientLight);
        case COL_COLOR:
            return ReadEV3ColorSensor(port); // TODO: is this the value that the library user expects?
        case US_DIST_CM:
            return ReadEV3UltrasonicSensorDistance(port, CM);
        case US_DIST_MM:
            return ReadEV3UltrasonicSensorDistance(port, MM);
        case US_DIST_IN:
            return ReadEV3UltrasonicSensorDistance(port, IN);
        case GYRO_ANG:
            return ReadEV3GyroSensor(port, EV3GyroAngle);
        case GYRO_RATE:
            return ReadEV3GyroSensor(port, EV3GyroRate);
        case IR_PROX:
            return ReadEV3IrSensorProximity(port);
        case IR_SEEK:
            return readEV3IrSensorSeekForBackCompatibility(port);
        case IR_REMOTE:
            return ReadEV3IrSensorRemote(port, ev3IrSensorChannel[port]);
        case NXT_IR_SEEKER:
            return ReadHTIrSensor(port, Modulated);// TODO: is 'Modulated' the default mode?
        case NXT_TEMP_C:
            return ReadNXTTemperatureSensor(port, C);
        case NXT_TEMP_F:
            return ReadNXTTemperatureSensor(port, F);
        default:
            return -1;
    }
}

int readEV3IrSensorSeekForBackCompatibility (int port) {
    EV3IrSeekResult result = ReadEV3IrSensorSeek(port);
    int channel = ev3IrSensorChannel[port];
    return result.distances[channel]; // TODO: is this the value that the library user expects?
}

int SetIRBeaconCH(int sensorPort, int channel) {
    ev3IrSensorChannel[sensorPort] = channel;
    return 0;
}
