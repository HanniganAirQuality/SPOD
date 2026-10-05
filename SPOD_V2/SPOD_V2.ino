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
 ******************************************************************************/

#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
#include <string.h>
#include <Wire.h>
#include "SPOD_node.h"

#if SI1145  
  #include "light_sensor.h"
#endif

#if MOISTURE_ENABLED
  #include "moisture_sensor.h"
#endif

#if CO2_ENABLED
  #include "co2_sensor.h"
#endif

#if TEMPERATURE_ENABLED
  #include "temperature_sensors.h"
#endif

#if CALIBRATION
  #include "calibration.h"
#endif

#if LCDDisplay
  #include "LCDDisplay.h"
#endif

/*  RTC & File Formatting */
//RTC DS3231 Module - to re-initialize time, use RTClib>examples>ds3231
RTC_DS3231 RTC;
#if SD_ENABLED
  //SdFat (SD Card) & File file (file on SD)
  #include <SdFat.h>   //P - last tested with "SdFat@2.2.3"
  SdFat sd;
  File file;
  // Buffers
  // char ypodID[] = "YPODID";
  char fileName[] = "SPODID_YYYY_MM_DD.CSV";
#endif //SD_ENABLED
char firmwareFileName[32];
char bufftime[] = "YYYY-MM-DDThh:mm:ss";
int Y, M, D, h, m, s;

/* STRUCTURES */

struct SpodReadings {
  LightSensorReading light;
  MoistureSensorReading moisture;
  int calibratedMoistureValue;
  TemperatureSensorReadings temperatures;
  Co2SensorReading co2;
  int calibratedCo2Ppm;
};

const size_t OUTPUT_FIELD_SIZE = 20;

// This is the one canonical record consumed by SD, serial, and the LCD.
// Values are calibrated (when enabled), validated, and formatted only once.
struct SpodOutputRecord {
  char visible[OUTPUT_FIELD_SIZE];
  char infrared[OUTPUT_FIELD_SIZE];
  char uvIndex[OUTPUT_FIELD_SIZE];
  char uncalibratedMoisture[OUTPUT_FIELD_SIZE];
  char calibratedMoisture[OUTPUT_FIELD_SIZE];
  char temperature1[OUTPUT_FIELD_SIZE];
  char temperature2[OUTPUT_FIELD_SIZE];
  char uncalibratedCo2[OUTPUT_FIELD_SIZE];
  char calibratedCo2[OUTPUT_FIELD_SIZE];
  bool moistureNumeric;
  bool co2Numeric;
};

void copyOutputField(char* destination, const char* source) {
  strncpy(destination, source, OUTPUT_FIELD_SIZE - 1);
  destination[OUTPUT_FIELD_SIZE - 1] = '\0';
}

void formatFloatOutput(char* destination, bool enabled, bool valid, float value) {
  if (!enabled) {
    destination[0] = '\0';
  } else if (!valid) {
    copyOutputField(destination, "NA");
  } else {
    char formatted[OUTPUT_FIELD_SIZE];
    dtostrf(value, 1, 2, formatted);
    copyOutputField(destination, formatted);
  }
}

void formatIntOutput(char* destination, bool enabled, bool valid, int value) {
  if (!enabled) {
    destination[0] = '\0';
  } else if (!valid) {
    copyOutputField(destination, "NA");
  } else {
    snprintf(destination, OUTPUT_FIELD_SIZE, "%d", value);
  }
}

