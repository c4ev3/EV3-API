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
 *
 * There are multiple types of resets:
 *
 *  - "Angle offset reset" will only reset the accumulated angle to zero.
 *    The rate of rotation will remain unchanged. The gyro sensor does
 *    not need to stay idle during this operation.
 *
 *  - "Recalibration" will perform bias elimination on the rate of rotation.
 *    The angle will be reset as well. During this operation, the gyro
 *    sensor must remain stationary, otherwise the rotational rate will
 *    have a constant error and the accumulated angle will be increasingly wrong.
 *
 *  - "Reboot" is just a recalibration. In this context reboot means
 *    the sensor will timeout and it will be re-detected by the brick.
 *
 * For more details, see https://github.com/ev3dev/ev3dev/issues/1387
 */
typedef enum EV3GyroResetStrategy {
    /**
     * @brief Send a 0x11 direct command to the sensor.
     * This will switch the sensor to GYRO-ANG, send the command and switch the mode back.
     *
     * What this will really do depends on the sensor revision and current mode:
     *  - on xxN2/xxN3 sensors
     *    - for GYRO-ANG this will only reset the angle offset (possibly fastest)
     *    - for GYRO-G&A this will reboot the sensor (slow - 3 seconds)
     *    - for other modes it will recalibrate the sensor (fast)
     *  - on xxN4 and newer sensors
     *    - this will only reset the offset (fast), it will not recalibrate the sensor
     */
    EV3GyroHardwareCommand0x11,

    /**
     * @brief Send a 0x88 direct command to the sensor.
     * This will switch the sensor to GYRO-ANG, send the command and switch the mode back.
     *
     * What this will really do depends on the sensor revision and current mode:
     *  - on xxN2/xxN3 sensors
     *     - for GYRO-ANG this will perform recalibration (possibly fastest)
     *     - for GYRO-G&A this will reboot the sensor (slow - 3 seconds)
     *     - for other modes it will recalibrate the sensor (fast)
     *  - on xxN4 and newer sensors
     *    - this will only reset the offset (fast), it will not recalibrate the sensor
     *
     * @remark On xxN4 and newer sensors, please prefer the 0x11 command.
     *         That one is used by the EV3-G environment. This command
     *         was found only accidentally in a debugging program
     *         in the firmware source code.
     */
    EV3GyroHardwareCommand0x88,

    /**
     * @brief Recalibrate the sensor by forcefully restarting it.
     *
     * This strategy should work equally across all sensor revisions.
     * It is quite slow (2.5 seconds), but it is the fastest option
     * for xxN2/xxN3 GYRO-G&A mode. It is also possibly the only way
     * to recalibrate xxN4 and newer sensors.
     *
     * The reboot through this strategy should be the fastest known
     * (apart from physically unplugging the sensor and plugging it back).
     * This strategy resets the UART comm stack on the given port. This
     * stops the brick from sending keep-alives to the sensor and that
     * will make the sensor reset on its own (it will be forced to by its
     * watchdog).
     */
    EV3GyroHardwareReboot,

    /**
     * @brief Set current angle to zero by a software correction.
     *
     * This will not poke the hardware in an intrusive way. It will just
     * read the current uncorrected measured angle and it will store it
     * as the true "zero" angle. The correction is then performed
     * automatically in the ReadEV3GyroSensorAngle() function.
     *
     * The correction offset is specific for each mode and each port.
     *
     * If the sensor is already initialized and giving measurements,
     * expect this strategy to be very fast.
     *
     * @remark The correction will be used only for the mode that the
     *         sensor currently is in.
     * @remark If the sensor is not in an angle-giving mode, this
     *         strategy will fail.
     * @remark If you combine this strategy with other stategies,
     *         this one has to be the last so that the stored offset
     *         is correct.
     * @remark Also beware when using multiple sensor modes.
     *         On N2/N3 sensors, changing sensor modes will trigger a hardware
     *         recalibration, which will interfere with the software correction.
     */
    EV3GyroSoftwareOffset,
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
