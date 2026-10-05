#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

// Change only this line when swapping displays.
#define SPOD_DISPLAY_PCD8544 1
#define SPOD_DISPLAY_ST7735  2
#define SPOD_DISPLAY_TYPE SPOD_DISPLAY_ST7735

// SPOD Shield V5.1 display-header connections.
#define SPOD_DISPLAY_CS_PIN    6
#define SPOD_DISPLAY_DC_PIN    8
#define SPOD_DISPLAY_RESET_PIN 9
#define SPOD_DISPLAY_MOSI_PIN  11
#define SPOD_DISPLAY_SCK_PIN   13

// Verified settings for the 1.44-inch ST7735 through the shield dividers.
#define SPOD_ST7735_SPI_FREQUENCY 1000000UL
#define SPOD_ST7735_ROTATION      2

struct LCDReadings {
  bool lightEnabled;
  const char* visible;
  const char* infrared;
  const char* uvIndex;

  bool moistureEnabled;
  bool moistureNumeric;
  const char* moisture;

  bool temperature1Enabled;
  const char* temperature1;
  bool temperature2Enabled;
  const char* temperature2;

  bool co2Enabled;
  bool co2Numeric;
  const char* co2;
};

void lcdDisplayBegin(const char* spodId);
void lcdDisplayUpdate(const LCDReadings& readings);

#endif // LCD_DISPLAY_H
