#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

struct LCDReadings {
  bool phEnabled;
  const char* ph;

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
