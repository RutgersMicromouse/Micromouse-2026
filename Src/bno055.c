#include "bno055.h"
#include "i2c.h"
#include<stdio.h>

bno055_results bno055_init(void) {
    uint8_t chip_id;
    i2c_result result = i2cread(0x28, 0x00, &chip_id); // read the chip id register to check if the sensor is connected
    if (result != i2c_success) {
        return BNO055_ERROR;
    }
    if (chip_id != 0xA0) {
        printf("BNO055 initialization failed: Incorrect chip ID (0x%02X)\n", chip_id);
        return BNO055_ERROR;
    }
    return BNO055_OK;
}