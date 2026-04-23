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

  pidForward(100);

  Serial.println("LED TEST DONE");
}

void loop() {
  Serial.println("loop alive");
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(1000);
}
