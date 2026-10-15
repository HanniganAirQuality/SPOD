#include <Arduino.h>

#include "moisture_sensor.h"
#include "SPOD_node.h"

void MoistureSensor::begin() {
  #if MOISTURE_ENABLED
    pinMode(SOIL_POWER_PIN, OUTPUT);
    digitalWrite(SOIL_POWER_PIN, LOW);
  #endif
}

MoistureSensorReading MoistureSensor::read() {
  MoistureSensorReading reading = {};

  #if MOISTURE_ENABLED
    digitalWrite(SOIL_POWER_PIN, HIGH);
    delay(10);
    reading.rawValue = analogRead(SOIL_SENSOR_PIN);
    digitalWrite(SOIL_POWER_PIN, LOW);
    reading.valid = true;
  #endif

  return reading;
}
