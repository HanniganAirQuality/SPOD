/*******************************************************************************
 * @file    co2_sensor.h
 * @brief   Module to organize functions for the EZO CO2 circuit (I2C) on the SPOD
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 12, 2026
 *
 * @log     Translated from evil linux-esque code (ALEX!!!)
 ******************************************************************************/
#ifndef _CO2_SENSOR_H
#define _CO2_SENSOR_H

#include <Arduino.h>
#include <limits.h>
#include <stdlib.h>
#include <Wire.h>

#include "SPOD_node.h"

/****************** STRUCTS, OBJECTS ********************/
/*! CO2 reading (ppm) with the raw EZO response and validity flags */
struct Co2SensorReading {
  bool valid;                   // true if the EZO response parsed to a ppm value
  bool hasResponse;             // true if any response text was received
  int ppm;                      // CO2 concentration [ppm]
  unsigned char responseCode;   // EZO response code (1 = command succeeded)
  char rawResponse[20];         // raw EZO response text (null-terminated)
}; //struct Co2SensorReading

/****************** CLASSES ********************/
class Co2_Sensor {
  public:
    void begin();
    Co2SensorReading read();
  private:
    const unsigned long CO2_PRE_READ_DELAY_MS = 900;
    const unsigned long CO2_RESPONSE_DELAY_MS = 250;
    bool parsePpm(const char* response, int& ppm);
    Co2SensorReading Co2_alldata;
}; //class Co2_Sensor

#endif //_CO2_SENSOR_H