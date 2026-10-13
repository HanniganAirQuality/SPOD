/*******************************************************************************
 * @file    moisture_sensor.cpp
 * @brief   Module to organize functions for the analog soil moisture sensor
 *          (powered from SOIL_POWER_PIN) on the SPOD
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 12, 2026
 *
 * @log     Translated from evil linux-esque code (ALEX!!!)
 ******************************************************************************/
#include "moisture_sensor.h"

/**************************************************************************/
 /*!
 *    @brief  Starts the moisture sensor; sets SOIL_POWER_PIN as an output
 *            and leaves it LOW (sensor unpowered) until read()
 */
/**************************************************************************/
void Moisture_Sensor::begin() {
  pinMode(SM_POWER, OUTPUT);
  digitalWrite(SM_POWER, LOW);
} //void Moisture_Sensor::begin()

/**************************************************************************/
 /*!
 *    @brief  Powers the sensor, waits 10 ms, reads SOIL_SENSOR_PIN, then
 *            powers the sensor back down; no signal processing!!
 *    @return MoistureSensorReading with the raw ADC value & valid = true
 */
/**************************************************************************/
MoistureSensorReading Moisture_Sensor::read() {
  Moisture_alldata = {};

  digitalWrite(SM_POWER, HIGH);
  delay(10);
  Moisture_alldata.rawValue = analogRead(SM_SENSOR);
  digitalWrite(SM_POWER, LOW);
  Moisture_alldata.valid = true;

  return Moisture_alldata;
} //MoistureSensorReading Moisture_Sensor::read()