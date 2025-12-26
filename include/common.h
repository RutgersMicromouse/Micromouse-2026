#ifndef COMMON_H
#define COMMON_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <Arduino.h>
#include <stdlib.h>
#include <stdio.h>

#define assert_value(expr,message) \
    do { \
        if (!(expr)) { \
            Serial.printf("Assertion failed: %s, file %s, line %d\n", message, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#endif // COMMON_H