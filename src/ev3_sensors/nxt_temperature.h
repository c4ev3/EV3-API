#ifndef EV3_API_NXT_TEMPERATURE_H
#define EV3_API_NXT_TEMPERATURE_H


typedef enum NXTTemperatureUnit {
    C,
    F
} NXTTemperatureUnit;

int ReadNXTTemperatureSensor(int port, NXTTemperatureUnit unit);


#endif //EV3_API_NXT_TEMPERATURE_H
