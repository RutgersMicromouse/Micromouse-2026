#ifndef bno055_H_
#define bno055_H_

#include <stdint.h>
#include "i2c.h"


typedef struct {
    float heading;
    float roll;
    float pitch;
} bno055_euler_t;

i2c_result bno055_init(void);
i2c_result bno055_get_angles(bno055_euler_t *angles);



 #endif /* bno055_H_ */
