#ifndef SPOD_PH_SENSOR_H
#define SPOD_PH_SENSOR_H

struct PhSensorReading {
  bool valid;
  float value;
};

namespace PhSensor {
  void begin();
  PhSensorReading read(bool hasTemperature, float temperature);
}

#endif // SPOD_PH_SENSOR_H
