#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <ctype.h>
#include <contrib/bluetooth/bluetooth.h>
#include <contrib/bluetooth/hci.h>
#include <contrib/bluetooth/hci_lib.h>
#include <contrib/bluetooth/rfcomm.h>

#include "ev3_bluetooth.h"

/**
 * Channel 1 is already taken by the UI process of the robot, so we use 2.
 */
#define DEFAULT_C4EV3_RFCOMM_CHANNEL    2
#define MAX_CONNECTIONS                 7
#define SHORT_BLUETOOTH_ADDRESS_LENGTH  sizeof("000000000000")

#define KNOWN_BLUETOOTH_NAMES_FILE_NAME_LENGTH 256
#define KNOWN_BLUETOOTH_NAMES_FILE_ROW_LENGTH  256

void BluetoothInit () { }

static BluetoothConnectionHandle connections[MAX_CONNECTIONS];
static int currentConnections = 0;

static void addConnectionToList (BluetoothConnectionHandle c) {
    if (currentConnections > MAX_CONNECTIONS) {
        return;
    }
    connections[currentConnections++] = c;
}

static void removeConnectionFromList (BluetoothConnectionHandle c) {
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


static bool isBluetoothAddress(const char * nameOrAddress);
static bool findAddressByBluetoothName(const char * name, char * address);

/**
 * Connects to the specified bluetooth address. If the connection fails, a new
 * attempt is tried after one second.
 * @param address
 * @param attempts number of attempts to retry connection
 * @return connection handle or -1 if the connection couldn't be established
 */
static BluetoothConnectionHandle connectByBluetoothAddress(const char * address, int attempts);

BluetoothConnectionHandle ConnectTo(const char * nameOrAddress, int attempts) {
    char address[BLUETOOTH_ADDRESS_LENGTH];
    if (isBluetoothAddress(nameOrAddress)) {
        strncpy(address, nameOrAddress, BLUETOOTH_ADDRESS_LENGTH);
        address[BLUETOOTH_ADDRESS_LENGTH - 1] = '\0';
    } else {
        bool found = findAddressByBluetoothName(nameOrAddress, address);
        if (!found) {
            return -1;
        }
    }
    BluetoothConnectionHandle c = connectByBluetoothAddress(address, attempts);
    addConnectionToList(c);
    return c;
}

static bool isBluetoothAddress(const char * nameOrAddress) {
    return bachk(nameOrAddress) == 0;
}

static FILE * openKnownBluetoothNamesFile(void);
static void getLocalBluetoothAddress (char * address);
static void getKnownBluetoothNamesFile (const char * localBluetoothAddress, char * fileName);
static bool getAddressFromKnownBluetoothNamesFileRow(const char *addressAndName, char * address);
static void getNameFromKnownBluetoothNamesFileRow(const char *addressAndName, char * name);

/**
 * To find the address of a device, given its name, we read a file that the
 * firmware of the robot writes repeatedly. This file contains a line for each
 * known bluetooth device. Each line contains the bluetooth address and name.
 * We don't use the BlueZ C API directly because that part of the API causes the
 * UI of the robot to crash.
 * @param nameToFind name of the device for which you need the address
 * @param foundAddress output string where the address will be written to
 * @return true if an address was found and it has been written to the output
 *         string, false otherwise
 */
static bool findAddressByBluetoothName(const char * nameToFind, char * foundAddress) {
    FILE * namesFile = openKnownBluetoothNamesFile();
    if (namesFile == NULL) {
        perror("open known bluetooth names file");
        return false;
    }
    char addressAndName[KNOWN_BLUETOOTH_NAMES_FILE_ROW_LENGTH];
    bool found = false;
    while (!found && fgets(addressAndName, KNOWN_BLUETOOTH_NAMES_FILE_ROW_LENGTH, namesFile)) {
        char address[BLUETOOTH_ADDRESS_LENGTH];
        if(!getAddressFromKnownBluetoothNamesFileRow(addressAndName, address)) {
            continue;
        }

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

static FILE * openKnownBluetoothNamesFile() {
    char localBluetoothAddress[BLUETOOTH_ADDRESS_LENGTH];
    getLocalBluetoothAddress(localBluetoothAddress);

    char knownBluetoothNamesFile[KNOWN_BLUETOOTH_NAMES_FILE_NAME_LENGTH];
    getKnownBluetoothNamesFile(localBluetoothAddress, knownBluetoothNamesFile);

    return fopen(knownBluetoothNamesFile, "r");
}

static void getLocalBluetoothAddress (char * address) {
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

static void getKnownBluetoothNamesFile (const char * localBluetoothAddress, char * fileName) {
    strcpy(fileName, "/mnt/ramdisk/bluetooth/");
    strcat(fileName, localBluetoothAddress);
    strcat(fileName, "/names");
}

static bool getAddressFromKnownBluetoothNamesFileRow(const char *addressAndName, char * address) {
    strncpy(address, addressAndName, BLUETOOTH_ADDRESS_LENGTH - 1);
    address[BLUETOOTH_ADDRESS_LENGTH - 1] = 0;
    return isBluetoothAddress(address);
}

static void getNameFromKnownBluetoothNamesFileRow(const char *addressAndName, char * name) {
    char* end = stpncpy(name, &addressAndName[18], MAX_BLUETOOTH_NAME_LENGTH);
    *(end - 1) = 0; // remove new line
}

static bool waitAsyncConnection (int socket);

static BluetoothConnectionHandle connectByBluetoothAddress(const char * address, int attempts) {
    struct sockaddr_rc remoteAddress;
    remoteAddress.rc_family = AF_BLUETOOTH;
    remoteAddress.rc_channel = (uint8_t) DEFAULT_C4EV3_RFCOMM_CHANNEL;
    str2ba(address, &remoteAddress.rc_bdaddr);
    while (attempts > 0) {
        int socketToServer = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (socketToServer != -1) {
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
            close(socketToServer);
        }

        // try to connect again later
        sleep(1);
        attempts--;
    }
    return -1;
}

static bool waitAsyncConnection (int socket) {
    struct pollfd toMonitor;
    toMonitor.fd = socket;
    toMonitor.events = POLLOUT;
    int poolResult;
    do {
        poolResult = poll(&toMonitor, 1, -1);
    } while (poolResult == -1 && errno == EINTR);

    if (poolResult == -1) {
        return false;
    }

    int optval;
    socklen_t optLength = sizeof(optval);
    int getSockOptResult = getsockopt(socket, SOL_SOCKET, SO_ERROR, &optval, &optLength);
    return (getSockOptResult == 0 && optval == 0);
}

static bool isServerStarted (void);
static int startServer (void);
static BluetoothConnectionHandle acceptIncomingConnection (void);

BluetoothConnectionHandle WaitConnection() {
    if(!isServerStarted()) {
        if(startServer() != 0) {
            return -1;
        }
    }
    return acceptIncomingConnection();
}


static int serverSocket = -1;

static bool isServerStarted(){
    return serverSocket > 0;
}

static int bindServerSocket (void);

static int startServer () {
    serverSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (serverSocket == -1) {
        return -1;
    }
    if(bindServerSocket() != 0) {
        return -1;
    }
    return listen(serverSocket, 1); // TODO: do we keep 1 as backlog?
}

static int bindServerSocket () {
    struct sockaddr_rc listenAddress;
    listenAddress.rc_family = AF_BLUETOOTH;
    listenAddress.rc_bdaddr = *BDADDR_ANY;
    listenAddress.rc_channel = DEFAULT_C4EV3_RFCOMM_CHANNEL;
    return bind(serverSocket, (struct sockaddr *) &listenAddress, sizeof(listenAddress));
}

static BluetoothConnectionHandle acceptIncomingConnection () {
    BluetoothConnectionHandle c;
    do {
        c = accept(serverSocket, NULL, NULL);
    } while (c == -1 && errno == EINTR);
    return c;
}


int SendStringTo(BluetoothConnectionHandle to, const char * str) {
    return write(to, str, strlen(str));
}

int ReceiveStringFrom(BluetoothConnectionHandle from, char * buffer, int bufferLength) {
    int readBytes;
    do {
        readBytes = read(from, buffer, bufferLength - 1);
    } while (readBytes == -1 && errno == EINTR);
    buffer[readBytes] = 0;
    return readBytes;
}

void DisconnectFrom(BluetoothConnectionHandle from) {
    close(from);
    removeConnectionFromList(from);
}

static void disconnectAllConnections(void);
static void stopServer(void);

void BluetoothExit() {
    disconnectAllConnections();
    stopServer();
}

static void disconnectAllConnections() {
    while (currentConnections > 0) {
        DisconnectFrom(connections[0]);
    }
}

static void stopServer() {
    close(serverSocket);
    serverSocket = -1;
}
