/*******************************************************************************
 * @file    SPOD_Rework.ino
 * @brief   
 *
 * @author  Sascha Fowler
 * @date    July 28, 2026
 * @log     SPOD code redesign from V1, Tested with Orange SPOD - RTC Update, Serial/File Write (All data in one day), SI1145 Function
 * @TBD     Test Moisture Sensor, Temp Sensors, and CO2
            Add in New LCD screen
            Add in calibration sections to the code
******************************************************************************/
/*  Libraries  */
#include <Arduino.h>            
#include <Wire.h>             //P - last tested with "Wire@1.0"
#include <SoftwareSerial.h>   //P - last tested with "SoftwareSerial@1.0"
#include <SdFat.h>            //P - last tested with "SdFat@2.2.3"
#include <RTClib.h>           //P - last tested with "RTClib@2.1.4"
#include <SPI.h>
#include "SPOD_node.h"


#define SerialStream 1
#define chipSelect (10)
//const word chipSelect = 10;

/**************************  Conditional Declarations for Sensors  **************************/
/*  Header Files for LCD Screen  */
#if LCDDisplay
  #include "LCD_PCD8544.h"      //PCD8544 controller  datasheet   84x48 pixel black and white LCDs
                                //https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf
#endif

/*  Header Files for the Visible/UV/IR Light Sensor  */
#if SI1145
  #include "Adafruit_SI1145.h"
  Adafruit_SI1145 sun = Adafruit_SI1145();
#endif

#if MOISTURE_ENABLED
  int val = 0;        //value for storing moisture value
  int soilPin = A0;   //
  int soilPower = 7;  //Variable for soil moisture power
  int readSoil(){
    digitalWrite(soilPower, HIGH);//turn D7 "On"
    delay(10);//wait 10 milliseconds 
    val = analogRead(soilPin);//Read the SIG value form sensor 
    return val;//send current moisture value
  }
#endif

/*  Header Files for Temp Sensors  */
#if TEMPERATURE_ONE_ENABLED || TEMPERATURE_TWO_ENABLED
  #include <Ezo_i2c.h>
  #include <Ezo_i2c_util.h>
  #include <iot_cmd.h>

  Ezo_board ph = Ezo_board(99, "PH");
  Ezo_board temperature1 = Ezo_board(102, "TEMP1");  // Replace 100 with the I2C address of the first temperature sensor
  Ezo_board temperature2 = Ezo_board(103, "TEMP2");  // Replace 101 with the I2C address of the second temperature sensor
#endif                               

#if CO2_ENABLED
  #include <sequencer1.h>
  #include <sequencer2.h>
  #include <sequencer3.h>
  #include <sequencer4.h>

  #define address 105                                //default I2C ID number for EZO Co2 sensor.
  char computerdata[20];            //we make a 20 byte character array to hold incoming data from a pc/mac/other.
  byte received_from_computer = 0;  //we need to know how many characters have been received.
  byte serial_event = false;        //a flag to signal when data has been received from the pc/mac/other.
  byte code = 0;                    //used to hold the I2C response code.
  char Co2_data[20];                //we make a 20-byte character array to hold incoming data from the Co2 sensor.
  byte in_char = 0;                 //used as a 1 byte buffer to store inbound bytes from the Co2 sensor.
  byte j = 0;                       //counter used for Co2_data array.
  int time_ = 250;                  //used to set the delay needed to process the command sent to the EZO Co2 sensor.
  int Co2_int;                      //int var used to hold the value of the Co2.
#endif

#if CO2_ENABLED || TEMPERATURE_ONE_ENABLED || TEMPERATURE_TWO_ENABLED
  void receive_reading(Ezo_board & Sensor);
#endif

/*  RTC & File Formatting */
//RTC DS 1307 initialization
RTC_DS1307 RTC;
SdFat sd;
SdFile file;

// Buffers
// char spodID[] = "SPODID";
String fileName;
char date[] = "YYYY-MM-DD";
char bufftime[] = "hh:mm:ss";
int Y, M, D, h, m, s;
/*******/

