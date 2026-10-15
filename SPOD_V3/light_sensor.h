/*******************************************************************************
 * @file    light_sensor.h
 * @brief   Module to organize functions for SI1145, intemediate for Adafruit
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 12, 2026
 *
 * @log     Translated from evil linux-esque code (ALEX!!!)
 ******************************************************************************/
#ifndef _LIGHT_SENSOR_H
#define _LIGHT_SENSOR_H

#include <stdint.h>
#include "Adafruit_SI1145.h"
#include "SPOD_node.h"

/****************** STRUCTS, OBJECTS ********************/
/*! Light reading (visible, infrared, UV index), each with a validity flag */
struct LightSensorReading {
  // Visible Light Reading 
  bool visibleValid;
  uint16_t visible;
  // InfraRed Light Reading (IR)
  bool infraredValid;
  uint16_t infrared;
  // UltraViolet Light Reading (UV)
  bool uvValid;
  uint16_t rawUV;        
  float uvIndex;         // raw UV reading / 100
}; //struct LightSensorReading

/****************** FUNCTIONS ********************/
class Light_Sensor {
  public: 
    bool begin();
    LightSensorReading read();
  private: 
    bool initialized = false;
    Adafruit_SI1145 SI1145_sensor;
    LightSensorReading SI1145_alldata; 
}; //class LightSensor

#endif //_LIGHT_SENSOR_H