SpodReadings collectReadings() {
  SpodReadings readings = {};

  readings.light = LightSensor::read();
  readings.moisture = MoistureSensor::read();
  readings.temperatures = TemperatureSensors::read();
  const float rawTemperature1 = readings.temperatures.temperature1;
  const bool rawTemperature1Valid =
    readings.temperatures.temperature1Valid;

  if (readings.moisture.valid) {
    readings.calibratedMoistureValue = readings.moisture.rawValue;
    #if CALIBRATION
      readings.calibratedMoistureValue =
        SpodCalibration::calibrateSoilMoisture(
          readings.moisture.rawValue);
    #endif
  }

  #if CALIBRATION
    if (readings.temperatures.temperature1Valid) {
      readings.temperatures.temperature1 =
        SpodCalibration::calibrateTemperature1(
          readings.temperatures.temperature1);
    }
    if (readings.temperatures.temperature2Valid) {
      readings.temperatures.temperature2 =
        SpodCalibration::calibrateTemperature2(
          readings.temperatures.temperature2);
    }
  #endif

  readings.co2 = Co2Sensor::read();
  readings.calibratedCo2Ppm = readings.co2.ppm;

  #if CALIBRATION
    if (readings.co2.valid) {
      readings.calibratedCo2Ppm = SpodCalibration::calibrateCo2(
        readings.co2.ppm, rawTemperature1, rawTemperature1Valid);
    }
  #endif

  return readings;
}

SpodOutputRecord buildOutputRecord(const SpodReadings& readings) {
  SpodOutputRecord output = {};

  formatIntOutput(
    output.visible, SI1145, readings.light.visibleValid,
    readings.light.visible);
  formatIntOutput(
    output.infrared, SI1145, readings.light.infraredValid,
    readings.light.infrared);
  formatFloatOutput(
    output.uvIndex, SI1145, readings.light.uvValid, readings.light.uvIndex);
  formatIntOutput(
    output.uncalibratedMoisture, MOISTURE_ENABLED, readings.moisture.valid,
    readings.moisture.rawValue);
  formatIntOutput(
    output.calibratedMoisture, MOISTURE_ENABLED, readings.moisture.valid,
    readings.calibratedMoistureValue);
  output.moistureNumeric = MOISTURE_ENABLED && readings.moisture.valid;
  formatFloatOutput(
    output.temperature1, TEMPERATURE_ENABLED,
    readings.temperatures.temperature1Valid,
    readings.temperatures.temperature1);
  formatFloatOutput(
    output.temperature2, TEMPERATURE_ENABLED,
    readings.temperatures.temperature2Valid,
    readings.temperatures.temperature2);

  if (!CO2_ENABLED) {
    output.uncalibratedCo2[0] = '\0';
    output.calibratedCo2[0] = '\0';
  } else if (readings.co2.valid) {
    formatIntOutput(output.uncalibratedCo2, true, true, readings.co2.ppm);
    formatIntOutput(
      output.calibratedCo2, true, true, readings.calibratedCo2Ppm);
    output.co2Numeric = true;
  } else if (readings.co2.hasResponse) {
    // Preserve status text such as "*WARM" rather than inventing a zero.
    copyOutputField(output.uncalibratedCo2, readings.co2.rawResponse);
    copyOutputField(output.calibratedCo2, readings.co2.rawResponse);
  } else {
    copyOutputField(output.uncalibratedCo2, "NA");
    copyOutputField(output.calibratedCo2, "NA");
  }

  return output;
}

// RETIGO-compatible SPOD_V2.0 row. Longitude and latitude remain empty until
// the SPOD has a position source.
template <typename Output>
void writeOutputRecord(Output& destination, const SpodOutputRecord& output) {
  // destination.print(date);
  // destination.print('T');
  destination.print(bufftime);
  destination.print(',');
  destination.print(',');
  destination.print(',');
  destination.print(spodID);
  destination.print(',');
  destination.print(firmwareVersion);
  destination.print(',');
  destination.print(output.temperature1);
  destination.print(',');
  destination.print(output.temperature2);
  destination.print(',');
  destination.print(output.uncalibratedCo2);
  destination.print(',');
  destination.print(output.calibratedCo2);
  destination.print(',');
  destination.print(output.uncalibratedMoisture);
  destination.print(',');
  destination.print(output.calibratedMoisture);
  destination.print(',');
  destination.print(output.visible);
  destination.print(',');
  destination.print(output.infrared);
  destination.print(',');
  destination.println(output.uvIndex);
}

