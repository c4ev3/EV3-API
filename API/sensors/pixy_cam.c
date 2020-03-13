#include "inputs/ev3_input_iic.h"
#include "pixy_cam.h"

#define PIXY_CAM_SENSOR_IIC_ADDRESS 0x01 // TODO: Not tested yet

#define PIXY_CAM_BIGGEST_BLOB_RECTANGLE_REGISTER 0x50
#define PIXY_CAM_BIGGEST_BLOB_ANGLE_REGISTER 0x60

#define PIXY_CAM_ANGLE_CONSTANT 1.40625

SensorHandler * PixyCam = &(SensorHandler){
        .Init = initPixyCamSensor,
        .Exit = NULL,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};


bool initPixyCamSensor(int port) {
    initIICPort(port, PIXY_CAM_SENSOR_IIC_ADDRESS);
    return true;
}


int ReadPixyCamSensorBiggestBlobRectangle(int port, PixyRectangle* result) {
    DATA8 rectangleData[6];
    DATA8 angleData[1];
    if(readFromIIC(port, PIXY_CAM_BIGGEST_BLOB_RECTANGLE_REGISTER, rectangleData, 6) < 0) {
        return -1;
    }
    if(readFromIIC(port, PIXY_CAM_BIGGEST_BLOB_ANGLE_REGISTER, angleData, 1) < 0) {
        return -1;
    }

    result->signature  = ((rectangleData[0] & 0xFF) << 8) | (rectangleData[1] & 0xFF);
    result->x          = (rectangleData[2] & 0xFF);
    result->y          = (rectangleData[3] & 0xFF);
    result->width      = (rectangleData[4] & 0xFF);
    result->height     = (rectangleData[5] & 0xFF);
    result->angle      = (angleData[0] & 0xFF) * PIXY_CAM_ANGLE_CONSTANT;
    return 0;
}

