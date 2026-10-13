/*******************************************************************************
 * @file    moisture_sensor.h
 * @brief   Module to organize functions for the analog soil moisture sensor
 *          (powered from SOIL_POWER_PIN) on the SPOD
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 12, 2026
 *
 * @log     Translated from evil linux-esque code (ALEX!!!)
 ******************************************************************************/
#ifndef _MOISTURE_SENSOR_H
#define _MOISTURE_SENSOR_H

#include <Arduino.h>
#include "SPOD_node.h"

/****************** STRUCTS, OBJECTS ********************/
/*! Soil moisture reading (raw ADC value) with a validity flag */
struct MoistureSensorReading {
  bool valid;            // true if a reading was taken
  int rawValue;          // raw analogRead() value from SOIL_SENSOR_PIN
}; //struct MoistureSensorReading

/****************** CLASSES ********************/
class Moisture_Sensor {
  public:
    void begin();
    MoistureSensorReading read();
  private:
    MoistureSensorReading Moisture_alldata;
}; //class Moisture_Sensor

#endif //_MOISTURE_SENSOR_H