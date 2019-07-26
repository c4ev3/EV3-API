#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <errno.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ev3_bluetooth.h"

#define DEFAULT_C4EV3_RFCOMM_CHANNEL    2
#define MAX_CONNECTIONS				    7
#define SHORT_BLUETOOTH_ADDRESS_LENGTH  13 // 000000000000 + null terminator

#define KNOWN_BLUETOOTH_NAMES_FILE_NAME_LENGTH 256
#define KNOWN_BLUETOOTH_NAMES_FILE_ROW_LENGTH  256

void BluetoothInit () { }

BluetoothConnectionHandle connections[MAX_CONNECTIONS];
int currentConnections = 0;

void addConnectionToList (BluetoothConnectionHandle c) {
	if (currentConnections > MAX_CONNECTIONS) {
		return;
	}
	connections[currentConnections++] = c;
}

void removeConnectionFromList (BluetoothConnectionHandle c) {
	int i;
	for (i = 0; i < currentConnections; i++) {
		if (connections[i] == c) {
			break;
		}
	}
	if (i >= currentConnections) {
		return;
	}
	currentConnections--;
	for (; i < currentConnections; i++) {
		connections[i] = connections[i + 1];
	}
}


bool isBluetoothAddress(char * nameOrAddress);
bool findAddressByBluetoothName(char * name, char * address);
BluetoothConnectionHandle connectByBluetoothAddress(char * address);

BluetoothConnectionHandle ConnectTo(char * nameOrAddress) {
	char address[BLUETOOTH_ADDRESS_LENGTH];
	if (isBluetoothAddress(nameOrAddress)) {
		strcpy(address, nameOrAddress);
	} else {
		bool found = findAddressByBluetoothName(nameOrAddress, address);
		if (!found) {
		    return -1;
		}
	}
	BluetoothConnectionHandle c = connectByBluetoothAddress(address);
	addConnectionToList(c);
	return c;
}

bool isBluetoothAddress(char * nameOrAddress) {
    return bachk(nameOrAddress) == 0;
}

FILE * openKnownBluetoothNamesFile();
void getLocalBluetoothAddress (char * address);
void getKnownBluetoothNamesFile (char * localBluetoothAddress, char * fileName);
void getAddressFromKnownBluetoothNamesFileRow(char *addressAndName, char * address);
void getNameFromKnownBluetoothNamesFileRow(char *addressAndName, char * name);

bool findAddressByBluetoothName(char * nameToFind, char * foundAddress) {
    FILE * namesFile = openKnownBluetoothNamesFile();
    if (namesFile == NULL) {
        perror("open known bluetooth names file");
        return false;
    }
    char addressAndName[KNOWN_BLUETOOTH_NAMES_FILE_ROW_LENGTH];
    bool found = false;
    while (!found && fgets(addressAndName, KNOWN_BLUETOOTH_NAMES_FILE_ROW_LENGTH, namesFile)) {
        char address[BLUETOOTH_ADDRESS_LENGTH];
        getAddressFromKnownBluetoothNamesFileRow(addressAndName, address);

        char name[MAX_BLUETOOTH_NAME_LENGTH];
        getNameFromKnownBluetoothNamesFileRow(addressAndName, name);

        if (strcmp(name, nameToFind) == 0) {
            found = true;
            strcpy(foundAddress, address);
        }
    }
    fclose(namesFile);
    return found;
}

FILE * openKnownBluetoothNamesFile() {
    char localBluetoothAddress[BLUETOOTH_ADDRESS_LENGTH];
    getLocalBluetoothAddress(localBluetoothAddress);

    char knownBluetoothNamesFile[KNOWN_BLUETOOTH_NAMES_FILE_NAME_LENGTH];
    getKnownBluetoothNamesFile(localBluetoothAddress, knownBluetoothNamesFile);

    return fopen(knownBluetoothNamesFile, "r");
}

void getLocalBluetoothAddress (char * address) {
    FILE * addressFile = fopen("/home/root/lms2012/sys/settings/BTser", "r");
    char shortAddress[SHORT_BLUETOOTH_ADDRESS_LENGTH];
    fgets(shortAddress, SHORT_BLUETOOTH_ADDRESS_LENGTH, addressFile);
    fclose(addressFile);

    address[0] = toupper(shortAddress[0]);
    address[1] = toupper(shortAddress[1]);
    address[2] = ':';
    address[3] = toupper(shortAddress[2]);
    address[4] = toupper(shortAddress[3]);
    address[5] = ':';
    address[6] = toupper(shortAddress[4]);
    address[7] = toupper(shortAddress[5]);
    address[8] = ':';
    address[9] = toupper(shortAddress[6]);
    address[10] = toupper(shortAddress[7]);
    address[11] = ':';
    address[12] = toupper(shortAddress[8]);
    address[13] = toupper(shortAddress[9]);
    address[14] = ':';
    address[15] = toupper(shortAddress[10]);
    address[16] = toupper(shortAddress[11]);
    address[17] = 0;
}

