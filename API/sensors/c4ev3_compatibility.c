#include <stddef.h>
#include "ev3_sensors.h"
#include "c4ev3_compatibility.h"

static int sensorModeNames[NUM_INPUTS] = {NO_SEN};
static int ev3IrSensorChannel[NUM_INPUTS] = {0};
static SensorHandler * sensorModeHandlersByModeName[100];

int SetAllSensorMode(int name_1, int name_2, int name_3, int name_4) {
    sensorModeNames[IN_1] = name_1;
    sensorModeNames[IN_2] = name_2;
    sensorModeNames[IN_3] = name_3;
    sensorModeNames[IN_4] = name_4;
    if (!initializeAllSensors()) {
        return -1;
    }
    return 0;
}

bool initializeAllSensors () {
    initializeBackCompatibilityIfNeeded();
    SensorHandler * handlers[4];
    for (int i = 0; i < NUM_INPUTS; i++) {
        int modeName = sensorModeNames[i];
        if (modeName == NO_SEN) {
            handlers[i] = NULL;
        } else {
            handlers[i] = sensorModeHandlersByModeName[modeName];
        }
    }
    return SetAllSensors(handlers[0],handlers[1],handlers[2],handlers[3]);
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
    //sensorModeHandlersByModeName[NXT_IR_SEEKER] = HTIrV2; // TODO: This is not HTIrV2
    sensorModeHandlersByModeName[NXT_TEMP_C]    = NXTTemperature;
    sensorModeHandlersByModeName[NXT_TEMP_F]    = NXTTemperature;
    sensorModeHandlersByModeName[HT_DIR_DC]     = HTIrV2;
    sensorModeHandlersByModeName[HT_DIR_AC]     = HTIrV2;
    sensorModeHandlersByModeName[HT_DIR_DALL]   = HTIrV2;
    sensorModeHandlersByModeName[HT_DIR_AALL]   = HTIrV2;
    sensorModeHandlersByModeName[HT_COMP]       = HTCompass;

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
            return ReadEV3ColorSensor(port);
        case US_DIST_CM:
            return ReadEV3UltrasonicSensorDistance(port, EV3_ULTRASONIC_CM);
        case US_DIST_MM:
            return ReadEV3UltrasonicSensorDistance(port, EV3_ULTRASONIC_MM);
        case US_DIST_IN:
            return ReadEV3UltrasonicSensorDistance(port, EV3_ULTRASONIC_IN);
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
            return -1; // TODO: NXT ir not implemented yet
        case NXT_TEMP_C:
            return ReadNXTTemperatureSensor(port, NXT_TEMPERATURE_C);
        case NXT_TEMP_F:
            return ReadNXTTemperatureSensor(port, NXT_TEMPERATURE_F);
        case HT_DIR_DC:
            return ReadHTIrV2Sensor(port, Modulated);
        case HT_DIR_AC:
            return ReadHTIrV2Sensor(port, Unmodulated);
        case HT_DIR_DALL: // TODO: Not implemented yet
        case HT_DIR_AALL:
            return -1;
        case HT_COMP:
            return ReadHTCompassSensor(port, HTCompassCompass);
        default:
            return -1;
    }
}

int readEV3IrSensorSeekForBackCompatibility (int port) {
    EV3IrSeekResult result;
    ReadEV3IrSensorSeek(port, &result);
    int channel = ev3IrSensorChannel[port];
    return result.distances[channel];
}

int SetIRBeaconCH(int sensorPort, int channel) {
    ev3IrSensorChannel[sensorPort] = channel;
    return 0;
}
