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
  // Initialize Serial and I2C
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // LED ON (active low on some Teensys/Xiaos)

  delay(2000); // Delay to open serial monitor
  digitalWrite(LED_BUILTIN, HIGH); // LED OFF

  // 1. Setup hardware components
  imuSetup();
  tofSetup();
  delay(50);
  motorSetup();

  // 2. Debug: Scan I2C bus to verify all sensors/motor drivers are alive
  Serial.println("Scanning I2C bus...");
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Found I2C device at 0x");
      Serial.println(address, HEX);
    }
  }
  Serial.println("Scan done.");

  // 3. Initial movement: move to middle of starting cell from back wall
  pidForward(50);

  // 4. Mode Selection based on switches
  if(isFirefighter()) { 
    Serial.println("Firefighter mode");
    // firefighterSetup();
    // firefighterLoop();
    while(1); // Do nothing for now until firefighter is implemented
    return;
  } 
  
  if(isSpeedrun()) {
    Serial.println("Lightning McQueen mode");
    initialize(); 
    delay(100);
    speedrun();
    return;
  }

  if(isLabyrinth()) {
    Serial.println("Labyrinth mode");
    labyrinthLoop();
    return;    
  }

  // 5. Default: Exploration mode
  Serial.println("Exploration mode");
  initialize();
  runMaze('c');
}

void loop() {
  // Most logic is handled in the Loop functions above (labyrinthLoop, etc.)
  // If those return, we just pulse a heartbeat here.
  delay(2000);
  Serial.println("System Standby - Main loop.");
  
  // Debugging sensors if needed:
  // Serial.print("F: "); Serial.println(front());
  // Serial.print("Angle: "); Serial.println(angle());
}