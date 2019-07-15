#ifndef EV3_API_HT_COMPASS_H
#define EV3_API_HT_COMPASS_H

typedef enum HTCompassMode {
    HTCompassCompass,
    HTCompassAngle
} HTCompassMode;

int ReadHTCompassSensor(int port, HTCompassMode mode);

void StartHTCompassCalibration(int port);

void StopHTCompassCalibration(int port);

#endif //EV3_API_HT_COMPASS_H
