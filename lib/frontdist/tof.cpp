#include "tof.h"
#include <Wire.h>
#include <VL53L4CD.h>

// Teensy 4.0 Pins
#define PIN_FRONT_XSHUT 2
#define PIN_LEFT_XSHUT  3
#define PIN_RIGHT_XSHUT 4

VL53L4CD tofFront;
VL53L4CD tofLeft;
VL53L4CD tofRight;

int16_t frontLast = -1;
int16_t leftLast  = -1;
int16_t rightLast = -1;

void tofSetup() {
    // 1. Force all sensors into hardware reset (LOW)
    pinMode(PIN_FRONT_XSHUT, OUTPUT);
    pinMode(PIN_LEFT_XSHUT, OUTPUT);
    pinMode(PIN_RIGHT_XSHUT, OUTPUT);
    
    digitalWrite(PIN_FRONT_XSHUT, LOW);
    digitalWrite(PIN_LEFT_XSHUT, LOW);
    digitalWrite(PIN_RIGHT_XSHUT, LOW);
    delay(50); 

    Wire.begin();
    Wire.setClock(400000); 

    // 2. Wake and Init Front
    digitalWrite(PIN_FRONT_XSHUT, HIGH);
    delay(10);
    tofFront.setBus(&Wire);
    if (tofFront.init()) {
        tofFront.setAddress(0x30);
        tofFront.startContinuous();
    }

    // 3. Wake and Init Left
    digitalWrite(PIN_LEFT_XSHUT, HIGH);
    delay(10);
    tofLeft.setBus(&Wire);
    if (tofLeft.init()) {
        tofLeft.setAddress(0x32);
        tofLeft.startContinuous();
    }

    // 4. Wake and Init Right
    digitalWrite(PIN_RIGHT_XSHUT, HIGH);
    delay(10);
    tofRight.setBus(&Wire);
    if (tofRight.init()) {
        tofRight.setAddress(0x34);
        tofRight.startContinuous();
    }
}

int16_t readSensor(VL53L4CD &sensor, int16_t &last) {
    // read() is the standard Pololu method
    uint16_t dist = sensor.read();
    
    if (!sensor.timeoutOccurred()) {
        last = (int16_t)dist;
    }
    return last;
}

int16_t front() { return readSensor(tofFront, frontLast); }
int16_t left()  { return readSensor(tofLeft, leftLast); }
int16_t right() { return readSensor(tofRight, rightLast); }