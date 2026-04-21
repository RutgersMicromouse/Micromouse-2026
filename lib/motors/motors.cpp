#include "motors.h"

MotoronI2C mc;

// Teensy 4.0 handles these pins automatically with interrupts
Encoder encLeft(1, 2);
Encoder encRight(3, 4);

void motorSetup() {
    mc.reinitialize();
    mc.disableCrc();
    mc.clearResetFlag();
    
    // Note: With the PaulStoffregen/Encoder library, you do NOT 
    // need attachInterrupt() or manual update functions. 
    // It works automatically on Pins 1,2,3,4.
}

// These are no longer needed for the Encoder library, 
// but we keep them empty if other files expect them to exist.
void updateRightEncoder() {}
void updateLeftEncoder() {}

void setLeftPWM(int PWM) {
    mc.setSpeed(1, -PWM);
}

void setRightPWM(int PWM) {
    mc.setSpeed(2, -PWM);
}