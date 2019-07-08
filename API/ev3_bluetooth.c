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

#define MAX_CONNECTIONS				    7
#define MAX_SCAN_RESULTS			    256
#define	MAX_BLUETOOTH_NAME_LENGTH	    256
#define MAX_RFCOMM_CHANNEL	            30
#define DEFAULT_LMS_RFCOMM_CHANNEL 	    1

#define BLUETOOTH_ADDRRESS_LENGTH       18 // 00:00:00:00:00:00 + null terminator
#define SHORT_BLUETOOTH_ADDRESS_LENGTH  13 // 000000000000 + null terminator

void BluetoothInit () {

}


bool isBluetoothAddress(char * nameOrAddress);
void findAddressByBluetoothName(char * name, char * address);
BluetoothConnectionHandle connectByBluetoothAddress(char * address);

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

BluetoothConnectionHandle ConnectTo(char * nameOrAddress) {
	char address[BLUETOOTH_ADDRESS_LENGTH];
	if (isBluetoothAddress(nameOrAddress)) {
		strcpy(address, nameOrAddress);
	} else {
		findAddressByBluetoothName(nameOrAddress, address);
        printf("found %s\n", address);
        exit(-1);
	}
	BluetoothConnectionHandle c = connectByBluetoothAddress(address);
	addConnectionToList(c);
	return c;
}

bool isBluetoothAddress(char * nameOrAddress) {
    return bachk(nameOrAddress) == 0;
}



void getLocalBluetoothAddress (char * address);
void getKnownBluetoothNamesFile (char * localBluetoothAddress, char * fileName);

void findAddressByBluetoothName(char * nameToFind, char * foundAddress) {
    char localBluetoothAddress[BLUETOOTH_ADDRRESS_LENGTH];
    getLocalBluetoothAddress(localBluetoothAddress);
    printf("my mac: %s\n", localBluetoothAddress);

    char knownBluetoothNamesFile[MAX_BLUETOOTH_NAME_LENGTH];
    getKnownBluetoothNamesFile(localBluetoothAddress, knownBluetoothNamesFile);

    FILE * namesFile = fopen(knownBluetoothNamesFile, "r");
    if (namesFile == -1) {
        // TODO: Handle error
        perror("open\n");
    }
    char addressAndName[256];
    bool found = false;
    while (!found && fgets(addressAndName, 256, namesFile)) {
        char address[BLUETOOTH_ADDRRESS_LENGTH];
        strncpy(address, addressAndName, 17);
        address[17] = 0;

        char name[MAX_BLUETOOTH_NAME_LENGTH];
        strcpy(name, &addressAndName[18]);
        name[strlen(name) - 1] = 0; // remove new line

        if (strcmp(name, nameToFind) == 0) {
            found = true;
            strcpy(foundAddress, address);
        }
    }
    fclose(namesFile);
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

BluetoothConnectionHandle connectByBluetoothAddress(char * address) {
    struct sockaddr_rc remoteAddress;
    remoteAddress.rc_family = AF_BLUETOOTH;
    remoteAddress.rc_channel = (uint8_t) 2; // TODO: Can we keep thye channel hard coded?
    str2ba(address, &remoteAddress.rc_bdaddr);

    int connectResult = -1;
    while (true) {
        int socketToServer = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        int connectResult = connect(socketToServer, (struct sockaddr *) &remoteAddress, sizeof(remoteAddress));

        /**
         * 'connect' block only for a small amount of time. Then it return -1 and set errno to EINTR, that is, we received
         * an interrupt. Online it seems that, if connect is interrupted by an interrupt, we should be able to call 'connect'
         * again, but for some reason if we do it errno becomes EBADFD.
         * 
         * We didn't ask to use the socket in a non blocking wait (using the SOCK_NONBLOCK flag),but if we check the connections
         * status with 'pool' and 'getsockopt' (the way we would do if we were using async socket) it works.
        */
        if (errno == EINTR) {
            struct pollfd toMonitor;
            toMonitor.fd = socketToServer;
            toMonitor.events = POLLOUT;
            int poolResult = -1;
            do {
                poolResult = poll(&toMonitor, 1, -1);
            } while (poolResult == -1 && errno == EINTR);

            if (poolResult != -1) {
                int optval;
                socklen_t optLength = sizeof(optval);
                int getSockOptResult = getsockopt(socketToServer, SOL_SOCKET, SO_ERROR, &optval, &optLength);

                if (getSockOptResult != -1 || optval == 0) {
                    printf("We should now be connected\n");
                    return socketToServer;
                }
            }
        }
        
        // try to connect again later
        close(socketToServer);
        sleep(1);
    }
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
    int bindResult = -1;
    do {
    	int channel;
	    for (channel = DEFAULT_LMS_RFCOMM_CHANNEL + 1; channel < MAX_RFCOMM_CHANNEL && bindResult == -1; channel++) {
	        listenAddress.rc_channel = channel;
	        bindResult = bind(serverSocket, (struct sockaddr *) &listenAddress, sizeof(listenAddress));
	    }	
    } while (bindResult == -1);// TODO: Handle error
    printf("> Listening on channel: %d\n", listenAddress.rc_channel);
}

BluetoothConnectionHandle acceptIncomingConnection () {
	struct sockaddr_rc remoteAddress = { 0 };
	socklen_t addressLength = sizeof(remoteAddress);
    BluetoothConnectionHandle c = -1;
    do {
    	c = accept(serverSocket, (struct sockaddr *) &remoteAddress, &addressLength);
    	//perror("a)");
    } while (c == -1);
    return c;
}


void SendStringTo(BluetoothConnectionHandle to, char * str) {
	write(to, str, strlen(str));
}

int ReceiveStringFrom(BluetoothConnectionHandle from, char * destination, int maxLength) {
	int readBytes = -1;
	do {
		readBytes = read(from, destination, maxLength);
	} while (readBytes == -1);
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
