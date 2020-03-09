#ifndef EV3_API_EV3_GYRO_H
#define EV3_API_EV3_GYRO_H

#include "ev3_sensors.h"

// see http://docs.ev3dev.org/projects/lego-linux-drivers/en/ev3dev-buster/sensor_data.html#lego-ev3-gyro

#define EV3_GYRO_SENSOR_TYPE                32

#define EV3_GYRO_SENSOR_ANGLE_MODE          0
#define EV3_GYRO_SENSOR_RATE_MODE           1
#define EV3_GYRO_SENSOR_RATE_FAST_MODE      2
#define EV3_GYRO_SENSOR_ANGLE_AND_RATE_MODE 3
#define EV3_GYRO_SENSOR_FACTORY_MODE        4
#define EV3_GYRO_SENSOR_TILT_RATE_MODE      5
#define EV3_GYRO_SENSOR_TILT_ANGLE_MODE     6
#define EV3_GYRO_SENSOR_MODES               7
#define EV3_GYRO_SENSOR_DEFAULT_MODE        EV3_GYRO_SENSOR_ANGLE_MODE

#define EV3_GYRO_ZERO_ANGLE_COMMAND  0x11
#define EV3_GYRO_CALIBRATE_COMMAND   0x88


/**
 * @brief EV3 Gyro Sensor (LEGO #45505)
 */
extern SensorHandler *EV3Gyro;

/**
 * @brief Gyro sensor reset strategy
 *
 * These constants specify the reset algorithm to be used.
 */
typedef enum EV3GyroResetStrategy {
    /**
     * @brief Set current angle to zero by sending a direct command to the sensor.
     * - this will:
     *   - switch the sensor to GYRO-ANG (if needed)
     *   - send the command 0x11 (GyroNoAutoZero) to the sensor
     *   - switch the sensor back (if needed)
     * - generally it should be quite fast
     *   - except on N2/N3 from GYRO-G&A where it will take around 3 seconds
     * - on N2/N3 sensors this will behave identically to
     *   EV3GyroHardwareFullCalibration - it will recalibrate the sensor as well
     */
    EV3GyroHardwareZeroAngle,

    /**
     * @brief Recalibrate the sensor by sending a direct command to the sensor.
     * - this will:
     *   - switch the sensor to GYRO-ANG (if needed)
     *   - send the command 0x11 (GyroNoAutoZero) to the sensor
     *   - switch the sensor back (if needed)
     * - generally it should be quite fast
     *   - except on N2/N3 from GYRO-G&A where it will take around 3 seconds
     */
    EV3GyroHardwareFullCalibration,

    /**
     * @brief Recalibrate the sensor by forcefully restarting it.
     * - it should work for all sensor versions
     * - however it is quite slow (~2.5 secs)
     *   - it is the fastest method for the N2/N3 GYRO-G&A mode though
     */
    EV3GyroHardwareReboot,

    /**
     * @brief Set current angle to zero by a software correction.
     * - the current angle measured by the sensor will be captured
     * - this angle will then be subtracted from all future measurements
     * - it should be very fast
     * - the offset will be specific to the current sensor mode
     * @remark If you combine this strategy with other stategies,
     *         this one has to be the last so that the stored offset
     *         is correct.
     * @remark Also beware when using multiple sensor modes.
     *         On N2/N3 sensors, changing modes will trigger a hardware
     *         recalibration, which will interfere with the software correction.
     */
    EV3GyroSoftwareZeroAngle,
} EV3GyroResetStrategy;

/**
 * @brief Angle reading modes.
 * @remark The behavior after performing > ~91 turns from the origin
 *         is dependent on the sensor revision.
 *         On some sensors the value will overflow, on other ones
 *         the value will stay stuck at +32767 or -32768 (see ev3dev wiki)
 */
