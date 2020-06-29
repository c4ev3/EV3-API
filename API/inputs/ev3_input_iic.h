#ifndef EV3_API_EV3_INPUT_IIC_H
#define EV3_API_EV3_INPUT_IIC_H

#include <stdbool.h>
#include "contrib/lms2012/ev3_basictypes.h"

bool initEV3IICInput(void);

bool initIICPort(int port, int deviceAddress);

/**
 * Reads data from IIC sensor.
 * @param port port to which the sensor is connected
 * @param registerAddress address of the first register to read. IF length > 1,
 *      registers will be read one after the other
 * @param buffer buffer to which bytes will be written. The size of this buffer
 *      must be at least 'length' long
 * @param length how many bytes to read
 * @return number of bytes read, -1 in case of error
 */
int readFromIIC(int port, int registerAddress, DATA8 * buffer, int length);

/**
 * Read values, received from IIC device, from shared memory.
 * You need to call this function to get the read value from the sensor after calling writeToIIC or startPollingFromIIC.
 * @param sensorPort port to which the sensor is connected
 * @param buffer buffer to which bytes will be written. The size of this buffer
 *      must be at least 'length' long
 * @param length how many bytes to read
 * @return bytes read count, -1 in case of error
 */
int readFromIICSharedMemory(int port, DATA8 * buffer, int length);

/**
 * Writes to IIC device connected to sensor port
 * @param port port to which the sensor is connected
 * @param toWrite data to write to the sensor
 * @param toWriteLength how many bytes to send to the sensor
 * @param repeatTimes how many times send the message. If 0 the message is sent
 * every 'repeatInterval' milliseconds infinitely.
 * @param repeatInterval delay between consecutive writings
 * @param responseLength number of bytes that will be read from the IIC device
 *      and copied to shared memory
 */
void writeToIIC(int port, DATA8 *toWrite, int toWriteLength, int repeatTimes, int repeatInterval, int responseLength);

/**
 * Start to continuously send an IIC message to ask for data. The data from the sensor can be
 * read using readFromIICSharedMemory
 * @param sensorPort
 * @param registerAddress
 * @param pollDelay
 */
void startPollingFromIIC(int port, int registerAddress, int pollDelay);

void exitEV3IICInput(void);

#endif //EV3_API_EV3_INPUT_IIC_H
