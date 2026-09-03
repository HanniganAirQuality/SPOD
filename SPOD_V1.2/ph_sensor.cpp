#include <Arduino.h>

#include "ph_sensor.h"
#include "SPOD_node.h"

#if PH_ENABLED
  #include <Ezo_i2c.h>

namespace {
  Ezo_board sensor(PH_I2C_ADDRESS, "PH");
}
#endif

void PhSensor::begin() {
  // EZO pH circuits do not require a separate initialization command.
}

PhSensorReading PhSensor::read(bool hasTemperature, float temperature) {
  PhSensorReading reading = {};

  #if PH_ENABLED
    if (hasTemperature) {
      sensor.send_read_with_temp_comp(temperature);
    } else {
      sensor.send_read_cmd();
    }

    delay(1000);
    sensor.receive_read_cmd();
    if (sensor.get_error() == Ezo_board::SUCCESS) {
      reading.value = sensor.get_last_received_reading();
      reading.valid = reading.value > -1000.0f;
    }
  #else
    (void)hasTemperature;
    (void)temperature;
  #endif

  return reading;
}
