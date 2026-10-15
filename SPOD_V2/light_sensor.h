#ifndef SPOD_LIGHT_SENSOR_H
#define SPOD_LIGHT_SENSOR_H

#include <stdint.h>

struct LightSensorReading {
  bool visibleValid;
  uint16_t visible;
  bool infraredValid;
  uint16_t infrared;
  bool uvValid;
  float uvIndex;
};

namespace LightSensor {
  bool begin();
  LightSensorReading read();
}

#endif // SPOD_LIGHT_SENSOR_H
