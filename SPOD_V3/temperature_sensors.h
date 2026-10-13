/*******************************************************************************
 * @file    temperature_sensors.h
 * @brief   Module to organize functions for the two EZO RTD temperature
 *          circuits (I2C) on the SPOD
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 12, 2026
 *
 * @log     Translated from evil linux-esque code (ALEX!!!)
 ******************************************************************************/
#ifndef _TEMPERATURE_SENSORS_H
#define _TEMPERATURE_SENSORS_H

#include <Arduino.h>
#include <Ezo_i2c.h>
#include "SPOD_node.h"

/****************** STRUCTS, OBJECTS ********************/
/*! Temperature readings (2 EZO RTD circuits), each with a validity flag */
struct TemperatureSensorReadings {
  // Temperature Probe 1
  bool temperature1Valid;
  float temperature1;
  // Temperature Probe 2
  bool temperature2Valid;
  float temperature2;
}; //struct TemperatureSensorReadings

/****************** CLASSES ********************/
class Temperature_Sensors {
  public:
    void begin();
    TemperatureSensorReadings read();
  private:
    bool receiveReading(Ezo_board& sensor, float& value);
    Ezo_board temperature1{TEMP1_I2C_ADDRESS, "Temp1"};
    Ezo_board temperature2{TEMP2_I2C_ADDRESS, "Temp2"};
    TemperatureSensorReadings temperature_data; 
}; //class Temperature_Sensors

#endif //_TEMPERATURE_SENSORS_H