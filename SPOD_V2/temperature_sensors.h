#ifndef SPOD_TEMPERATURE_SENSORS_H
#define SPOD_TEMPERATURE_SENSORS_H

struct TemperatureSensorReadings {
  bool temperature1Valid;
  float temperature1;
  bool temperature2Valid;
  float temperature2;
};

namespace TemperatureSensors {
  void begin();
  TemperatureSensorReadings read();
}

#endif // SPOD_TEMPERATURE_SENSORS_H
