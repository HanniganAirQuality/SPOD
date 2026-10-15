/*******************************************************************************
 * @file    SPOD_node.h
 * @brief   Use for ReWork V2
 *
 * @author  Sascha Fowler & Percy Smith
 * @date    October 2, 2026
 * @log     Changed addresses, variables, removed bulk 
 * @TBD     
******************************************************************************/
#ifndef _SPOD_NODE_H
#define _SPOD_NODE_H

#define SERIAL_ENABLED          1
  #define SERIAL_DEBUG_ENABLED  0
#define CALIBRATION             1
#define SCRIPT_VERSION          1

#define RTC_UPDATE            0 // IF you have to update RTC, please upload after with a 0

// Comms Config
#define LCD_ENABLED           1
#define SD_ENABLED            1

// Sensor Config
#define MOISTURE_ENABLED      1
#define TEMPERATURE_ENABLED   1
#define CO2_ENABLED           1
#define SI1145_ENABLED        1

// Pin definitions
#define SM_SENSOR             A0
#define SM_POWER              7
#define LCD_CS                6
#define LCD_RST               9 //attempted to pass -1 but failed by whitescreening
#define LCD_DC                8    
#define SD_CS                 10           
// We really need to add an LED indicator for the SD card - We can look at external mount LEDs

// I2C Address
#define SI1145_I2C_ADDRESS    0x60 //Declared internally to Adafruit_SI1145 as SI1145_ADDR
#define TEMP1_I2C_ADDRESS     0x66 //I haven't seen I2c formatted like this? Check scanner
#define TEMP2_I2C_ADDRESS     0x67
// #define RTC_ADDRESS        0x68
#define CO2_I2C_ADDRESS       0x69


#define SD_CONFIG SdSpiConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(4), &SPI)

// Verified settings for the 1.44-inch ST7735 through the shield dividers.
#define SPOD_ST7735_SPI_FREQUENCY 8000000UL //idk why it looksj like this but it works, just think of it as 8MHz
#define SPOD_ST7735_ROTATION      2

// This is the only POD-specific selection needed by the firmware.
// Calibration uses the first character for the color and the final character
// for the POD number, following the YPOD calibration pattern.
const char spodID[] = "SPODY3"; //SPOD + Color-Letter + Num
  const char calID_letter = spodID[4];
  const char calID_number = spodID[5];

const char firmwareVersion[] = "SPOD_V3";

// Error Reading ; we will use this to print NULL 
const int NO_READING = -99;

#endif // _SPOD_NODE_H
