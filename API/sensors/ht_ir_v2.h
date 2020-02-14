#ifndef EV3_API_HT_IR_PRIVATE_H
#define EV3_API_HT_IR_PRIVATE_H

#include "ht_ir_v2.h"


bool initHTIrV2Sensor(int port);

void exitHTIrV2Sensor(int port);

int getHTIrV2RegisterForReadingMode(HTIrV2ReadingMode mode);

#endif
