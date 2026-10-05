#include <Arduino.h>
#include <limits.h>
#include <stdlib.h>
#include <Wire.h>

#include "co2_sensor.h"
#include "SPOD_node.h"

namespace {
  const unsigned long CO2_PRE_READ_DELAY_MS = 900;
  const unsigned long CO2_RESPONSE_DELAY_MS = 250;

  bool parsePpm(const char* response, int& ppm) {
    char* end = nullptr;
    const long parsed = strtol(response, &end, 10);

    // Require the entire response to be a base-10 integer. Unlike atoi(),
    // this keeps status text such as "*WARM" from becoming a valid zero.
    if (end == response || *end != '\0' || parsed < INT_MIN || parsed > INT_MAX) {
      return false;
    }

    ppm = static_cast<int>(parsed);
    return true;
  }
}

void Co2Sensor::begin() {
  // The EZO CO2 circuit uses the Wire bus initialized by the main sketch.
}

Co2SensorReading Co2Sensor::read() {
  Co2SensorReading reading = {};

  #if CO2_ENABLED
    delay(CO2_PRE_READ_DELAY_MS);

    Wire.beginTransmission(CO2_I2C_ADDRESS);
    Wire.write('R');
    if (Wire.endTransmission() != 0) {
      return reading;
    }

    delay(CO2_RESPONSE_DELAY_MS);
    const uint8_t bytesReceived = Wire.requestFrom(
      static_cast<uint8_t>(CO2_I2C_ADDRESS), static_cast<uint8_t>(20),
      static_cast<uint8_t>(true));
    if (bytesReceived == 0 || !Wire.available()) {
      return reading;
    }

    reading.responseCode = Wire.read();
    uint8_t index = 0;

    while (Wire.available() && index < sizeof(reading.rawResponse) - 1) {
      const char incoming = static_cast<char>(Wire.read());
      if (incoming == '\0') {
        break;
      }
      reading.rawResponse[index++] = incoming;
    }
    reading.rawResponse[index] = '\0';
    reading.hasResponse = index > 0;

    // EZO response code 1 means the command completed successfully.
    if (reading.responseCode == 1 && reading.hasResponse) {
      reading.valid = parsePpm(reading.rawResponse, reading.ppm);
    }
  #endif

  return reading;
}