void getKnownBluetoothNamesFile (char * localBluetoothAddress, char * fileName) {
    strcpy(fileName, "/mnt/ramdisk/bluetooth/");
    strcat(fileName, localBluetoothAddress);
    strcat(fileName, "/names");
}

void getAddressFromKnownBluetoothNamesFileRow(char *addressAndName, char * address) {
    strncpy(address, addressAndName, BLUETOOTH_ADDRESS_LENGTH - 1);
    address[BLUETOOTH_ADDRESS_LENGTH - 1] = 0;
}

void getNameFromKnownBluetoothNamesFileRow(char *addressAndName, char * name) {
    strcpy(name, &addressAndName[18]);
    name[strlen(name) - 1] = 0; // remove new line
}

bool waitAsyncConnection (int socket);

BluetoothConnectionHandle connectByBluetoothAddress(char * address) {
    struct sockaddr_rc remoteAddress;
    remoteAddress.rc_family = AF_BLUETOOTH;
    remoteAddress.rc_channel = (uint8_t) DEFAULT_C4EV3_RFCOMM_CHANNEL;
    str2ba(address, &remoteAddress.rc_bdaddr);
    printf("connecting...\n");
    while (true) {
        int socketToServer = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        int connectResult = connect(socketToServer, (struct sockaddr *) &remoteAddress, sizeof(remoteAddress));

        /**
         * 'connect' block only for a small amount of time. Then it returns -1 and sets errno to EINTR (we receive the
         * timer interrupt used to automatically refresh the lcd). Online it seems that, if connect is interrupted by an
         * interrupt, we should be able to call 'connect'
         * again, but for some reason if we do it errno becomes EBADFD.
         * 
         * We didn't ask to use the socket in a non blocking wait (using the SOCK_NONBLOCK flag), but if we check the connection
         * status with 'pool' and 'getsockopt' (the way we would do if we were using async socket) it works.
        */
        if (connectResult == 0 || errno == EINTR) {
            bool connected = waitAsyncConnection(socketToServer);
            if (connected) {
                return socketToServer;
            }
        }
        
        // try to connect again later
        close(socketToServer);
        sleep(1);
    }
}

bool waitAsyncConnection (int socket) {
    struct pollfd toMonitor;
    toMonitor.fd = socket;
    toMonitor.events = POLLOUT;
    int poolResult;
    do {
        poolResult = poll(&toMonitor, 1, -1);
    } while (poolResult == -1 && errno == EINTR);

    if (poolResult != -1) {
        int optval;
        socklen_t optLength = sizeof(optval);
        int getSockOptResult = getsockopt(socket, SOL_SOCKET, SO_ERROR, &optval, &optLength);
        if (getSockOptResult == 0 && optval == 0) {
            printf("connected\n");
            return true;
        }
    }

    return false;
}

bool isServerStarted();
void startServer ();
BluetoothConnectionHandle acceptIncomingConnection ();

BluetoothConnectionHandle WaitConnection() {
	if(!isServerStarted()) {
		startServer();
	}
	return acceptIncomingConnection();
}


int serverSocket = -1;

bool isServerStarted(){
	return serverSocket > 0;
}

void bindServerSocketToFirstAvailableChannel ();

void startServer () {
    serverSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    // TODO: Handle error
	bindServerSocketToFirstAvailableChannel();
    int res = listen(serverSocket, 1); // TODO: do we keep 1 as backlog?
    // TODO: Handle error
}

void bindServerSocketToFirstAvailableChannel () {
    struct sockaddr_rc listenAddress = { 0 };
    listenAddress.rc_family = AF_BLUETOOTH;
    listenAddress.rc_bdaddr = *BDADDR_ANY;
    listenAddress.rc_channel = DEFAULT_C4EV3_RFCOMM_CHANNEL;
    bind(serverSocket, (struct sockaddr *) &listenAddress, sizeof(listenAddress));
    printf("> Listening on channel: %d\n", listenAddress.rc_channel);
}

BluetoothConnectionHandle acceptIncomingConnection () {
	struct sockaddr_rc remoteAddress = { 0 };
	socklen_t addressLength = sizeof(remoteAddress);
    BluetoothConnectionHandle c;
    printf("Waiting new connection\n");
    do {
    	c = accept(serverSocket, (struct sockaddr *) &remoteAddress, &addressLength);
    } while (c == -1);
    printf("Received new connection\n");
    return c;
}


void SendStringTo(BluetoothConnectionHandle to, char * str) {
	write(to, str, strlen(str));
}

int ReceiveStringFrom(BluetoothConnectionHandle from, char * buffer, int bufferLength) {
	int readBytes;
	do {
		readBytes = read(from, buffer, bufferLength - 1);
	} while (readBytes == -1);
	buffer[readBytes] = 0;
	return readBytes;
}

void DisconnectFrom(BluetoothConnectionHandle from) {
	close(from);
	removeConnectionFromList(from);
}

void disconnectAllConnections();
void stopServer();

void BluetoothExit() {
	disconnectAllConnections();
	stopServer();
}

void disconnectAllConnections() {
	while (currentConnections > 0) {
		DisconnectFrom(connections[0]);
	}
}

void stopServer() {
	close(serverSocket);
	serverSocket = -1;
}
