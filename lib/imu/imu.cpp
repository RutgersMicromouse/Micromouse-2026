#include "imu.h"

Adafruit_BNO08x bno;

sh2_SensorValue_t sensorValue;

void imuSetup() {
  if (!bno.begin_I2C()) {
    Serial.println("No BNO08x detected");
    while (1);
  }

  bno.enableReport(SH2_ROTATION_VECTOR);
}

double angle() {
  if (bno.getSensorEvent(&sensorValue)) {
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR) {

      float qw = sensorValue.un.rotationVector.real;
      float qx = sensorValue.un.rotationVector.i;
      float qy = sensorValue.un.rotationVector.j;
      float qz = sensorValue.un.rotationVector.k;

      // Convert quaternion → yaw
      double yaw = atan2(2.0 * (qw*qz + qx*qy),
                         1.0 - 2.0 * (qy*qy + qz*qz));

      return yaw * 180.0 / PI;
    }
  }
  return 0;
}