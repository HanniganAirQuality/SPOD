/*******************************************************************************
 * @file    temperature_sensors.cpp
 * @brief   Module to organize functions for the two EZO RTD temperature
 *          circuits (I2C) on the SPOD
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 12, 2026
 *
 * @log     Translated from evil linux-esque code (ALEX!!!)
 ******************************************************************************/
#include "temperature_sensors.h"

/**************************************************************************/
 /*!
 *    @brief  Starts the temperature sensors; EZO RTD circuits do not
 *            require a separate initialization command, so this is empty
 */
/**************************************************************************/
void Temperature_Sensors::begin() {
} //void Temperature_Sensors::begin()

/**************************************************************************/
 /*!
 *    @brief  Receives the reading from an EZO circuit after send_read_cmd()
 *    @param  sensor EZO board that was already sent a read command
 *    @param  value  output reading (only meaningful if True is returned)
 *    @return True if the read succeeded and value > -1000,
 *            False otherwise
 */
/**************************************************************************/
bool Temperature_Sensors::receiveReading(Ezo_board& sensor, float& value) {
  sensor.receive_read_cmd();
  if (sensor.get_error() != Ezo_board::SUCCESS) {
    return false;
  } //if (sensor.get_error() != Ezo_board::SUCCESS)

  value = sensor.get_last_received_reading();
  return value > -1000.0f;
} //bool Temperature_Sensors::receiveReading(Ezo_board& sensor, float& value)

/**************************************************************************/
 /*!
 *    @brief  Reads both EZO RTD circuits (1 s wait after each read command)
 *    @return TemperatureSensorReadings with a valid flag per circuit
 */
/**************************************************************************/
TemperatureSensorReadings Temperature_Sensors::read() {
  temperature1.send_read_cmd();
  delay(1000);
  temperature_data.temperature1Valid = receiveReading(temperature1, temperature_data.temperature1);

  temperature2.send_read_cmd();
  delay(1000);
  temperature_data.temperature2Valid = receiveReading(temperature2, temperature_data.temperature2);

  return temperature_data;
} //TemperatureSensorReadings Temperature_Sensors::read()