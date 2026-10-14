/*******************************************************************************
 * @file    lcd_module.cpp
 * @brief   Module to organize functions for ST7735 LCD display
 *
 * @cite    Adapted LCDDisplay.cpp (SPOD)
 *
 * @author  Alexander Hansen
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    October 7, 2026
 *
 * @log     Restructured into lab module format (LCD_Module class)
 *          Removed PCD8544 support (ST7735 only)
 ******************************************************************************/
#include "lcd_module.h"

/**************************************************************************/
 /*!
 *    @brief  "begins" the ST7735 display & shows the SPOD ID splash screen
 *    @return true/false - did display begin? (driver gives no failure
 *            feedback, so this is true once init completes)
 */
/**************************************************************************/
bool LCD_Module::begin() {
  tft.initR(INITR_144GREENTAB);
  tft.startWrite();
  tft.setRotation(SPOD_ST7735_ROTATION);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 42);
  tft.print(spodID);
  tft.endWrite();
  delay(1000);  // splash screen stays up for 1 second
  #if SERIAL_DEBUG_ENABLED
    Serial.println("LCD Attempted");
  #endif //SERIAL_DEBUG_ENABLED
  // drawScreen_2();  // boxes & labels are drawn once; update() only redraws the values
  tft.fillScreen(ST77XX_BLACK);
  return true;
} //bool LCD_Module::begin(const char* spod_id)

/**************************************************************************/
 /*!
 *    @brief  Update Data 
 */
/**************************************************************************/
void LCD_Module::update(const char* clock, float t1, float t2, int co2_cal, int sm_cal, int visible, int infrared, int uv_raw) {
  // (void)visible;  // not shown on this screen layout (yet), discarding
  tft.startWrite();
  tft.setCursor(0, 0);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  // // tft.setTextWrap(false);
  /* CLOCK UPDATE */
  // tft.fillRect(69, 7, 48, 8, ST77XX_BLACK);
  // delay(100);
  tft.println(clock);
  tft.endWrite();
  delay(100);
  tft.startWrite();
  tft.setCursor(0, 20);
  // tft.setTextWrap(false);
  // tft.setTextSize(2);
  // tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.print(uv_raw);
  tft.print(","); 
  tft.print(infrared);
  tft.print(",");
  tft.println(visible);
  tft.endWrite();
  delay(100);
  tft.startWrite();
  // tft.
  tft.setCursor(0, 50);
  // tft.setTextWrap(false);
  // tft.setTextSize(2);
  // tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  // delay(100);
  if(t1==-99)   {
    tft.print("--");
  } else {
    tft.print(t1, 1);
  }
  tft.print(",");
  if(t2==-99)   {
    tft.println("--");
  } else {
    tft.println(t2, 1);
  }
  tft.endWrite();
  // delay(100);
  // tft.startWrite();
  // tft.setCursor(10, 80);
  // tft.println(co2_cal);
  // // delay(100);

  // tft.endWrite();

  // tft.println(t2);
  // tft.println(infrared);
  // tft.drawString(clock);
  // tft.setCursor(10, 28);
  // tft.print(uv_raw);
  // updateIntVars(12, 28, 3, uv_raw);
  /* Variable Updates! */
  // tft.setTextSize(2);
  // updateIntVars(28, 22, 3, uv_raw);
  // updateIntVars(82, 22, 3, infrared);
  // updateFloatVars(36, 44, 6, t1);
  // updateFloatVars(36, 62, 6, t2);
  // updateIntVars(38, 84, 5, co2_cal);
  // // updateIntVars(74, 107, 3, sm_cal);
  // tft.endWrite();
  delay(2000);
  
  // tft.startWrite();
  // tft.setRotation(SPOD_ST7735_ROTATION);
  // tft.fillScreen(ST77XX_BLACK);
  // tft.setTextSize(2);
  
  // tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    // tft.setCursor(4, 4);
    // tft.print("00:00:00");
  
    // tft.setTextSize(2);
  // tft.setTextSize(2);
  // tft.setCursor(29, 28);
  // tft.print("000");
  // // string 2
  // tft.setCursor(40, 47);
  // tft.print("000.00");
  // // string 2
  // tft.setCursor(83, 28);
  // tft.print("000");
  // // string 2
  // tft.setCursor(40, 65);
  // tft.print("000.00");
  // // string 2
  // tft.setCursor(40, 84);
  // tft.print("00000");
  // // string 2
  // tft.setCursor(75, 104);
  // tft.print("100");


  // tft.setTextSize(1);
  // tft.fillRect(69, 7, 48, 8, ST77XX_BLACK);   // erase old clock
  // tft.setCursor(69, 7);
  // tft.print(clock);                           // draw new cloc


  // tft.endWrite();
  // delay(1000);
  // #if SERIAL_DEBUG_ENABLED
  //   Serial.println("LCD Attempted");
  // #endif //SERIAL_DEBUG_ENABLED
  // drawScreen_1();  // boxes & labels are drawn once; update() only redraws the values

} //void LCD_Module::update()

