/*******************************************************************************
 * @file    co2_sensor.cpp
 * @brief   Module to organize functions for the EZO CO2 circuit (I2C) on the SPOD
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 12, 2026
 *
 * @log     Translated from evil linux-esque code (ALEX!!!)
 ******************************************************************************/
#include "co2_sensor.h"

/**************************************************************************/
 /*!
 *    @brief  Starts the CO2 sensor; the EZO CO2 circuit uses the Wire bus
 *            initialized by the main sketch, so this is empty
 */
/**************************************************************************/
void Co2_Sensor::begin() {
} //void Co2_Sensor::begin()

/**************************************************************************/
 /*!
 *    @brief  Sends a read command to the EZO CO2 circuit & receives the response
 *    @return Co2SensorReading with the response code, raw response text &
 *            parsed ppm (all zero/false if the I2C transaction fails)
 */
/**************************************************************************/
Co2SensorReading Co2_Sensor::read() {
  delay(CO2_PRE_READ_DELAY_MS);

  Wire.beginTransmission(CO2_I2C_ADDRESS);
  Wire.write('R');
  if (Wire.endTransmission() != 0) {
    return Co2_alldata;
  } //if (Wire.endTransmission() != 0)

  delay(CO2_RESPONSE_DELAY_MS);
  const uint8_t bytesReceived = Wire.requestFrom(
    static_cast<uint8_t>(CO2_I2C_ADDRESS), static_cast<uint8_t>(20),
    static_cast<uint8_t>(true));
  if (bytesReceived == 0 || !Wire.available()) {
    return Co2_alldata;
  } //if (bytesReceived == 0 || !Wire.available())

  Co2_alldata.responseCode = Wire.read();
  uint8_t index = 0;

  while (Wire.available() && index < sizeof(Co2_alldata.rawResponse) - 1) {
    const char incoming = static_cast<char>(Wire.read());
    if (incoming == '\0') {
      break;
    } //if (incoming == '\0')
    Co2_alldata.rawResponse[index++] = incoming;
  } //while (Wire.available() && index < sizeof(Co2_alldata.rawResponse) - 1)
  Co2_alldata.rawResponse[index] = '\0';
  Co2_alldata.hasResponse = index > 0;

  // EZO response code 1 means the command completed successfully.
  if (Co2_alldata.responseCode == 1 && Co2_alldata.hasResponse) {
    Co2_alldata.valid = parsePpm(Co2_alldata.rawResponse, Co2_alldata.ppm);
  } //if (Co2_alldata.responseCode == 1 && Co2_alldata.hasResponse)

  return Co2_alldata;
} //Co2SensorReading Co2_Sensor::read()

/**************************************************************************/
 /*!
 *    @brief  Parses an EZO response as a base-10 integer ppm value
 *    @param  response null-terminated response text
 *    @param  ppm      output ppm (only set if True is returned)
 *    @return True if the entire response is a base-10 integer in int range,
 *            False otherwise (e.g. status text such as "*WARM")
 */
/**************************************************************************/
bool Co2_Sensor::parsePpm(const char* response, int& ppm) {
  char* end = nullptr;
  const long parsed = strtol(response, &end, 10);

  // Require the entire response to be a base-10 integer. Unlike atoi(),
  // this keeps status text such as "*WARM" from becoming a valid zero.
  if (end == response || *end != '\0' || parsed < INT_MIN || parsed > INT_MAX) {
    return false;
  } //if (end == response || *end != '\0' || parsed < INT_MIN || parsed > INT_MAX)

  ppm = static_cast<int>(parsed);
  return true;
} //bool Co2_Sensor::parsePpm(const char* response, int& ppm)