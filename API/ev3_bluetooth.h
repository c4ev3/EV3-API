#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_bluetooth_h
#define ev3_bluetooth_h

#define BLUETOOTH_ADDRESS_LENGTH            sizeof("00:00:00:00:00:00")
#define MAX_BLUETOOTH_NAME_LENGTH           128

void BluetoothInit(void);

typedef int BluetoothConnectionHandle;

/**
 * Connect to the remote device given his bluetooth name or address.
 * If the connection can't be established, a new attempt will be tried after 1
 * second.
 * @param nameOrAddress
 * @param attempts maximum number of attempts. This argument should be >= 1,
 *        otherwise no attempt will be tried (and the connection will never be
 *        established)
 * @return Connection handle or -1 if wasn't possible to find the address given the device name
 */
BluetoothConnectionHandle ConnectTo(const char * nameOrAddress, int attempts);

/**
 * Wait for an incoming bluetooth connection from another device
 * @return Connection handle or -1 if there was an error while waiting for the connection
 */
BluetoothConnectionHandle WaitConnection(void);

/**
 * Sends a string to the remote device.
 * The string is sent without the null terminator, since ReceiveStringFrom will
 * add it once it's received.
 * @param to handle of the connection to use to send the string
 * @param str null-terminated string to send
 * @return number of bytes sent or -1 if there was an error. It may happen that
 *         the number of sent bytes is smaller than the length of the string.
 */
int SendStringTo(BluetoothConnectionHandle to, const char * str);

/**
 * Receives a string from a connection, copying it into the buffer. The buffer
 * will be null-terminated.
 * @param from connection to use to receive the string
 * @param buffer buffer where to write the null-terminated received string
 * @param bufferLength length of the buffer (max length of the string)
 * @return number of bytes received or -1 if there was an error
 */
int ReceiveStringFrom(BluetoothConnectionHandle from, char * buffer, int bufferLength);

void DisconnectFrom(BluetoothConnectionHandle from);

void BluetoothExit(void);

#endif // ev3_bluetooth_h

#ifdef __cplusplus
}
#endif
