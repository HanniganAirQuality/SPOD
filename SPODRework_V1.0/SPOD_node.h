/*******************************************************************************
 * @file    SPOD_node.h
 * @brief   
 *
 * @author  Sascha Fowler 
 * @date    July 28, 2026
 * @log     Creating SPOD nodes for code redesign from V1, Testing with Orange SPOD - RTC Update, Serial/File Write, SI1145 Function
 * @TBD     
******************************************************************************/
#ifndef _SPOD_NODE_H
#define _SPOD_NODE_H


#define SD_ENABLED            1
#define SERIAL_ENABLED        1
#define CALIBRATION           0

#define RTC_UPDATE            0 // IF you have to update RTC, please upload after with a 0

#define SI1145                1
#define LCDDisplay            1

#define MOISTURE_ENABLED               0

#define TEMPERATURE_ONE_ENABLED        0
#define TEMPERATURE_TWO_ENABLED        0
#define CO2_ENABLED                    0

const int PM_RX = 2;
const int PM_TX = 3;

//default I2C ID number for EZO Co2 sensor and soil moisture sensors
#define I2C_ADDRESS   105
//SD Card Settings
#define SD_CS         10

char spodID[] = "Green_5";

#endif // _SPOD_NODE_H