/*******************************************************************************
 * @file    light_sensor.cpp
 * @brief   Module to organize headers for SI1145, intemediate for Adafruit
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 12, 2026
 *
 * @log     Translated from evil linux-esque code (ALEX!!!)
 ******************************************************************************/
#include "light_sensor.h"

/**************************************************************************/
 /*!
 *    @brief  Starts the SI1145 (if SI1145 is enabled)
 *    @return True if the sensor initialized,
 *            False if it failed or SI1145 is disabled
 */
/**************************************************************************/
bool Light_Sensor::begin() {
  initialized = SI1145_sensor.begin();
  return initialized;
} //bool Light_Sensor::begin()

/**************************************************************************/
 /*!
 *    @brief  Reads visible, infrared & UV; no signal processing except UV index
 *    @return LightSensorSI1145_alldata with a valid flag per channel
 *            (all zero/false if not initialized or SI1145 is disabled)
 */
/**************************************************************************/
LightSensorReading Light_Sensor::read() {
  if (!initialized) {
    return SI1145_alldata;
  } //if (!initialized)

  SI1145_alldata.visibleValid = SI1145_sensor.readVisible(SI1145_alldata.visible);
  SI1145_alldata.infraredValid = SI1145_sensor.readIR(SI1145_alldata.infrared);
  SI1145_alldata.uvValid = SI1145_sensor.readUV(SI1145_alldata.rawUV);

  if (SI1145_alldata.uvValid) {
    SI1145_alldata.uvIndex = SI1145_alldata.rawUV / 100.0f;
  } //if (SI1145_alldata.uvValid)

  return SI1145_alldata;
} //LightSensorSI1145_alldata Light_Sensor::read() 