// #if SD_ENABLED
//   void writeSdRecord(const SpodOutputRecord& output) {
//     serviceSdCard();
//     if (!sdReady) {
//       return;
//     }

//     if (!file.open(fileName.c_str(), O_CREAT | O_APPEND | O_WRITE)) {
//       #if SERIAL_DEBUG_ENABLED
//         Serial.println("SD file open failed; waiting to retry");
//       #endif
//       markSdCardUnavailable();
//       return;
//     }

//     writeOutputRecord(file, output);

//     const bool syncSucceeded = file.sync();
//     const bool writeSucceeded = !file.getWriteError();
//     const bool closeSucceeded = file.close();

//     if (!writeSucceeded || !syncSucceeded || !closeSucceeded) {
//       #if SERIAL_DEBUG_ENABLED
//         Serial.println("SD write failed; waiting to retry");
//       #endif
//       markSdCardUnavailable();
//     }
//   }
// #endif

#if LCDDisplay
  void updateLcd(const SpodOutputRecord& output) {
    LCDReadings displayReadings = {};
    displayReadings.lightEnabled = SI1145;
    displayReadings.visible = output.visible;
    displayReadings.infrared = output.infrared;
    displayReadings.uvIndex = output.uvIndex;
    displayReadings.moistureEnabled = MOISTURE_ENABLED;
    displayReadings.moistureNumeric = output.moistureNumeric;
    displayReadings.moisture = output.calibratedMoisture;
    displayReadings.temperature1Enabled = TEMPERATURE_ENABLED;
    displayReadings.temperature1 = output.temperature1;
    displayReadings.temperature2Enabled = TEMPERATURE_ENABLED;
    displayReadings.temperature2 = output.temperature2;
    displayReadings.co2Enabled = CO2_ENABLED;
    displayReadings.co2Numeric = output.co2Numeric;
    displayReadings.co2 = output.calibratedCo2;
    lcdDisplayUpdate(displayReadings);
  }
#endif

// #if SERIAL_ENABLED
//   void writeSerialRecord(const SpodOutputRecord& output) {
//     writeOutputRecord(Serial, output);
//   }
// #endif

void setup() {
  #if SERIAL_ENABLED || SERIAL_DEBUG_ENABLED
    Serial.begin(9600);
  #endif

  //Central Firmware (comms protocols)
  Wire.begin();
  SPI.begin();
  //Object Begins
  RTC.begin();  //Initialize RTC
  #if RTC_UPDATE
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  #endif //RTC_UPDATE

  //Initialize Pins - Establish direction of pin comms
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(SOIL_POWER_PIN, OUTPUT);
    digitalWrite(SOIL_POWER_PIN, LOW);
  pinMode(SD_CS, OUTPUT);

  #if SD_ENABLED
    pinMode(SD_CS, OUTPUT);
    /*  SD Card & File Setup  */
    digitalWrite(SD_CS, LOW);  //Pull SD_CS pin LOW to initialize SPI comms
    sd.begin(SD_CS);           //Initialize SD Card with relevant chip select pin
    // Establish contact with SD card - if initialization fails, run until success
    while (!sd.begin(SD_CS)) {
      #if SERIAL_ENABLED
        Serial.println("insert sd card to begin");
      #endif                        //SERIAL_ENABLED
      sd.begin(SD_CS);          //attempt to initialize again
    }                           //while(!sd.begin(SD_CS))
    //File Naming (FORMATTING HAS TO BE CONSISTENT WITH GLOBAL DECLARATION!!)
    DateTime now = RTC.now();  //pulls setup() time so we have one file name per run in a day
    Y = now.year();
    M = now.month();
    D = now.day();
    // sprintf(ypodID, "YPOD%02X", YPODID);                                  //char array for podID
    sprintf(fileName, "%s_%04u_%02u_%02u.CSV", spodID, Y, M, D);  //char array for fileName
    delay(100);
    file.open(fileName, O_CREAT | O_APPEND | O_WRITE);  //open with create, append, write permissions
    file.close();                                       //close file, we opened so loop() is faster
    digitalWrite(SD_CS, HIGH);                          //release chip select on SD - allow other comm with SPI
  #else  //SD_ENABLED
    DateTime now = RTC.now();
  #endif //SD_ENABLED

  #if SERIAL_DEBUG_ENABLED
    Serial.println("SPOD startup");
    // Serial.println("File Name: " + fileName);
    #if CALIBRATION
      Serial.print("Calibration profile: ");
      Serial.println(spodID);
    #endif
  #endif


  const bool lightSensorReady = LightSensor::begin();
  MoistureSensor::begin();
  TemperatureSensors::begin();
  Co2Sensor::begin();

  #if SERIAL_DEBUG_ENABLED
    Serial.println(lightSensorReady ? "SI1145 ready" : "SI1145 unavailable");
  #else
    (void)lightSensorReady;
  #endif

  #if LCDDisplay
    digitalWrite(LCD_CS, LOW);
    delay(100);
    lcdDisplayBegin(spodID);
    digitalWrite(LCD_CS, HIGH);
    delay(100);
  #endif
}

