#include "light_sensor.h"
#include "SPOD_node.h"

#if SI1145
  #include "Adafruit_SI1145.h"

namespace {
  Adafruit_SI1145 sensor;
  bool initialized = false;
}
#endif

bool LightSensor::begin() {
  #if SI1145
    initialized = sensor.begin();
    return initialized;
  #else
    return false;
  #endif
}

LightSensorReading LightSensor::read() {
  LightSensorReading reading = {};

  #if SI1145
    if (!initialized) {
      return reading;
    }

    uint16_t uv = 0;
    reading.visibleValid = sensor.readVisible(reading.visible);
    reading.infraredValid = sensor.readIR(reading.infrared);
    reading.uvValid = sensor.readUV(uv);

    if (reading.uvValid) {
      reading.uvIndex = uv / 100.0f;
    }
  #endif

  return reading;
}