typedef enum EV3GyroAngleMode {
    /**
     * @brief Read the angle measured by the sensor using the GYRO-ANG mode.
     * The measurement will be done along the axis marked on the sensor.
     * Clockwise rotation (when looking on the sensor top) will increase the reported values.
     */
    EV3GyroNormalAngle,

    /**
     * @brief Read the angle measured by the sensor using the GYRO-G&A mode.
     * Use this mode if you want to read both angle and rate at once.
     * The measurement will be done along the axis marked on the sensor.
     * Clockwise rotation (when looking on the sensor top) will increase the reported values.
     * @remark On N2/N3 sensors, switching out of this mode will take
     *         a long time (~3 seconds), because the sensor has to time out
     *         and reboot. On newer sensors, this shouldn't be an issue.
     */
    EV3GyroInterleavedAngle,

    /**
     * @brief Read the angle along an alternative axis using the TILT-ANG mode.
     * The measurement will be done along the cable axis.
     * Clockwise rotation (when looking against the cable plug) will increase the reported values.
     * @remark This mode is only available in N4+ sensors.
     */
    EV3GyroTiltAngle,
} EV3GyroAngleMode;

/**
 * @brief Rate reading modes.
 */
typedef enum EV3GyroRateMode {
    /**
     * @brief Read the rotation rate measured by the sensor using the GYRO-RATE mode.
     * The measurement will be done along the axis marked on the sensor.
     * Clockwise rotation (when looking on the sensor top) will report positive values.
     * @remark This mode supports only +- 440 °/s rotational rates.
     *         For faster rotations, try using the GYRO-FAS mode.
     */
    EV3GyroNormalRate,

    /**
     * @brief Read the fast rotation rate measured by the sensor using the GYRO-FAS mode.
     * The measurement will be done along the axis marked on the sensor.
     * Clockwise rotation (when looking on the sensor top) will report positive values.
     * @remark This mode supports +- 2000 °/s rotational rates. As a
     *         tradeoff, it likely has lower precision compared to the GYRO-RATE mode.
     */
    EV3GyroFastRate,

    /**
     * @brief Read rate measured by the sensor using the GYRO-G&A mode.
     * Use this mode if you want to read both angle and rate at once.
     * The measurement will be done along the axis marked on the sensor.
     * Clockwise rotation (when looking on the sensor top) will report positive values.
     * @remark On N2/N3 sensors, switching out of this mode will take
     *         a long time (~3 seconds), because the sensor has to time out
     *         and reboot. On newer sensors, this shouldn't be an issue.
     */
    EV3GyroInterleavedRate,

    /**
     * @brief Read the rotational rate along an alternative axis using the TILT-RATE mode.
     * The measurement will be done along the cable axis.
     * Clockwise rotation (when looking against the cable plug) will report positive values.
     * @return Measured rate in degrees per second.
     *         The rate is measured along the cable axis; clockwise rotation
     *         when looking against the cable plug will give positive values.
     *         On error, INT_MIN is returned.
     * @remark This mode supports only +- 440 °/s rotational rates.
     * @remark This mode is only available in N4+ sensors.
     */
    EV3GyroTiltRate,
} EV3GyroRateMode;

/**
 * @brief Read the current angle reported by the sensor.
 * @param port Port to which the sensor is connected
 * @param mode How to measure the angle
 * @return Measured angle in degrees.
 *         On error, INT_MIN is returned.
 * @sa EV3GyroAngleMode
 */
int ReadEV3GyroSensorAngle(int port, EV3GyroAngleMode mode);

/**
 * @brief Read the current rate of rotation reported by the sensor.
 * @param port Port to which the sensor is connected
 * @param mode How to measure the rate
 * @return Measured rate in degrees per second.
 *         On error, INT_MIN is returned.
 * @sa EV3GyroRateMode
 */
int ReadEV3GyroSensorRate(int port, EV3GyroRateMode mode);

/**
 * @brief Resets the gyro sensor connected to the specified port
 * @param port Port to which the sensor is connected
 * @param mode Reset strategy to use
 * @return 0 on success, INT_MIN on failure.
 */
int ResetEV3GyroSensor(int port, EV3GyroResetStrategy mode);

#endif //EV3_API_EV3_GYRO_H
