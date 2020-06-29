#include "inputs/ev3_input_uart.h"
#include "ev3_sensors.h"
#include "contrib/lms2012/ev3_basictypes.h"
#include "ev3_command.h"
#include "ev3_sensors/ev3_gyro.h"
#include "utility.h"
#include "limits.h"

//
// PRIVATE DECLARATIONS
//

static bool initEV3GyroSensor(int port);

static int universalRead(int port, int mode, int data_length, int index, int offset);

static int parseS16(uint8_t *data, int valueNo);

static int setEV3GyroSoftwareReset(int port);

static void setEV3GyroSensorMode(int port, int mode);

//
// GLOBAL DATA
//

static int ev3GyroSoftwareResetOffset[4][EV3_GYRO_SENSOR_MODES];

SensorHandler * EV3Gyro = &(SensorHandler){
    .Init = initEV3GyroSensor,
    .Exit = NULL,
    .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

//
// IMPLEMENTATION
//

bool initEV3GyroSensor (int port) {
    memset(&ev3GyroSoftwareResetOffset[port], 0, EV3_GYRO_SENSOR_MODES * sizeof(int));
    setEV3GyroSensorMode(port, EV3_GYRO_SENSOR_DEFAULT_MODE);
    return true;
}

int ReadEV3GyroSensorAngle(int port, EV3GyroAngleMode mode) {
    int data_len;
    int target_mode;
    int value_index;

    switch (mode) {
        case EV3GyroNormalAngle:
            data_len = 2;
            target_mode = EV3_GYRO_SENSOR_ANGLE_MODE;
            value_index = 0;
            break;

        case EV3GyroInterleavedAngle:
            data_len = 4;
            target_mode = EV3_GYRO_SENSOR_ANGLE_AND_RATE_MODE;
            value_index = 0;
            break;

        case EV3GyroTiltAngle:
            data_len = 2;
            target_mode = EV3_GYRO_SENSOR_TILT_ANGLE_MODE;
            value_index = 0;
            break;

        default:
            return INT_MIN;
    }

    int angle_offset = ev3GyroSoftwareResetOffset[port][target_mode];

    return universalRead(port, target_mode, data_len, value_index, angle_offset);
}

int ReadEV3GyroSensorRate(int port, EV3GyroRateMode mode) {
    int data_len;
    int target_mode;
    int value_index;

    switch (mode) {
        case EV3GyroNormalRate:
            data_len = 2;
            target_mode = EV3_GYRO_SENSOR_RATE_MODE;
            value_index = 0;
            break;

        case EV3GyroFastRate:
            data_len = 2;
            target_mode = EV3_GYRO_SENSOR_RATE_FAST_MODE;
            value_index = 0;
            break;

        case EV3GyroInterleavedRate:
            data_len = 4;
            target_mode = EV3_GYRO_SENSOR_ANGLE_AND_RATE_MODE;
            value_index = 1;
            break;

        case EV3GyroTiltRate:
            data_len = 2;
            target_mode = EV3_GYRO_SENSOR_TILT_RATE_MODE;
            value_index = 0;
            break;

        default:
            return INT_MIN;
    }

    return universalRead(port, target_mode, data_len, value_index, 0);
}

int universalRead(int port, int mode, int data_length, int index, int offset) {
    uint8_t data[4] = {0};

    setEV3GyroSensorMode(port, mode);

    int retval = readFromUART(port, (DATA8*) data, data_length);
    if (retval != data_length)
        return INT_MIN;

    return parseS16(data, index) - offset;
}

int parseS16(uint8_t *data, int valueNo) {
    uint16_t raw = (data[2*valueNo + 0] << 0u)
                 | (data[2*valueNo + 1] << 8u);

    /**
     * Bit trick overview (sign-extension):
     * - The if checks for the sign-bit.
     * - The and-mask removes the sign bit - that gives us how much bigger is
     *   the actual value compared to the most negative value.
     * - The subtraction just moves the result so that it matches the
     *   previous value.
     */
    if (raw & 0x8000) {
        return (int) (raw & 0x7FFF) - 0x8000;
    }
    return (int) raw;
}

int ResetEV3GyroSensor(int port, EV3GyroResetStrategy mode) {

    uint8_t resetCmd = 0;
    int oldMode = EV3Gyro->currentSensorMode[port];

    switch (mode) {
        case EV3GyroHardwareCommand0x11: // fallthrough
        case EV3GyroHardwareCommand0x88:

            if (oldMode != EV3_GYRO_SENSOR_ANGLE_MODE)
                setEV3GyroSensorMode(port, EV3_GYRO_SENSOR_ANGLE_MODE);

            resetCmd = mode == EV3GyroHardwareCommand0x11
                         ? EV3_GYRO_ZERO_ANGLE_COMMAND
                         : EV3_GYRO_CALIBRATE_COMMAND;
            if (! writeToUART(port, (DATA8*) &resetCmd, sizeof(resetCmd)))
                return INT_MIN;

            if (oldMode != EV3_GYRO_SENSOR_ANGLE_MODE)
                setEV3GyroSensorMode(port, oldMode);

            return 0;

        case EV3GyroHardwareReboot:
            disableUART(port);
            setEV3GyroSensorMode(port, EV3Gyro->currentSensorMode[port]);
            return 0;

        case EV3GyroSoftwareOffset:
            return setEV3GyroSoftwareReset(port);

        default:
            return INT_MIN;
    }
}

int setEV3GyroSoftwareReset(int port) {
    int currentMode = EV3Gyro->currentSensorMode[port];

    ev3GyroSoftwareResetOffset[port][currentMode] = 0;

    int currentAngle = 0;
    switch (currentMode) {
        case EV3_GYRO_SENSOR_RATE_MODE: // fallthrough
        case EV3_GYRO_SENSOR_RATE_FAST_MODE: // fallthrough
        case EV3_GYRO_SENSOR_TILT_RATE_MODE: // fallthrough
        default:
            return -1;

        case EV3_GYRO_SENSOR_ANGLE_MODE:
            currentAngle = ReadEV3GyroSensorAngle(port, EV3GyroNormalAngle);
            break;

        case EV3_GYRO_SENSOR_ANGLE_AND_RATE_MODE:
            currentAngle = ReadEV3GyroSensorAngle(port, EV3GyroInterleavedAngle);
            break;

        case EV3_GYRO_SENSOR_TILT_ANGLE_MODE:
            currentAngle = ReadEV3GyroSensorAngle(port, EV3GyroTiltAngle);
            break;
    }

    ev3GyroSoftwareResetOffset[port][currentMode] = currentAngle;
    return 0;
}

void setEV3GyroSensorMode(int port, int mode) {
    setUARTSensorHandlerMode(EV3Gyro, port, EV3_GYRO_SENSOR_TYPE, mode);
}
