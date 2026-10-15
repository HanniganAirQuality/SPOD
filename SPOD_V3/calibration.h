/*******************************************************************************
 * @file    calibration.h
 * @brief   header file for calibration.cpp, stores struct and class.
 *          Conditionals for which variables to run calibration on
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 12, 2026
 *
 * @log     Translated from evil linux-esque code (ALEX!!!)
 *          Converted SPOD calibration from inline namespace to Cal class to
 *          match YPOD calibration. Pod IDs are now Letter + number (was
 *          Color + number, e.g. GREEN1 --> G1). Temperature calibration
 *          removed (readings already accurate); CO2 & SM (soil moisture) only
******************************************************************************/
#ifndef _CALIBRATION_H
#define _CALIBRATION_H

#define CALIBRATE_CO2   1 // Calibrates CO2 sensor
#define CALIBRATE_SM    1 // Calibrates soil moisture (SM) sensor

#include "SPOD_node.h" // include statement

struct calOutput { // struct for which var to calibrate
  int CO2_;
  int SM_;
};

class Cal {
  public: // public variables
    calOutput calibrate (int co2, int sm);

  private: // private variables
    int calibrate_co2 (int co2);
    int calibrate_sm (int sm);
};

#endif // _CALIBRATION_H