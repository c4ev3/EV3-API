#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_bluetooth_h
#define ev3_bluetooth_h

#define BLUETOOTH_ADDRESS_LENGTH			18
#define	BLUETOOTH_STRING_MESSAGE_MAX_LENGTH 128

void BluetoothInit();

typedef int BluetoothConnectionHandle;

BluetoothConnectionHandle ConnectTo(char * name);
BluetoothConnectionHandle WaitConnection();

void SendStringTo(BluetoothConnectionHandle to, char * str);

int ReceiveStringFrom(BluetoothConnectionHandle from, char * destination, int maxLength);

void DisconnectFrom(BluetoothConnectionHandle from);

void BluetoothExit();

#endif // ev3_bluetooth_h

#ifdef __cplusplus
}
#endif
