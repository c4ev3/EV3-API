#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_bluetooth_h
#define ev3_bluetooth_h

#define BLUETOOTH_ADDRESS_LENGTH            18 // 00:00:00:00:00:00 + null terminator
#define	MAX_BLUETOOTH_NAME_LENGTH	        128

void BluetoothInit();

typedef int BluetoothConnectionHandle;

/**
 * Connect to the remtoe device given his bluetooth name or address
 * @param nameOrAddress
 * @return Connection handle or -1 if wasn't possible to find the address given the device name
 */
BluetoothConnectionHandle ConnectTo(char * nameOrAddress);
BluetoothConnectionHandle WaitConnection();

void SendStringTo(BluetoothConnectionHandle to, char * str);

int ReceiveStringFrom(BluetoothConnectionHandle from, char * buffer, int bufferLength);

void DisconnectFrom(BluetoothConnectionHandle from);

void BluetoothExit();

#endif // ev3_bluetooth_h

#ifdef __cplusplus
}
#endif
