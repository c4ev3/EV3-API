#ifndef EV3_API_HT_IR_V2_H
#define EV3_API_HT_IR_V2_H

#include "ev3_sensors.h"

#define HT_IR_V2_SENSOR_IIC_ADDRESS 0x08

#define HT_IR_V2_SENSOR_DC_READING_MODE_REGISTER        0x42
#define HT_IR_V2_SENSOR_AC_READING_MODE_REGISTER        0x49
#define HT_IR_V2_SENSOR_DC_ALL_READING_MODE_REGISTER    0x43
#define HT_IR_V2_SENSOR_AC_ALL_READING_MODE_REGISTER    0x4A

#define HT_IR_V2_CHANNELS 5

#define HT_IR_V2_SENSOR_DEFAULT_MODE 0

extern SensorHandler * HTIrV2;

typedef enum HTIrV2ReadingMode {
    Modulated,
    Unmodulated ///< Untested

} HTIrV2ReadingMode;

/**
 * Returns the direction from which the IR signal is coming from.
 * The direction ranges from 1 to 9. If no signal is detected, 0 is returned.
 * @param port port to which the sensor is connected
 * @param mode whether to read AC or DC signals
 * @return direction of the IR signal, -1 in case of error.
 */
int ReadHTIrV2Sensor(int port, HTIrV2ReadingMode mode);

/**
 * Read all 5 individual channels of the sensor.
 * @param port port to which the sensor is connected
 * @param mode whether do read AC or DC IR signals
 * @param output array to which the 5 individual channel values are written to.
 *          This is used as an output variable, and the size must be at least 5
 * @return -1 in case of error
 */
int ReadHTIrV2SensorAll(int port, HTIrV2ReadingMode mode, int8_t output[]);

#endif //EV3_API_HT_IR_V2_H
