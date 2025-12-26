#include "../include/logger.h"
#include "../include/common.h"

// Logging queue
QueueHandle_t logQueue;

// Logging task
static void loggingTask(void *pvParameters) {
    char buffer[256];
    while (true) {
        if (xQueueReceive(logQueue, &buffer, portMAX_DELAY)) {
            Serial.println(buffer);
        }
    }
}

// Initialize logger
void initLogger() {
    logQueue = xQueueCreate(10, sizeof(char[256]));
    xTaskCreate(loggingTask, "Logging", 2048, NULL, 1, NULL);
}

// Function to log asynchronously
void asyncLog(const char *message) {
    xQueueSend(logQueue, message, 0);
}