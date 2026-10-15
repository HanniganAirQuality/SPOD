#ifndef SPOD_CALIBRATION_H
#define SPOD_CALIBRATION_H

#include <string.h>

#include "SPOD_node.h"

namespace SpodCalibration {
  inline float calibrateTemperature1(float rawTemperature) {
    return rawTemperature;
  }

  inline float calibrateTemperature2(float rawTemperature) {
    return rawTemperature;
  }

  // CO2 equations fitted by SPOD-Calibration/calibrate_spod.py against the
  // colocated LI-840A. Training used the first 80% of July 9-27, 2026 and
  // validation used the final 20% chronologically.
  inline int calibrateCo2(
    int rawCo2, float temperature1, bool temperature1Valid) {
    // Reserved for future models that include temperature. The selected
    // GREEN1 and YELLOW3 models are CO2-only.
    (void)temperature1;
    (void)temperature1Valid;

    float calibrated = static_cast<float>(rawCo2);
    if (strcmp(spodID, "GREEN1") == 0) {
      calibrated = (0.612990628f * rawCo2) + 346.073773f;
    } else if (strcmp(spodID, "YELLOW3") == 0) {
      calibrated = (0.789818177f * rawCo2) - 63.7789205f;
    }

    // ORANGE has false zero readings, YELLOW2 has no overlapping calibration
    // data, and unknown pods all retain their raw CO2 value.
    if (calibrated <= 0.0f) {
      return 0;
    }
    return static_cast<int>(calibrated + 0.5f);
  }

  inline int calibrateSoilMoisture(int rawSoilMoisture) {
    return rawSoilMoisture;
  }
}

#endif // SPOD_CALIBRATION_H
