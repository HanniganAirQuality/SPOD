#include <Arduino.h>
#include <SPI.h>

#include "SPOD_node.h"
#include "LCDDisplay.h"

#if LCDDisplay

#if SPOD_DISPLAY_TYPE == SPOD_DISPLAY_PCD8544
  // Map the legacy driver names onto the V5.1 display header.
  #define RST          SPOD_DISPLAY_RESET_PIN
  #define ChipEnable   SPOD_DISPLAY_CS_PIN
  #define DCmodeSelect SPOD_DISPLAY_DC_PIN
  #define DataIN       SPOD_DISPLAY_MOSI_PIN
  #define SerialCLK    SPOD_DISPLAY_SCK_PIN
  #include "LCD_PCD8544.h"
#elif SPOD_DISPLAY_TYPE == SPOD_DISPLAY_ST7735
  #include <Adafruit_GFX.h>
  #include <Adafruit_ST7735.h>
#else
  #error "Select SPOD_DISPLAY_PCD8544 or SPOD_DISPLAY_ST7735"
#endif

namespace {
  const unsigned long DISPLAY_UPDATE_INTERVAL_MS = 2000;
  unsigned long lastDisplayUpdate = 0;
  byte nextPage = 0;

#if SPOD_DISPLAY_TYPE == SPOD_DISPLAY_PCD8544

  void prepareDisplayBus() {
    // The old LCD uses software SPI. Temporarily release the hardware SPI
    // peripheral used by the SD card, then restore it after drawing.
    digitalWrite(SD_CS, HIGH);
    SPI.end();
    pinMode(RST, OUTPUT);
    pinMode(ChipEnable, OUTPUT);
    pinMode(DCmodeSelect, OUTPUT);
    pinMode(DataIN, OUTPUT);
    pinMode(SerialCLK, OUTPUT);
    digitalWrite(ChipEnable, HIGH);
    digitalWrite(SerialCLK, LOW);
  }

  void finishDisplayUpdate() {
    digitalWrite(ChipEnable, HIGH);
    digitalWrite(SD_CS, HIGH);
    SPI.begin();
  }

  void screenBegin(const char* spodId) {
    prepareDisplayBus();
    delay(10);
    LcdInit();
    LcdXY(5, 3);
    LcdWriteString(spodId);
    delay(1500);
    LCDClear();
    finishDisplayUpdate();
  }

  void clearDisplay() {
    prepareDisplayBus();
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

  void writeMessage(const char* message) {
    LcdXY(0, 0);
    LcdWriteString(message);
  }

#else // SPOD_DISPLAY_ST7735

  // The library normally initializes at 32 MHz. Override begin() so the
  // complete initialization and all drawing use the verified 1 MHz clock.
  class SlowST7735 : public Adafruit_ST7735 {
  public:
    SlowST7735(int8_t cs, int8_t dc, int8_t rst)
      : Adafruit_ST7735(cs, dc, rst) {}

    void begin(uint32_t frequency = 0) override {
      (void)frequency;
      Adafruit_ST77xx::begin(SPOD_ST7735_SPI_FREQUENCY);
    }
  };

  SlowST7735 tft(
    SPOD_DISPLAY_CS_PIN, SPOD_DISPLAY_DC_PIN,
    SPOD_DISPLAY_RESET_PIN);

  uint16_t labelColor(const char* label) {
    if (label[0] == 'C') return ST77XX_YELLOW;
    if (label[0] == 'S') return ST77XX_GREEN;
    if (label[0] == 'U') return ST77XX_MAGENTA;
    if (label[0] == 'I') return ST77XX_RED;
    return ST77XX_CYAN;
  }

  void screenBegin(const char* spodId) {
    pinMode(SPOD_DISPLAY_CS_PIN, OUTPUT);
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SPOD_DISPLAY_CS_PIN, HIGH);
    digitalWrite(SD_CS, HIGH);

    tft.initR(INITR_144GREENTAB);
    tft.setRotation(SPOD_ST7735_ROTATION);
    tft.setTextWrap(false);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(2);
    tft.setCursor(10, 42);
    tft.print(spodId);
    delay(1500);
    tft.fillScreen(ST77XX_BLACK);
    digitalWrite(SPOD_DISPLAY_CS_PIN, HIGH);
  }

  void clearDisplay() {
    digitalWrite(SD_CS, HIGH);
    tft.fillScreen(ST77XX_BLACK);
  }

  void finishDisplayUpdate() {
    digitalWrite(SPOD_DISPLAY_CS_PIN, HIGH);
    digitalWrite(SD_CS, HIGH);
  }

  void writeOutputValue(
    const char* label, const char* value, const char* units,
    bool showUnits, byte row) {
    const byte slot = row / 2;
    const int16_t labelY = 2 + slot * 42;
    const int16_t valueY = labelY + 12;

    tft.setTextColor(labelColor(label), ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(2, labelY);
    tft.print(label);

    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(2);
    tft.setCursor(2, valueY);
    tft.print(value);
    if (showUnits) {
      tft.print(units);
    }
  }

  void writeMessage(const char* message) {
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(2);
    tft.setCursor(2, 52);
    tft.print(message);
  }

#endif

  void showLightPage(const LCDReadings& readings) {
    clearDisplay();
    writeOutputValue("Vis:", readings.visible, "", false, 0);
    writeOutputValue("IR:", readings.infrared, "", false, 2);
    writeOutputValue("UV:", readings.uvIndex, "", false, 4);
  }

  byte environmentReadingCount(const LCDReadings& readings) {
    return readings.temperature1Enabled +
           readings.temperature2Enabled +
           readings.moistureEnabled +
           readings.co2Enabled;
  }

  void showEnvironmentPage(const LCDReadings& readings, byte page) {
    clearDisplay();
    const byte firstReading = page * 3;
    byte reading = 0;
    byte row = 0;

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
            "Soil Moisture %:", readings.moisture, "%", readings.moistureNumeric, row);
        #else
          writeOutputValue("Raw SM:", readings.moisture, "", false, row);
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
  screenBegin(spodId);
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
    writeMessage("No sensors");
    finishDisplayUpdate();
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

  finishDisplayUpdate();
  nextPage = (nextPage + 1) % totalPages;
}

#endif // LCDDisplay
