/*******************************************************************************
 * @file    SPOD_node.h
 * @brief   Use for ReWork V1.2
 *
 * @author  Sascha Fowler 
 * @date    August 7, 2026
 * @log     
 * @TBD     
******************************************************************************/
#ifndef _SPOD_NODE_H
#define _SPOD_NODE_H


#define SD_ENABLED            1
#define SERIAL_ENABLED        1
#define SERIAL_DEBUG_ENABLED  1
#define CALIBRATION           1

#define RTC_UPDATE            1 // IF you have to update RTC, please upload after with a 0

#define SI1145                1
#define LCDDisplay            1

#define MOISTURE_ENABLED               1

#define TEMPERATURE_ENABLED            1
#define CO2_ENABLED                    1

const int PM_RX = 2;
const int PM_TX = 3;

// Sensor pins and I2C addresses
#define SOIL_SENSOR_PIN  A0
#define SOIL_POWER_PIN   7
#define TEMP1_I2C_ADDRESS 102
#define TEMP2_I2C_ADDRESS 103
#define CO2_I2C_ADDRESS  105
//SD Card Settings
#define SD_CS         10

// This is the only POD-specific selection needed by the firmware.
// Calibration uses the first character for the color and the final character
// for the POD number, following the YPOD calibration pattern.
const char spodID[] = "SPOD_Y3";
const char firmwareVersion[] = "SPOD_V1.2";
const char calID_letter = spodID[0];
const char calID_number = spodID[sizeof(spodID) - 2];

#endif // _SPOD_NODE_H
