#ifndef SPOD_CO2_SENSOR_H
#define SPOD_CO2_SENSOR_H

struct Co2SensorReading {
  bool valid;
  bool hasResponse;
  int ppm;
  unsigned char responseCode;
  char rawResponse[20];
};

namespace Co2Sensor {
  void begin();
  Co2SensorReading read();
}

#endif // SPOD_CO2_SENSOR_H
