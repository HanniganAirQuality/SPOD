/*******************************************************************************
 * @file    SPOD_V3.ino
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
  LCD_Module lcd_module;
#endif  //LCD_ENABLED

/*  RTC & File Formatting */
//RTC DS3231 Module
RTC_DS3231 RTC;
char bufftime[] = "YYYY-MM-DDThh:mm:ss";
// char clockonly[] = "hh:mm:ss";
int Y, M, D, h, m, s;
#if SD_ENABLED
  #include <SdFat.h>
  SdFat sd;
  File file;
  char fileName[] = "SPODID_YYYY_MM_DD.CSV";
#endif

/***************************************************************************************/
void setup() {
  #if SERIAL_ENABLED || SERIAL_DEBUG_ENABLED
    Serial.begin(9600);
  #endif //SERIAL_ENABLED

  /* Central Firmware (comms protocols) */
  Wire.begin();
  // SPI.begin();
  
  /* RTC Declarations */
  RTC.begin();  //Initialize RTC
  #if RTC_UPDATE
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  #endif //RTC_UPDATE

  /* Sensor Begins */
  #if SI1145_ENABLED
    bool light_ready = light_sensor.begin();
    #if SERIAL_DEBUG_ENABLED
      if (light_ready) {
        Serial.println(F("SI1145 ready"));
      } else {
        Serial.println(F("SI1145 unavailable"));
      } //if (light_ready)
    #endif  //SERIAL_DEBUG_ENABLED
  #endif  //SI1145_ENABLED
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
  digitalWrite(SD_CS, HIGH); 
  pinMode(LCD_CS, OUTPUT);
  digitalWrite(LCD_CS, HIGH);

  // delay(100);
  // delay(100);

  /* LCD Screen Initialization*/
  #if LCD_ENABLED
    lcd_module.begin();
    // CHECK PINS ARE RELEASED FROM SPI!!!!
    digitalWrite(SD_CS, HIGH); 
    digitalWrite(LCD_CS, HIGH);
  delay(100);
  #endif //LCD_ENABLED

  /* SD & File Setup */
  #if SD_ENABLED
    // Establish contact with SD card - if initialization fails, run until success
    while (!sd.begin(SD_CONFIG)) {
      #if SERIAL_DEBUG_ENABLED
        Serial.println(F("SD Initialization Failed"));
      #endif //SERIAL_DEBUG_ENABLED
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
  /* INITIALIZING !!! */
  // Default variable set (-99 = no reading, same as the YPOD)
  float t1 = NO_READING;
  float t2 = NO_READING;
  int co2_raw = NO_READING;
  int co2_cal = NO_READING;
  int sm_raw = NO_READING;
  int sm_cal = NO_READING;
  int visible = NO_READING;  // long: raw light counts can reach 65535
  int infrared = NO_READING;
  int rawUV = NO_READING;

  // CHECK PINS ARE RELEASED FROM SPI!!!!
  digitalWrite(SD_CS, HIGH); 
  digitalWrite(LCD_CS, HIGH);
  delay(100);

  /* Sensor Reads - a variable only changes from null if its sensor gave a good reading */
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
  // sprintf(clockonly, "%02u:%02u:%02u", h, m, s);
  const char* clockonly = bufftime + 11;

  #if SI1145_ENABLED
    LightSensorReading light = light_sensor.read();
    if (light.visibleValid) {
      visible = light.visible;
    } //if (light.visibleValid)
    if (light.infraredValid) {
      infrared = light.infrared;
    } //if (light.infraredValid)
    if (light.uvValid) {
      rawUV = light.rawUV;
    } //if (light.uvValid)
  #endif  //SI1145_ENABLED

  #if MOISTURE_ENABLED
    MoistureSensorReading moisture = moisture_sensor.read();
    if (moisture.valid) {
      sm_raw = moisture.rawValue;
    } //if (moisture.valid)
  #endif  //MOISTURE_ENABLED

  #if TEMPERATURE_ENABLED
    TemperatureSensorReadings temps = temperature_sensors.read();
    if (temps.temperature1Valid) {
      t1 = temps.temperature1;
    } //if (temps.temperature1Valid)
    if (temps.temperature2Valid) {
      t2 = temps.temperature2;
    } //if (temps.temperature2Valid)
  #endif  //TEMPERATURE_ENABLED

  #if CO2_ENABLED
    Co2SensorReading co2 = co2_sensor.read();
    if (co2.valid) {
      co2_raw = co2.ppm;
    } //if (co2.valid)
  #endif  //CO2_ENABLED

  /* Calibration - runs once, and only keeps results for sensors that worked */
  #if CALIBRATION
    calOutput cal_out = cal.calibrate(co2_raw, sm_raw);
    if (co2_raw != NO_READING) {
      co2_cal = cal_out.CO2_;
    } //if (co2_raw != NO_READING)
    if (sm_raw != NO_READING) {
      sm_cal = cal_out.SM_;
    } //if (sm_raw != NO_READING)
  #endif  //CALIBRATION

  #if SERIAL_DEBUG_ENABLED
    Serial.println("Looped");
    delay(100);
  #endif //SERIAL_DEBUG_ENABLED

  /* LCD */
  #if LCD_ENABLED  
  //NEED TO EVENTUALLY MOVE TO lcd_module with correct formatting...
    lcd_module.update(clockonly, t1, t2, co2_cal, sm_cal, visible, infrared, rawUV);
    delay(100);
    digitalWrite(LCD_CS, HIGH);
    digitalWrite(SD_CS, HIGH);
    delay(100);
  #endif

  /* SD Card */
  #if SD_ENABLED
    while (!sd.begin(SD_CONFIG)) {
      #if SERIAL_DEBUG_ENABLED
        Serial.println(F("SD Initialization Failed"));
        sd.initErrorPrint(&Serial);
      #endif  //SERIAL_DEBUG_ENABLED
      delay(100);
    } //while (!sd.begin(SD_CONFIG))
    file.open(fileName, O_CREAT | O_APPEND | O_WRITE);
    if (file.isOpen()) {
      printOutput(file, t1, t2, co2_raw, co2_cal, sm_raw, sm_cal, visible, infrared, rawUV);
      file.sync();
      file.close();
    } else {
      #if SERIAL_DEBUG_ENABLED
        Serial.println("file not opening?");
      #endif  //SERIAL_DEBUG_ENABLED
      file.close();
    } //if (file.isOpen())
    digitalWrite(SD_CS, HIGH); 
    digitalWrite(LCD_CS, HIGH);
    delay(100);
  #endif //SD_ENABLED
  
  /* Echo to Serial */
  #if SERIAL_ENABLED
    printOutput(Serial, t1, t2, co2_raw, co2_cal, sm_raw, sm_cal, visible, infrared, rawUV);
    Serial.flush();
  #endif  //SERIAL_ENABLED
} //void loop()

/***************************************************************************************/
// One row of data, written to SD or Serial dep. on input.
// Column order (RETIGO-compatible): time, longitude, latitude, SPOD ID,
// firmware version, T1, T2, CO2 raw, CO2 calibrated, SM raw, SM calibrated,
// visible, infrared, UV index. Longitude & latitude stay empty (no GPS yet).
void printOutput(Print &output, float t1, float t2, int co2_raw, int co2_cal,
                 int sm_raw, int sm_cal, long visible, long infrared, int rawUV) {
  output.print(bufftime);
  output.print(",");
  output.print(",");  //GPS EAST_LONGITUDE
  output.print(",");  //GPS NORTH_LATITUDE
  output.print(spodID);
  output.print(",");
  output.print(firmwareVersion);
  output.print(",");
  output.print(t1);
  output.print(",");
  output.print(t2);
  output.print(",");
  output.print(co2_raw);
  output.print(",");
  output.print(co2_cal);
  output.print(",");
  output.print(sm_raw);
  output.print(",");
  output.print(sm_cal);
  output.print(",");
  output.print(visible);
  output.print(",");
  output.print(infrared);
  output.print(",");
  output.print(rawUV);
  output.print("\n");
} //void printOutput()