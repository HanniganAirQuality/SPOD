/*******************************************************************************
 * @file    SPOD_V2.ino
 * @brief   Coordinates SPOD sensors, calibration, display, and data logging.
 *
 * @author  Sascha Fowler
 *          Alexander Hansen
 * @cite    Original SPOD.ino, Ankit Kharkanis
 *          Previous editing by Daniel Carranza-Valenzuela
 * @editor  Percy Smith 
 * @date    October 7, 2026
 * @TBD     Finish formatting in-line with YPOD ino & update calls from V2
 ******************************************************************************/

#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
#include <string.h>
#include <Wire.h>

/*  Header Files  */
#include "SPOD_node.h"

/*  Conditional Declarations  */
#if SI1145_ENABLED
  #include "light_sensor.h"
  Light_Sensor light_sensor;
#endif  //SI1145_ENABLED

#if MOISTURE_ENABLED
  #include "moisture_sensor.h"
  Moisture_Sensor moisture_sensor;
#endif  //MOISTURE_ENABLED

#if CO2_ENABLED
  #include "co2_sensor.h"
  Co2_Sensor co2_sensor;
#endif  //CO2_ENABLED

#if TEMPERATURE_ENABLED
  #include "temperature_sensors.h"
  Temperature_Sensors temperature_sensors;
#endif  //TEMPERATURE_ENABLED

// Calibration equations
#if CALIBRATION
  #include "calibration.h"
  Cal cal;
#endif  //CALIBRATION

// ST7735 LCD
#if LCD_ENABLED
  #include <Adafruit_GFX.h>
  #include <Adafruit_ST7735.h>
  #include "lcd_module.h"
  SlowST7735 tft = SlowST7735(&SPI, LCD_CS, LCD_DC, LCD_RST);
#endif  //LCD_ENABLED

/*  RTC & File Formatting */
//RTC DS3231 Module
RTC_DS3231 RTC;
char bufftime[] = "YYYY-MM-DDThh:mm:ss";
char clockonly[] = "hh:mm:ss";
int Y, M, D, h, m, s;
#if SD_ENABLED
  #include <SdFat.h>
  SdFat sd;
  File file;
  char fileName[] = "SPODID_YYYY_MM_DD.CSV";
#endif

/***************************************************************************************/
void setup() {
  #if SERIAL_ENABLED
    Serial.begin(9600);
  #endif //SERIAL_ENABLED

  /* Central Firmware (comms protocols) */
  Wire.begin();
  SPI.begin();
  
  /* RTC Declarations */
  RTC.begin();  //Initialize RTC
  #if RTC_UPDATE
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  #endif //RTC_UPDATE

  /* Sensor Begins */
  #if SI1145_ENABLED
    light_sensor.begin();
  #endif //SI1145_ENABLED
  #if MOISTURE_ENABLED
    moisture_sensor.begin();
  #endif  //MOISTURE_ENABLED
  #if CO2_ENABLED
    co2_sensor.begin();
  #endif  //CO2_ENABLED
  #if TEMPERATURE_ENABLED
    temperature_sensors.begin();
  #endif  //TEMPERATURE_ENABLED

  /* Pin Declarations */
  pinMode(SD_CS, OUTPUT);
  pinMode(LCD_CS, OUTPUT);

  /* Initial Pinout Call */
  delay(100);
  digitalWrite(SD_CS, HIGH); 
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(SM_POWER, LOW);
  delay(100);

  /* LCD Screen Initialization*/
  #if LCD_ENABLED
    tft.initR(INITR_144GREENTAB);
    tft.startWrite();
    tft.setRotation(SPOD_ST7735_ROTATION);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 42);
    tft.print(spodID);
    tft.endWrite();
    delay(100);
    #if SERIAL_DEBUG_ENABLED
      Serial.println("LCD Attempted");
    #endif //SERIAL_DEBUG_ENABLED
  #endif //LCD_ENABLED

  // CHECK PINS ARE RELEASED FROM SPI!!!!
  digitalWrite(SD_CS, HIGH); 
  digitalWrite(LCD_CS, HIGH);
  delay(100);

  /* SD & File Setup */
  #if SD_ENABLED
    // Establish contact with SD card - if initialization fails, run until success
    while (!sd.begin(SD_CONFIG)) {
      #if SERIAL_DEBUG_ENABLED
        Serial.println(F("SD Initialization Failed"));
      #endif //SERIAL_DEBUG_ENABLED
      sd.begin(SD_CONFIG);
      delay(100);   
    } //while(!sd.begin(SD_CS))
    //File Naming (FORMATTING HAS TO BE CONSISTENT WITH GLOBAL DECLARATION!!)
    DateTime now = RTC.now();  //pulls setup() time so we have one file name per run in a day
      Y = now.year();
      M = now.month();
      D = now.day();
    sprintf(fileName, "%s_%04u_%02u_%02u.CSV", spodID, Y, M, D);  //char array for fileName
    delay(100);
    file.open(fileName, O_CREAT | O_APPEND | O_WRITE);  //open with create, append, write permissions
    file.close();                                       //close file, we opened so loop() is faster
    delay(100);
  #else 
    DateTime now = RTC.now();
  #endif //SD_ENABLED

  // CHECK PINS ARE RELEASED FROM SPI!!!!
  digitalWrite(SD_CS, HIGH); 
  digitalWrite(LCD_CS, HIGH);
  delay(100);
} //void setup()

/***************************************************************************************/
void loop() {  
  // CHECK PINS ARE RELEASED FROM SPI!!!!
  digitalWrite(SD_CS, HIGH); 
  digitalWrite(LCD_CS, HIGH);
  delay(100);

  //Timestamp for this record
  DateTime now = RTC.now();
  Y = now.year();
  M = now.month();
  D = now.day();
  h = now.hour();
  m = now.minute();
  s = now.second();
  delay(100);
  sprintf(bufftime, "%04u-%02u-%02uT%02u:%02u:%02u", Y, M, D, h, m, s);
  // char timestamp = bufftime[11:end];
  sprintf(clockonly, "%02u:%02u:%02u", h, m, s);

  #if SERIAL_DEBUG_ENABLED
    Serial.println("Looped");
    delay(100);
  #endif //SERIAL_DEBUG_ENABLED

  #if LCD_ENABLED  
    tft.startWrite();
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 42);
    tft.print(spodID);
    tft.endWrite();
    delay(1000);
    Serial.println("LCD Attempted");
    digitalWrite(LCD_CS, HIGH);
    digitalWrite(SD_CS, HIGH);
    delay(200);
  #endif

  #if SD_ENABLED
    delay(100);
    Serial.print(F("LCD_CS="));  Serial.println(digitalRead(LCD_CS));
    Serial.print(F("SD_CS="));   Serial.println(digitalRead(SD_CS));
    sd.begin(SD_CONFIG);
    while (!sd.begin(SD_CONFIG)) 
    {
      #if SERIAL_ENABLED
        Serial.println(F("SD Initialization Failed"));
      #endif  
      #if SERIAL_DEBUG_ENABLED
        sd.initErrorPrint(&Serial);
      #endif //SERIAL_DEBUG_ENABLED
      sd.begin(SD_CONFIG);
      delay(100);   
    }
    file.open(fileName, O_CREAT | O_APPEND | O_WRITE);
    file.println("Data, data, data");
    Serial.println("SD Success");
    file.close();
    delay(100);
  #endif
  
  digitalWrite(SD_CS, HIGH); 
  digitalWrite(LCD_CS, HIGH);
  delay(100);
} //void loop()