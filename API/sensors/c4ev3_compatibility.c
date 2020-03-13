#include "ev3_sensors.h"
#include "ev3_sensors/c4ev3_compatibility.h"

//
// PRIVATE DECLARATIONS
//

static bool initializeAllSensors(void);

static void initializeBackCompatibilityIfNeeded(void);

static int readEV3IrSensorSeekForBackCompatibility(int port);

//
// GLOBAL DATA
//

static int sensorModeNames[NUM_INPUTS] = {NO_SEN};
static int ev3IrSensorChannel[NUM_INPUTS] = {0};
static SensorHandler * sensorModeHandlersByModeName[100];

//
// IMPLEMENTATION
//


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

static bool initializeAllSensors(void) {
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

static void initializeBackCompatibilityIfNeeded(void) {
    if (backCompatibilityInitialized) {
        return;
    }

    sensorModeHandlersByModeName[TOUCH_PRESS]   = EV3Touch;
    sensorModeHandlersByModeName[COL_REFLECT]   = EV3Color;
    sensorModeHandlersByModeName[COL_AMBIENT]   = EV3Color;
    sensorModeHandlersByModeName[COL_COLOR]     = EV3Color;
    sensorModeHandlersByModeName[COL_COLOR_RGB] = EV3Color;
    sensorModeHandlersByModeName[US_DIST_CM]    = EV3Ultrasonic;
    sensorModeHandlersByModeName[US_DIST_MM]    = EV3Ultrasonic;
    sensorModeHandlersByModeName[US_DIST_IN]    = EV3Ultrasonic;
    sensorModeHandlersByModeName[GYRO_ANG]      = EV3Gyro;
    sensorModeHandlersByModeName[GYRO_RATE]     = EV3Gyro;
    sensorModeHandlersByModeName[IR_PROX]       = EV3Ir;
    sensorModeHandlersByModeName[IR_SEEK]       = EV3Ir;
    sensorModeHandlersByModeName[IR_REMOTE]     = EV3Ir;
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
            return ReadEV3ColorSensorReflectedLight(port);
        case COL_AMBIENT:
            return ReadEV3ColorSensorAmbientLight(port);
        case COL_COLOR:
            return ReadEV3ColorSensorColor(port);
        case COL_COLOR_RGB: {
#define RGB_TO_8BIT(x) (((x) >> 2) & 0xFF)

            RGB colors = {0};
            int retval = ReadEV3ColorSensorColorRGB(port, &colors);

            if (retval < 0)
                return INT_MIN;

            return (RGB_TO_8BIT(colors.red)   << 16)
                 | (RGB_TO_8BIT(colors.green) <<  8)
                 | (RGB_TO_8BIT(colors.blue)  <<  0);
        }
        case US_DIST_CM:
            return ReadEV3UltrasonicSensorDistance(port, EV3_ULTRASONIC_CM);
        case US_DIST_MM:
            return ReadEV3UltrasonicSensorDistance(port, EV3_ULTRASONIC_MM);
        case US_DIST_IN:
            return ReadEV3UltrasonicSensorDistance(port, EV3_ULTRASONIC_IN);
        case GYRO_ANG:
            return ReadEV3GyroSensorAngle(port, EV3GyroInterleavedAngle);
        case GYRO_RATE:
            return ReadEV3GyroSensorRate(port,  EV3GyroInterleavedRate);
        case IR_PROX:
            return ReadEV3IrSensorProximity(port);
        case IR_SEEK:
            return readEV3IrSensorSeekForBackCompatibility(port);
        case IR_REMOTE:
            return ReadEV3IrSensorRemote(port, ev3IrSensorChannel[port]);
        case NXT_TEMP_C:
            return ReadNXTTemperatureSensor(port, NXT_TEMPERATURE_C);
        case NXT_TEMP_F:
            return ReadNXTTemperatureSensor(port, NXT_TEMPERATURE_F);
        case HT_DIR_DC:
            return ReadHTIrV2Sensor(port, Modulated);
        case HT_DIR_AC:
            return ReadHTIrV2Sensor(port, Unmodulated);
        case HT_DIR_DALL:
        case HT_DIR_AALL:
            // to implement HT_DIR_DALL and HT_DIR_AALL, we need to call ReadHTIrV2SensorAll
            // and to encode the vector of 5 integers to a single integer.
            // TODO: How do we need to encode the value to preserve back-compatibility?
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
