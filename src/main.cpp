#include <Arduino.h>
#include "ioexpander.h"
#include "imu.h"
#include "tof.h"
#include "motors.h"
#include "pidstraight.h"
#include "pidrotate.h"

#include "Robot.h"
#include "MazeLogic.h"
#include "Flood.h"
#include "labyrinth.h"
#include "firefighter.h"

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    delay(2000);
    Serial.println("BOOT OK");

    for (int i = 0; i < 5; i++) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
    }
    Serial.println("LED TEST DONE");

    Wire.begin();
    Wire.setClock(400000);

    tofSetup();
    Serial.println("TOF SETUP DONE");
    motorSetup();
    Serial.println("MOTOR SETUP DONE");
}

int state = 0;

void loop() {
    if (state == 0) {
        Serial.println("loop alive");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        Serial.print(left());
        Serial.print(" | ");
        Serial.print(front());
        Serial.print(" | ");
        Serial.println(right());
        setLeftPWM(200);
        setRightPWM(200);
        if (front() < 50) {
            state = 1;
            setLeftPWM(0);
            setRightPWM(0);
        }
    } else if (state == 1) {
        setLeftPWM(0);
        setRightPWM(0);
        if (front() > 50) {
            state = 0;
            setLeftPWM(200);
            setRightPWM(200);
        }
    }

}
