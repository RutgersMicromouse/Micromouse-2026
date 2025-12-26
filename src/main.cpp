#include <Arduino.h>
#include "../include/common.h"
#include "../include/logger.h"

// put function declarations here:
int myFunction(int, int);

void setup() {
    Serial.begin(115200);
    initLogger();

    int result = myFunction(2, 3);
    LogInfo("Setup complete, result: %d", result);
}

void loop() {
    LogInfo("Loop running");
    delay(1000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}