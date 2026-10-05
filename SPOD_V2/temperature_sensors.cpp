#include <Arduino.h>

#include "temperature_sensors.h"
#include "SPOD_node.h"

#if TEMPERATURE_ENABLED
  #include <Ezo_i2c.h>

namespace {
  Ezo_board temperature1(TEMP1_I2C_ADDRESS, "Temp1");
  Ezo_board temperature2(TEMP2_I2C_ADDRESS, "Temp2");

  bool receiveReading(Ezo_board& sensor, float& value) {
    sensor.receive_read_cmd();
    if (sensor.get_error() != Ezo_board::SUCCESS) {
      return false;
    }

    value = sensor.get_last_received_reading();
    return value > -1000.0f;
  }
}
#endif

void TemperatureSensors::begin() {
  // EZO RTD circuits do not require a separate initialization command.
}

TemperatureSensorReadings TemperatureSensors::read() {
  TemperatureSensorReadings readings = {};

  #if TEMPERATURE_ENABLED
    temperature1.send_read_cmd();
    delay(1000);
    readings.temperature1Valid = receiveReading(
      temperature1, readings.temperature1);

    temperature2.send_read_cmd();
    delay(1000);
    readings.temperature2Valid = receiveReading(
      temperature2, readings.temperature2);
  #endif

  return readings;
}