/**************************************************************************/
 /*!
 *    @brief  Erases the old whole-number value, then draws the new one
 *            (or "--" if there is no reading). Big text only.
 *    @param  x, y   top-left corner of the value on the screen
 *    @param  chars  how many characters wide the value spot is
 *    @param  value  number to show
 */
/**************************************************************************/
void LCD_Module::updateIntVars(int x, int y, int chars, int value) {
  // tft.fillRect(x, y, chars * 12, 14, ST77XX_BLACK);  // big text is 12 wide & 14 tall per character
  tft.setCursor(x, y);
  if (value == NO_READING) {
    tft.print("--");
  } else {
    tft.print(value);
  } //if (value == NO_READING)
} //void LCD_Module::updateIntVars()


/**************************************************************************/
 /*!
 *    @brief  Same as updateIntVars() but for decimal values (2 decimal places)
 */
/**************************************************************************/
void LCD_Module::updateFloatVars(int x, int y, int chars, float value) {
  tft.fillRect(x, y, chars * 12, 14, ST77XX_BLACK);  // big text is 12 wide & 14 tall per character
  tft.setCursor(x, y);
  if (value == NO_READING) {
    tft.print("--");
  } else {
    tft.print(value);
  } //if (value == NO_READING)
} //void LCD_Module::updateFloatVars()


/**************************************************************************/
 /*!
 *    @brief  Draws background layout for the data  
 */
/**************************************************************************/
void LCD_Module::drawScreen_1() {
  tft.startWrite();
  tft.fillScreen(0x0);
  // rect 15
  tft.drawRect(8, 81, 111, 20, 0xFFFF);
  // rect 15
  tft.drawRect(8, 19, 111, 20, 0xFFFF);
  // rect 15
  tft.drawRect(8, 103, 111, 21, 0xFFFF);
  // rect 15
  tft.drawRect(51, 4, 13, 13, 0xFFFF);
  // rect 15
  tft.fillRect(54, 7, 7, 7, 0x4D6A);
  // rect 15
  tft.drawRect(66, 4, 53, 13, 0xFFFF);
  // rect 15
  tft.drawRect(8, 4, 41, 13, 0xFFFF);
  // rect 10
  tft.drawRect(8, 41, 111, 38, 0xFFFF);
  // string 2
  tft.setTextSize(1);
  tft.setTextColor(0xFFFF);
  tft.setTextWrap(false);
  tft.setCursor(65, 29);
  tft.print("IR:");
  // string 2
  tft.setCursor(11, 69);
  tft.print("T2:");
  // string 2
  tft.setCursor(11, 91);
  tft.print("CO2:");
  // string 2
  tft.setCursor(11, 114);
  tft.print("Moisture:");
  // string 2
  tft.setCursor(11, 106);
  tft.print("Soil");
  // string 2
  tft.setCursor(11, 7);
  tft.print(spodID);
  // string 2
  tft.setCursor(99, 82);
  tft.print("ppm");
  // string 2
  tft.setCursor(111, 107);
  tft.print("%");
  // string 2
  tft.setCursor(111, 65);
  tft.print("C");
  // string 2
  tft.setCursor(111, 47);
  tft.print("C");
  // string 2
  tft.setCursor(11, 29);
  tft.print("UV:");
  // string 2
  tft.setCursor(11, 51);
  tft.print("T1:");
  tft.endWrite();
} //void LCD_Module::drawScreen_1()


void LCD_Module::drawScreen_2() {
  tft.startWrite();
  tft.fillScreen(0x0);
  // rect 15
  tft.drawRect(9, 7, 111, 18, 0x4D6A);
  // rect 15
  tft.drawRect(9, 81, 111, 20, 0xFFFF);
  // rect 15
  tft.drawRect(9, 25, 111, 20, 0xFFFF);
  // rect 15
  tft.drawRect(9, 100, 111, 21, 0xFFFF);
  // rect 10
  tft.drawRect(9, 44, 111, 38, 0xFFFF);
  // string 2
  tft.setTextSize(1);
  tft.setTextColor(0xFFFF);
  tft.setTextWrap(false);
  tft.setCursor(66, 35);
  tft.print("IR:");
  // string 2
  tft.setCursor(12, 72);
  tft.print("T2:");
  // string 2
  tft.setCursor(12, 91);
  tft.print("CO2:");
  // string 2
  tft.setCursor(12, 111);
  tft.print("Moisture:");
  // string 2
  tft.setCursor(12, 103);
  tft.print("Soil");
  // string 2
  tft.setCursor(100, 84);
  tft.print("ppm");
  // string 2
  tft.setCursor(112, 104);
  tft.print("%");
  // string 2
  tft.setCursor(112, 65);
  tft.print("C");
  // string 2
  tft.setCursor(112, 47);
  tft.print("C");
  // string 2
  tft.setCursor(12, 35);
  tft.print("UV:");
  // string 2
  tft.setCursor(12, 54);
  tft.print("T1:");
  tft.endWrite();
  // string 2
}