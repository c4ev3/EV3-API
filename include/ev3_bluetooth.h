#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_bluetooth_h
#define ev3_bluetooth_h

#define BLUETOOTH_ADDRESS_LENGTH            18 // 00:00:00:00:00:00 + null terminator
#define	MAX_BLUETOOTH_NAME_LENGTH	        128

typedef int BluetoothConnectionHandle;

/**
 * Connect to the remote device given his bluetooth name or address.
 * This function will block until the connection has been established.
 * @param nameOrAddress
 * @return Connection handle or -1 if wasn't possible to find the address given the device name
 */
BluetoothConnectionHandle ConnectTo(char * nameOrAddress);

/**
 * Wait for an incoming bluetooth connection.
 * This function will block until a connection has been established.
 * @return Connection handle
 */
BluetoothConnectionHandle WaitConnection();

void SendStringTo(BluetoothConnectionHandle to, char * str);

/**
 * Receives a string from a bluetooth connection.
 * This function will block until a message has been received from the connection.
 * It's guaranteed that the string copied in the buffer will be null-terminated.
 * If the incoming message is longer that the buffer length, only the first characters will
 * be copied. A successive call to this function will allow to read the remaining characters.
 * @param from
 * @param buffer Buffer to which the incoming message will be copied to
 * @param bufferLength Maximum Size of the buffer
 * @return Length of the received message.
 */
int ReceiveStringFrom(BluetoothConnectionHandle from, char * buffer, int bufferLength);

void DisconnectFrom(BluetoothConnectionHandle from);

#endif // ev3_bluetooth_h

#ifdef __cplusplus
}
#endif
