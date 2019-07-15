#ifndef EV3_API_HT_IR_H
#define EV3_API_HT_IR_H

// TODO: single function with modes

int ReadHTIrSensorAC(int port);

int ReadHTIrSensorDC(int port);

/* Utility functions */
//void writeIicRequestToHtIRToReadRegister(int sensorPort, int registerAddress);

#endif //EV3_API_HT_IR_H