void setup() {

  Wire.begin();
  SPI.begin();

  RTC.begin();
  #if RTC_UPDATE
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  #endif                 //RTC_UPDATE


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
  #else 
    DateTime now = RTC.now();
  #endif //SD_ENABLED
  #if SERIAL_ENABLED
    Serial.begin(9600);
  #endif

  pinMode(chipSelect, OUTPUT);
  pinMode(10, OUTPUT);

  // Create a file name based on the current date (format: "YourPodID_YYYY_MM_DD.txt")
  fileName = "SPOD_" + String(spodID) + "_" + String(now.year()) + "_" + String(now.month()) + "_" + String(now.day()) + ".txt";
  // Print the file name to Serial for debugging
  Serial.println("File Name: " + fileName);
  delay(1000); // Delay for a second before updating the file name again

  char fileNameArray[fileName.length()+1];
  fileName.toCharArray(fileNameArray, sizeof(fileNameArray));

  if (!sd.begin(chipSelect)) {
    Serial.println("Error: Card failed, or not present");
    digitalWrite(chipSelect, HIGH);
  }
  else{
    Serial.println("SD present");
  }
  digitalWrite(10, LOW);

  // while (!sd.begin(chipSelect))
  #if SI1145
    if (!sun.begin()) {
      Serial.println("Didn't find Si1145"); //if the light sensor is not detected. Init failed
    }
    else
    {
      Serial.println("The light sensor SI1145 is detected!"); //Success for init of Light sensor
    }
  #endif

  #if MOISTURE_ENABLED
    /*******************SOIL MOISTURE SENSOR PIN SETUP************/
    pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
    digitalWrite(soilPower, HIGH);//Set to LOW so no power is flowing through the sensor
  #endif
  
  //RTC.adjust(DateTime(__DATE__, __TIME__)); // run uncommented for the first time, once time is set, comment out and reupload.
  #if LCDDisplay
    pinMode(10, OUTPUT);
    pinMode(POWERpin, OUTPUT);
    digitalWrite(POWERpin, HIGH);
    pinMode(RST, OUTPUT);
    pinMode(ChipEnable, OUTPUT);
    pinMode(DCmodeSelect, OUTPUT);
    pinMode(DataIN, OUTPUT);
    pinMode(SerialCLK, OUTPUT);
    LcdInit();
    LcdXY(5,3);
    LcdWriteString(("SPOD " + String(spodID)).c_str());
    delay(2000);
    LCDClear();
  #endif

  Serial.println("Done Setup"); //states that all of the setup code is completed
}

