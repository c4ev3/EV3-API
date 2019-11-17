#include "../ev3_inputs/ev3_input_iic.h"
#include "pixy_cam.h"

#include <stdio.h>

#define PIXY_CAM_SENSOR_IIC_ADDRESS 0x01 // TODO: Verify

#define PIXY_CAM_BIGGEST_BLOB_RECTANGLE_REGISTER 0x50
#define PIXY_CAM_BIGGEST_BLOB_ANGLE_REGISTER 0x60

#define PIXY_CAM_ANGLE_CONSTANT 1.40625

SensorHandler * PixyCam = &(SensorHandler){
        .Init = initPixyCamSensor,
        .Exit = exitPixyCamSensor,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};


bool initPixyCamSensor(int port) {
    initIICPort(port, PIXY_CAM_SENSOR_IIC_ADDRESS);
    return true;
}


PixyRectangle ReadPixyCamSensorBiggestBlobRectangle(int port) {
    DATA8 rectangleData[6];
    DATA8 angleData[1];
    readFromIIC(port, PIXY_CAM_BIGGEST_BLOB_RECTANGLE_REGISTER, rectangleData, 6);
    readFromIIC(port, PIXY_CAM_BIGGEST_BLOB_ANGLE_REGISTER, angleData, 1);
    return (PixyRectangle) {
            .signature  = ((rectangleData[0] & 0xFF) << 8) | (rectangleData[1] & 0xFF),
            .x          = (rectangleData[2] & 0xFF),
            .y          = (rectangleData[3] & 0xFF),
            .width      = (rectangleData[4] & 0xFF),
            .height     = (rectangleData[5] & 0xFF),
            .angle      = (angleData[0] & 0xFF) * PIXY_CAM_ANGLE_CONSTANT
    };
}

void exitPixyCamSensor(int port) {

}
