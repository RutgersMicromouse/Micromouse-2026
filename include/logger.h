#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <string.h>

void initLogger();
void asyncLog(const char *message);

#define LogInfo(format, ...) \
    do { \
        const char *filename = strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__; \
        char buf[256]; \
        snprintf(buf, sizeof(buf), "[%s: %s]: " format, filename, __FUNCTION__, ##__VA_ARGS__); \
        asyncLog(buf); \
    } while (0)

#endif // LOGGER_H