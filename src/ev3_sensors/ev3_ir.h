#ifndef EV3_API_EV3_IR_H
#define EV3_API_EV3_IR_H

#define EV3_IR_CHANNELS 4

typedef struct EV3IrSeekResult {
    int directions[EV3_IR_CHANNELS];
    int distances[EV3_IR_CHANNELS];
} EV3IrSeekResult;

EV3IrSeekResult ReadEV3IrSensorSeek (int port);

int ReadEV3IrSensorProximity (int port);

// TODO: Missing remote mode
//int ReadEV3IrSensorRemote (int port);


#endif //EV3_API_EV3_IR_H
