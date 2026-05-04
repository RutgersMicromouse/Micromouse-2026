#include <stdio.h>
#include "vl6180x.h"
#include "i2c.h"

#define VL6180X_INIT_TIMEOUT 50000

i2c_result vl6180x_init(void) {
    uint8_t reset = 0;

    // Check if sensor is at default address
    if (i2cread16(VL6180X_ADDR, SYSTEM__FRESH_OUT_OF_RESET, &reset) != i2c_success) {
        // Not at 0x29, check if it's at 0x2B (ADDR pin pulled high)
        if (i2cread16(VL6180X_ADDR_ALT, SYSTEM__FRESH_OUT_OF_RESET, &reset) == i2c_success) {
            printf("Error: VL6180X found at 0x2B instead of 0x29. ADDR pin may be pulled HIGH.\n\r");
            return i2c_error;
        }
        printf("Error: VL6180X not found at 0x29 or 0x2B.\n\r");
        return i2c_error;
    }

    if (reset == 1) {
        // Mandatory private registers (from ST app note AN4545)
        i2cwrite16(VL6180X_ADDR, 0x0207, 0x01);
        i2cwrite16(VL6180X_ADDR, 0x0208, 0x01);
        i2cwrite16(VL6180X_ADDR, 0x0096, 0x00);
        i2cwrite16(VL6180X_ADDR, 0x0097, 0xFD);
        i2cwrite16(VL6180X_ADDR, 0x00E3, 0x00);
        i2cwrite16(VL6180X_ADDR, 0x00E4, 0x04);
        i2cwrite16(VL6180X_ADDR, 0x00E5, 0x02);
        i2cwrite16(VL6180X_ADDR, 0x00E6, 0x01);
        i2cwrite16(VL6180X_ADDR, 0x00E7, 0x03);
        i2cwrite16(VL6180X_ADDR, 0x00F5, 0x02);
        i2cwrite16(VL6180X_ADDR, 0x00D9, 0x05);
        i2cwrite16(VL6180X_ADDR, 0x00DB, 0xCE);
        i2cwrite16(VL6180X_ADDR, 0x00DC, 0x03);
        i2cwrite16(VL6180X_ADDR, 0x00DD, 0xF8);
        i2cwrite16(VL6180X_ADDR, 0x009F, 0x00);
        i2cwrite16(VL6180X_ADDR, 0x00A3, 0x3C);
        i2cwrite16(VL6180X_ADDR, 0x00B7, 0x00);
        i2cwrite16(VL6180X_ADDR, 0x00BB, 0x3C);
        i2cwrite16(VL6180X_ADDR, 0x00B2, 0x09);
        i2cwrite16(VL6180X_ADDR, 0x00CA, 0x09);
        i2cwrite16(VL6180X_ADDR, 0x0198, 0x01);
        i2cwrite16(VL6180X_ADDR, 0x01B0, 0x17);
        i2cwrite16(VL6180X_ADDR, 0x01AD, 0x00);
        i2cwrite16(VL6180X_ADDR, 0x00FF, 0x05);
        i2cwrite16(VL6180X_ADDR, 0x0100, 0x05);
        i2cwrite16(VL6180X_ADDR, 0x0199, 0x05);
        i2cwrite16(VL6180X_ADDR, 0x01A6, 0x1B);
        i2cwrite16(VL6180X_ADDR, 0x01AC, 0x3E);
        i2cwrite16(VL6180X_ADDR, 0x01A7, 0x1F);
        i2cwrite16(VL6180X_ADDR, 0x0030, 0x00);

        // Public registers - recommended settings
        i2cwrite16(VL6180X_ADDR, 0x0011, 0x10); // Enables polling for New Sample ready
        i2cwrite16(VL6180X_ADDR, 0x010A, 0x30); // Set ALS integration time to 100ms
        i2cwrite16(VL6180X_ADDR, 0x003F, 0x46); // Set ALS gain to 1
        i2cwrite16(VL6180X_ADDR, 0x0031, 0xFF); // Set max convergence time
        i2cwrite16(VL6180X_ADDR, 0x0040, 0x63); // Set ALS interrupt thresholds
        i2cwrite16(VL6180X_ADDR, 0x002E, 0x01); // Disable SNR check

        // Clear fresh out of reset bit
        i2cwrite16(VL6180X_ADDR, SYSTEM__FRESH_OUT_OF_RESET, 0x00);
    }

    return i2c_success;
}

i2c_result vl6180x_read_distance(uint8_t *distance_mm) {
    uint8_t status = 0;
    uint32_t timeout = VL6180X_INIT_TIMEOUT;
    i2c_result res;

    // 1. Start a single-shot range measurement
    res = i2cwrite16(VL6180X_ADDR, SYSRANGE__START, 0x01);
    if (res != i2c_success) return res;

    // 2. Poll the interrupt status register with timeout
    do {
        res = i2cread16(VL6180X_ADDR, RESULT__INTERRUPT_STATUS_GPIO, &status);
        if (res != i2c_success) return res;
        if (--timeout == 0) return i2c_timeout;
    } while ((status & 0x07) != 0x04);

    // 3. Read the distance
    res = i2cread16(VL6180X_ADDR, RESULT__RANGE_VAL, distance_mm);
    if (res != i2c_success) return res;

    // 4. Clear the interrupt
    res = i2cwrite16(VL6180X_ADDR, SYSTEM__INTERRUPT_CLEAR, 0x01);
    return res;
}
