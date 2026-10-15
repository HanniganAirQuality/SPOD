/*******************************************************************************
 * @file    calibration.cpp
 * @brief   Calibration function architecture and equations for CO2 and soil
 *          moisture (SM) calibrations
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
#include "calibration.h"

/**************************************************************************/
 /*!
 *    @brief calls calibration functions and returns struct w/ variables
 */
/**************************************************************************/
calOutput Cal::calibrate (int co2, int sm) {
  calOutput out = {}; // creates an instance of calOutput struct (zeroed)
  #if CALIBRATE_CO2 // Conditional
    out.CO2_ = calibrate_co2 (co2);
  #endif
  #if CALIBRATE_SM // Conditional
    out.SM_ = calibrate_sm (sm);
  #endif
  return out;
}

/**************************************************************************/
 /*!
 *    @brief  Stores the CO2 calibration equations
 *            Equations fitted by SPOD-Calibration/calibrate_spod.py against
 *            the colocated LI-840A. Training used the first 80% of July 9-27,
 *            2026 and validation used the final 20% chronologically. The G1
 *            and Y3 models are CO2-only (no temperature term).
 *            Pods with no case below keep their raw CO2 value (previously
 *            ORANGE had false zero readings and YELLOW2 had no overlapping
 *            calibration data).
 */
/**************************************************************************/
int Cal::calibrate_co2 (int co2) {
  float co2_cal; // Stores calibrated CO2 value (rounded to int on return)
  switch (calID_letter) { // Switch statement for letter in spodID
    case 'G':
      switch (calID_number) { // Switch statement for number in spodID
        case '1':
          co2_cal = (0.204292677f * co2) + 243.747414f;
          break;
        default:
          co2_cal = co2; // Default = original signal
      }
      break;
    case 'O':
      switch (calID_number) { // Switch statement for number in spodID
        case '1':
          co2_cal = (0.538121277f * co2) + 8.77086603f;
          break;
        default:
          co2_cal = co2; // Default = original signal
      }
      break;
    case 'P':
      switch (calID_number) { // Switch statement for number in spodID
        case '1':
          co2_cal = (0.327509667f * co2) + 277.685169f;
          break;
        default:
          co2_cal = co2; // Default = original signal
      }
      break;
    case 'Y':
      switch (calID_number) { // Switch statement for number in spodID
        case '2':
          co2_cal = (0.814183960f * co2) - 382.856975f;
          break;
        case '3':
          co2_cal = (0.789818177f * co2) - 63.7789205f;
          break;
        default:
          co2_cal = co2; // Default = original signal
      }
      break;
    default:
      #if SERIAL_ENABLED
        // Serial.println("No CO2 calibration data for this pod.");
      #endif
      co2_cal = co2; // Default = original signal
  }
  if (co2_cal <= 0.0f) { // conditional for negative values
    return 0;
  }
  return static_cast<int>(co2_cal + 0.5f);
}

/**************************************************************************/
 /*!
 *    @brief  Stores the soil moisture (SM) calibration equations
 *            (no pod equations yet; add cases like calibrate_co2)
 */
/**************************************************************************/
int Cal::calibrate_sm (int sm) {
  int sm_cal; // Stores calibrated soil moisture value
  switch (calID_letter) { // Switch statement for letter in spodID
    case 'G':
      switch (calID_number) { // Switch statement for number in spodID
        case '1':
          sm_cal = (-0.0024*sm + 1.48)*100 ;
          break;
        default:
          sm_cal = sm; // Default = original signal
      }
      break;
    case 'O':
      switch (calID_number) { // Switch statement for number in spodID
        case '1':
          sm_cal = (-0.0024*sm + 1.53)*100 ;
          break;
        default:
          sm_cal = sm; // Default = original signal
      }
      break;
    case 'P':
      switch (calID_number) { // Switch statement for number in spodID
        case '1':
          sm_cal = (-0.0025*sm + 1.56)*100;
          break;
        default:
          sm_cal = sm; // Default = original signal
      }
      break;
    case 'Y':
      switch (calID_number) { // Switch statement for number in spodID
        case '2':
          sm_cal = (-0.0023*sm + 1.39)*100 ;
          break;
        case '3':
          sm_cal = (-0.0017*sm + 1.07)*100 ;
          break;
        default:
          sm_cal = sm; // Default = original signal
      }
      break;
    default:
      #if SERIAL_ENABLED
        // Serial.println("No soil moisture calibration data for this pod.");
      #endif
      sm_cal = sm; // Default = original signal
  }
  return sm_cal;
}
