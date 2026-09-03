#include <Arduino.h>

#include "SPOD_node.h"
#include "LCDDisplay.h"

#if LCDDisplay
  #include "LCD_PCD8544.h"

namespace {
  const unsigned long DISPLAY_UPDATE_INTERVAL_MS = 2000;

  unsigned long lastDisplayUpdate = 0;
  byte nextPage = 0;

  void clearDisplay() {
    LCDClear();
    LcdXY(0, 0);
  }

  void writeOutputValue(
    const char* label, const char* value, const char* units,
    bool showUnits, byte row) {
    LcdXY(0, row);
    LcdWriteString(label);
    LcdWriteString(value);
    if (showUnits) {
      LcdWriteString(units);
    }
  }

  void showLightPage(const LCDReadings& readings) {
    clearDisplay();
    writeOutputValue("Vis:", readings.visible, "", false, 0);
    writeOutputValue("IR:", readings.infrared, "", false, 2);
    writeOutputValue("UV:", readings.uvIndex, "", false, 4);
  }

  byte environmentReadingCount(const LCDReadings& readings) {
    return readings.phEnabled +
           readings.temperature1Enabled +
           readings.temperature2Enabled +
           readings.moistureEnabled +
           readings.co2Enabled;
  }

  void showEnvironmentPage(const LCDReadings& readings, byte page) {
    clearDisplay();
    const byte firstReading = page * 3;
    byte reading = 0;
    byte row = 0;

    if (readings.phEnabled) {
      if (reading >= firstReading && row < 6) {
        writeOutputValue("pH:", readings.ph, "", false, row);
        row += 2;
      }
      reading++;
    }
    if (readings.temperature1Enabled) {
      if (reading >= firstReading && row < 6) {
        writeOutputValue("T1:", readings.temperature1, "", false, row);
        row += 2;
      }
      reading++;
    }
    if (readings.temperature2Enabled) {
      if (reading >= firstReading && row < 6) {
        writeOutputValue("T2:", readings.temperature2, "", false, row);
        row += 2;
      }
      reading++;
    }
    if (readings.moistureEnabled) {
      if (reading >= firstReading && row < 6) {
        #if CALIBRATION
          writeOutputValue(
            "Soil:", readings.moisture, "%", readings.moistureNumeric, row);
        #else
          writeOutputValue("Soil:", readings.moisture, "", false, row);
        #endif
        row += 2;
      }
      reading++;
    }
    if (readings.co2Enabled && reading >= firstReading && row < 6) {
      writeOutputValue(
        "CO2:", readings.co2, "ppm", readings.co2Numeric, row);
    }
  }
}

void lcdDisplayBegin(const char* spodId) {
  pinMode(POWERpin, OUTPUT);
  digitalWrite(POWERpin, HIGH);

  pinMode(RST, OUTPUT);
  pinMode(ChipEnable, OUTPUT);
  pinMode(DCmodeSelect, OUTPUT);
  pinMode(DataIN, OUTPUT);
  pinMode(SerialCLK, OUTPUT);

  delay(10);
  LcdInit();
  LcdXY(5, 3);
  LcdWriteString("SPOD ");
  LcdWriteString(spodId);
  delay(1500);
  clearDisplay();

  // Allow the first loop pass to draw immediately.
  lastDisplayUpdate = millis() - DISPLAY_UPDATE_INTERVAL_MS;
}

void lcdDisplayUpdate(const LCDReadings& readings) {
  const unsigned long now = millis();
  if (now - lastDisplayUpdate < DISPLAY_UPDATE_INTERVAL_MS) {
    return;
  }
  lastDisplayUpdate = now;

  const byte environmentReadings = environmentReadingCount(readings);
  const byte environmentPages = (environmentReadings + 2) / 3;
  const bool hasEnvironment = environmentReadings > 0;

  if (!readings.lightEnabled && !hasEnvironment) {
    clearDisplay();
    LcdWriteString("No sensors");
    return;
  }

  const byte lightPages = readings.lightEnabled ? 1 : 0;
  const byte totalPages = lightPages + environmentPages;
  if (nextPage >= totalPages) {
    nextPage = 0;
  }

  if (readings.lightEnabled && nextPage == 0) {
    showLightPage(readings);
  } else if (readings.lightEnabled) {
    showEnvironmentPage(readings, nextPage - 1);
  } else {
    showEnvironmentPage(readings, nextPage);
  }

  nextPage = (nextPage + 1) % totalPages;
}
#endif // LCDDisplay