void loop() {
  //   //File Naming (FORMATTING HAS TO BE CONSISTENT WITH GLOBAL DECLARATION!!)
  //   DateTime now = RTC.now();  //pulls setup() time so we have one file name per run in a day
  //   Y = now.year();
  //   M = now.month();
  //   D = now.day();
  //   // sprintf(ypodID, "YPOD%02X", YPODID);                                  //char array for podID
  //   sprintf(fileName, "%s_%04u_%02u_%02u.CSV", spodID, Y, M, D);  //char array for fileName
  //   delay(100);
  //   file.open(fileName, O_CREAT | O_APPEND | O_WRITE);  //open with create, append, write permissions
  //   file.close();                                       //close file, we opened so loop() is faster
  //   digitalWrite(SD_CS, HIGH);                          //release chip select on SD - allow other comm with SPI
  // #else  //SD_ENABLED
  //   DateTime now = RTC.now();
  // #endif //SD_ENABLED

  DateTime now = RTC.now();
    Y = now.year();
    M = now.month();
    D = now.day();
    h = now.hour();
    m = now.minute();
    s = now.second();
  delay(100);
  sprintf(bufftime, "%04u-%02u-%02uT%02u:%02u:%02u", Y, M, D, h, m, s);

  const SpodReadings readings = collectReadings();
  const SpodOutputRecord output = buildOutputRecord(readings);

  //open SPI SD
  #if SD_ENABLED
    delay(100);
    digitalWrite(SD_CS, LOW);  //Pull SD_CS pin LOW to initialize SPI comms
    sd.begin(SD_CS);           //Initialize SD Card with relevant chip select pin

    while (!sd.begin(SD_CS)) {
      #if SERIAL_ENABLED
        Serial.println("error in loop");
      #endif  //SERIAL_ENABLED
        sd.begin(SD_CS);
    }
  // #endif //SD_ENABLED
    if (sd.begin(SD_CS)) {
      // FILE FORMAT = RETIGO
      // delay(100);
      file.open(fileName, O_CREAT | O_APPEND | O_WRITE);
      writeOutputRecord(file, output);
      delay(100);
      if (file.isOpen()) {
        delay(100);
        file.sync();
        file.close();
        // delay(100);
      } else {
        Serial.println("file not opening?");
        file.close();
      } //if (file.isOpen())
    }  //if (sd.begin())
    digitalWrite(SD_CS, HIGH);
    delay(100);
  #endif //SD_ENABLED

  #if LCDDisplay
    digitalWrite(LCD_CS, LOW);
    updateLcd(output);
    digitalWrite(LCD_CS, HIGH);
    delay(100);
  #endif

  #if SERIAL_ENABLED
    writeOutputRecord(Serial, output);
  #endif
}
