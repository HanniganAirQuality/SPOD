#ifndef POD_CONFIG_H
#define POD_CONFIG_H

// Update these values when preparing firmware for a different pod.
#define POD_NAME "SPOD YELLOW3"

#define SD_CHIP_SELECT_PIN 10

#define SOIL_SENSOR_PIN A0
#define SOIL_POWER_PIN 7

#define PH_I2C_ADDRESS 99
#define TEMP1_I2C_ADDRESS 102
#define TEMP2_I2C_ADDRESS 103
#define CO2_I2C_ADDRESS 105

#define CO2_CAL_SLOPE 0.9211
#define CO2_CAL_OFFSET 153.71

#define SOIL_MOISTURE_SLOPE -0.1642036125
#define SOIL_MOISTURE_OFFSET 100

// Set to 1, upload once to set the RTC from compile time, then set back to 0.
#define SET_RTC_FROM_COMPILE_TIME 0

#endif
