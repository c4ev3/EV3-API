#ifndef EV3_API_HT_COMPASS_H
#define EV3_API_HT_COMPASS_H

#include "ev3_sensors.h"

#define HT_COMPASS_SENSOR_IIC_ADDRESS 0x01

#define HT_COMPASS_SENSOR_DEFAULT_MODE 0

extern SensorHandler * HTCompass;

typedef enum HTCompassReadingMode {
    HTCompassCompass,
    HTCompassAngle
} HTCompassReadingMode;

int ReadHTCompassSensor(int port, HTCompassReadingMode mode);

/**
 * Starts the compass sensor calibration.
 * Once the calibration is started, you should rotate very slowly the sensor,
 * taking at least 20 seconds per rotation. You should make 1.5 to 2 full rotations.
 * After you completed the calibration, call 'StopHTCompassCalibration'.
 *
 * Those instructions have been adapted from the LeJOS documentation.
 * @param port port to which the sensor is connected
 */
void StartHTCompassCalibration(int port);

/**
 * Stops the calibration initialized calling 'StartHTCompassCalibration'
 * @param port port to which the sensor is connected
 */
void StopHTCompassCalibration(int port);


#endif //EV3_API_HT_COMPASS_H