void loop() {
  DateTime now = RTC.now();
  Y = now.year();
  M = now.month();
  D = now.day();
  h = now.hour();
  m = now.minute();
  s = now.second();
  sprintf(date, "%04u-%02u-%02u", Y, M, D);
  sprintf(bufftime, "%02u:%02u:%02u", h, m, s);

  #if CALIBRATION
    #if TEMPERATURE_ONE_ENABLED
      int temp_one_slope = 1;
      int temp_one_intercept = 0;
    #endif

    #if TEMPERATURE_TWO_ENABLED
      int temp_two_slope = 1;
      int temp_two_intercept = 0;
    #endif

    #if MOISTURE_ENABLED
      int moisture_slope = 1;
      int moisture_intercept = 0;
    #endif

    #if CO2_ENABLED
      int co2_slope = 1;
      int co2_intercept = 0;
    #endif
  #endif

  #if LCDDisplay
    static int cursorX = 0;
    static int cursorY = 0;
    LCDClear();
  #endif

  #if SI1145
    float UVindex = sun.readUV();
    // the index is multiplied by 100 so to get the
    // integer index, divide by 100!
    UVindex /= 100.0;
    float Visible = sun.readVisible();
    float IRindex = sun.readIR();
  #endif

  #if MOISTURE_ENABLED
    int read_soil = readSoil();
    int smspercent = 0;
    String soil_moisture = String(read_soil);

    #if CALIBRATION
      smspercent = (moisture_slope) * (read_soil) + 100;
      if (smspercent > 100) {
        smspercent = 100;
      } else if (smspercent < 0) {
        smspercent = 0;
      }
    #endif
  #endif

  #if TEMPERATURE_ONE_ENABLED
    float read_temp1;
    char floatStr[10];  // Adjust the size of the character array as needed
    dtostrf(read_temp1, 4, 2, floatStr);
    float read_ph;
    //check if there is a valid temperature reading
    if ((temperature1.get_error() == Ezo_board::SUCCESS) && (temperature1.get_last_received_reading() > -1000.0))  //Temp 1 check
    {
      ph.send_read_with_temp_comp(temperature1.get_last_received_reading());  //send readings from temp sensor to pH sensor
    } else {
      ph.send_read_cmd();
    }
    temperature1.send_read_cmd();
    delay(1000);
    receive_reading(temperature1);  //get the reading from the RTD circuit
    read_temp1 = temperature1.get_last_received_reading();
    char str[20];
    int temp_int = float(read_temp1);
    if (temp_int < 0)
      temp_int = 0;
    sprintf(str, "%d", (temp_int - 2));
    LcdXY(0, 0);                          // FOR OLD DISPLAY FILE
    LcdWriteString("Temp1:");             // FOR OLD DISPLAY FILE
    char temp1Str[10];                    // Create a character array to hold the temperature value
    dtostrf(read_temp1, 4, 2, temp1Str);  // Convert the float to a C-string with one decimal place
    LcdWriteString(temp1Str);             // FOR OLD DISPLAY FILE
    //LcdWriteString(String(read_temp1));
    LcdWriteString("C");  // FOR OLD DISPLAY FILE
  #endif

  #if TEMPERATURE_TWO_ENABLED
    #if (TEMPERATURE_ONE_ENABLED == 0)
      char str[20];
    #endif
    float read_temp2;
    if ((temperature2.get_error() == Ezo_board::SUCCESS) && (temperature2.get_last_received_reading() > -1000.0))
    { 
      ph.send_read_with_temp_comp(temperature2.get_last_received_reading());  //send readings from temp sensor to pH sensor
    } else {
      ph.send_read_cmd();
    }
    temperature2.send_read_cmd();

    delay(1000);  // read command needs a second to process

    receive_reading(temperature2);  //get the reading from the RTD circuit
    read_temp2 = temperature2.get_last_received_reading();

    /* Converting Int to char* for setStr */
    temp_int = float(read_temp2);


    if (temp_int < 0)
      temp_int = 0;

    sprintf(str, "%d", (temp_int - 2));

    LcdWriteString(", Temp2:");            // FOR OLD DISPLAY FILE
    char temp2Str[10];                    // Create a character array to hold the temperature value
    dtostrf(read_temp2, 4, 2, temp2Str);  // Convert the float to a C-string with one decimal place
    LcdWriteString(temp2Str);             // FOR OLD DISPLAY FILE
    //LcdWriteString(temp2Str);
    LcdWriteString("C");  // FOR OLD DISPLAY FILE
  #endif

  #if CO2_ENABLED
    String co2_data;
    #if ((TEMPERATURE_ONE_ENABLED == 0) && (TEMPERATURE_TWO_ENABLED == 0))
      char str[20];
    #endif
    /* CO2 Sensor Reading Logic */
    for (j = 0; j < 1; j++) {          //set all char to lower case, this is just so this exact sample code can recognize the "sleep" command.
      computerdata[j] = tolower('R');  //tolower(computerdata[i]);                                 //"Sleep" ≠ "sleep"
      delay(900);
    }
    j = 0;  //reset i, we will need it later

    Wire.beginTransmission(address);  //call the circuit by its ID number.
    Wire.write(computerdata);         //transmit the command that was sent through the serial port.
    Wire.endTransmission();           //end the I2C data transmission.

    if (strcmp(computerdata, "sleep") != 0) {  //if the command that has been sent is NOT the sleep command, wait the correct amount of time and request data.
                                              //if it is the sleep command, we do nothing. Issuing a sleep command and then requesting data will wake the Co2 sensor.
      delay(time_);                            //wait the correct amount of time for the circuit to complete its instruction.
      Wire.requestFrom(address, 20, 0);        //call the circuit and request 20 bytes (this may be more than we need)
      code = Wire.read();                      //the first byte is the response code, we read this separately.
      while (Wire.available()) {               //are there bytes to receive.
        in_char = Wire.read();                 //receive a byte.
        Co2_data[j] = (in_char);               //load this byte into our array
        co2_data = Co2_data;
        j += 1;              //incur the counter for the array element.
        if (in_char == 0) {  //if we see that we have been sent a null command.
          j = 0;             //reset the counter i to 0.
          break;             //exit the while loop.
        }
      }
    }
      Co2_int = atoi(Co2_data);
      serial_event = false;  //reset the serial event flag
      int co2_ppm = co2_data.toInt();
    //CALIBRATION AREAAAAAAA
    #if CALIBRATION
      co2_ppm = (co2_slope * Co2_int) + co2_intercept;  //White 2
      //Co2_data=String(co2_ppm);
    #endif
  #endif
  //fix co2 calibration
  //open SPI SD
  digitalWrite(SD_CS, LOW);

  while (!sd.begin(SD_CS)) {
    #if SERIAL_ENABLED
      Serial.println("Error in loop: No SD card found");
      delay(1000);
    #endif  //SERIAL_ENABLED
    sd.begin(SD_CS);
  }
  delay(100);

  if (sd.begin(SD_CS)) {
    // FILE FORMAT
    delay(100);
    file.open(fileName.c_str(), O_CREAT | O_APPEND | O_WRITE);
    delay(100);
    if (file.isOpen()) {
      //Serial.println("File is Open!");
      // file RTC Writing
      // SPOD ID
      file.print(spodID);
      file.print(",");
      delay(100);
      //file time writing
      file.print(date);
      delay(100);
      file.print(",");
      file.print(bufftime);
      delay(100);
      file.print(",");
      //PH (raw)
      delay(100);
      file.print(",");

      #if TEMPERATURE_ONE_ENABLED
        file.print(temp1Str);
      #endif
      file.print(",");
      delay(100);
      #if TEMPERATURE_TWO_ENABLED
        file.print(temp2Str);
      #endif
      file.print(",");
      delay(100);

      #if CO2_ENABLED
        file.print(co2_ppm);
      #endif
      file.print(",");
      delay(100);

      #if MOISTURE_ENABLED
        file.print(smspercent);
      #endif
      file.print(",");
      delay(100);

      #if SI1145
        file.print(Visible);
      #endif
      file.print(",");
      delay(100);
      #if SI1145
        file.print(IRindex);
      #endif
      file.print(",");
      delay(100);
      #if SI1145
        file.print(UVindex);
      #endif
      file.print(",");
      delay(100);
    
    file.println();  // optional: end the line
    file.sync();
    file.close();
    }
    else{
      Serial.println("File is not open.");
    }
  }

  //LCD SCREEN Print (legacy)
  // Temp 1 && Temp 2
  #if TEMPERATURE_ONE_ENABLED
    LcdXY(0, 0);
    LcdWriteString("Temp1:");
    LcdWriteString(temp1Str);
    LcdWriteString("C");
  #endif
  #if TEMPERATURE_TWO_ENABLED
      LcdWriteString(", Temp2:");
      LcdWriteString(temp2Str);
      LcdWriteString("C");
  #endif

  // CO2
  #if CO2_ENABLED
    LcdXY(0, 2);
    LcdWriteString("CO2: ");
    sprintf(str, "%d", Co2_data);
    LcdWriteString(str);
    LcdWriteString("ppm");
  #endif

  //Soil moisture
  #if MOISTURE_ENABLED
    char moistureStr[10];
    sprintf(moistureStr, "%d", smspercent);
    LcdXY(0, 4);
    LcdWriteString("S.M.S: ");
    LcdWriteString(moistureStr);
    LcdWriteString("%");
    delay(2000);
  #endif

  // SI1145 Section
  #if SI1145
    char visStr[10], irStr[10], uvStr[10];
    // Convertinbg float to string for the LCD
    dtostrf(Visible, 4, 2, visStr);
    dtostrf(IRindex, 4, 2, irStr);
    dtostrf(UVindex, 4, 2, uvStr);
    // VIS
    LcdXY(0, 0);
    LcdWriteString("Vis: ");
    LcdWriteString(visStr);
    delay(800);
    // IR
    LcdXY(0, 2);
    LcdWriteString("IR: ");
    LcdWriteString(irStr);
    delay(800);
    // UV
    LcdXY(0, 4);
    LcdWriteString("UV: ");
    LcdWriteString(uvStr);
    delay(2000);
    LCDClear();
  #endif

  #if SERIAL_ENABLED
    Serial.print("\n");
    Serial.print(date);
    Serial.print(",");
    delay(100);
    Serial.print(bufftime);
    Serial.print(",");
    delay(100);

    #if SI1145
      Serial.print("Vis:");
      Serial.print(Visible);
      Serial.print(",");
      Serial.print("IR:");
      Serial.print(IRindex);
      Serial.print(",");
      Serial.print("UV:");
      Serial.print(UVindex);
      Serial.print(",");
    #endif

    #if MOISTURE_ENABLED
      Serial.print("S.M.S: ");
      Serial.print(smspercent);
      Serial.print("%");
      Serial.print(",");
    #endif

    #if TEMPERATURE_ONE_ENABLED
      Serial.print("T1: ");
      Serial.print(temp1Str);
      Serial.print("C");
      Serial.print(",");
    #endif

    #if TEMPERATURE_TWO_ENABLED
      Serial.print("T2: ");
      Serial.print(temp2Str);
      Serial.print(" C");
      Serial.print(",");
    #endif

    #if CO2_ENABLED
      Serial.print("CO2: ");
      Serial.print(co2_data);
      Serial.print(" PPM");
      Serial.print(",");
    #endif
  #endif
}

#if CO2_ENABLED || TEMPERATURE_ONE_ENABLED || TEMPERATURE_TWO_ENABLED
  void receive_reading(Ezo_board & Sensor)
  {

    Serial.print(Sensor.get_name()); Serial.print(": ");  // print the name of the circuit getting the reading

    Sensor.receive_read_cmd();                            // get the response data

    switch (Sensor.get_error()) {                         // switch case based on what the response code is.
      case Ezo_board::SUCCESS:
        Serial.print(Sensor.get_last_received_reading()); //the command was successful, print the reading
        break;

      case Ezo_board::FAIL:
        Serial.print("Failed ");                          //means the command has failed.
        break;

      case Ezo_board::NOT_READY:
        Serial.print("Pending ");                         //the command has not yet been finished calculating.
        break;

      case Ezo_board::NO_DATA:
        Serial.print("No Data ");                         //the sensor has no data to send.
        break;
    }
  }
#endif
