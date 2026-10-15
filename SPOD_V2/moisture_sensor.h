#ifndef SPOD_MOISTURE_SENSOR_H
#define SPOD_MOISTURE_SENSOR_H

struct MoistureSensorReading {
  bool valid;
  int rawValue;
};

namespace MoistureSensor {
  void begin();
  MoistureSensorReading read();
}

#endif // SPOD_MOISTURE_SENSOR_H
