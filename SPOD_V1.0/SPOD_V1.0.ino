
#include "Adafruit_SI1145.h"
#include "PodConfig.h"

#include <Ezo_i2c.h>
#include <Ezo_i2c_util.h>
#include <iot_cmd.h>
#include <sequencer1.h>
#include <sequencer2.h>
#include <sequencer3.h>
#include <sequencer4.h>

#include <Wire.h>
#include <RTClib.h>
#include <SdFat.h>
#include "LCD_PCD8544.h"
#include <SoftwareSerial.h>
#define SerialStream 1
RTC_DS1307 RTC;
SdFat sd;
SdFile file;
bool sdReady = false;
#define chipSelect (SD_CHIP_SELECT_PIN)
//const word chipSelect = 10;

String fileName;
String delimiter = ",";
/**********SOIL MOISTURE VARIABLES************/
int val = 0; //value for storing moisture value 
int soilPin = SOIL_SENSOR_PIN; // 
int soilPower = SOIL_POWER_PIN;//Variable for Soil moisture Power
/**********SOIL MOISTURE VARIABLES************/
Adafruit_SI1145 uv = Adafruit_SI1145();
/****RTD EZO VARIABLES***********/
Ezo_board ph = Ezo_board(PH_I2C_ADDRESS, "PH");
Ezo_board temperature1 = Ezo_board(TEMP1_I2C_ADDRESS, "TEMP1"); // Replace 100 with the I2C address of the first temperature sensor
Ezo_board temperature2 = Ezo_board(TEMP2_I2C_ADDRESS, "TEMP2"); // Replace 101 with the I2C address of the second temperature sensor


/****CO2 I2C VARIABLES***********/
char computerdata[20];           //we make a 20 byte character array to hold incoming data from a pc/mac/other.
byte received_from_computer = 0; //we need to know how many characters have been received.
byte serial_event = false;       //a flag to signal when data has been received from the pc/mac/other.
byte code = 0;                   //used to hold the I2C response code.
char Co2_data[20];               //we make a 20-byte character array to hold incoming data from the Co2 sensor.
byte in_char = 0;                //used as a 1 byte buffer to store inbound bytes from the Co2 sensor.
byte j = 0;                      //counter used for Co2_data array.
int time_ = 250;                 //used to set the delay needed to process the command sent to the EZO Co2 sensor.
int Co2_int;                     //int var used to hold the value of the Co2.

void setup() {
  
  #if SerialStream
  Serial.begin(9600);
  #endif
  Wire.begin();
  if (! uv.begin()) {
    Serial.println("Didn't find Si1145"); //if the light sensor is not detected. Init failed
  }
 else
 {
  Serial.println("The light sensor SI1145 is detected!"); //Success for init of Light sensor
 }

  RTC.begin();
  #if SET_RTC_FROM_COMPILE_TIME
  RTC.adjust(DateTime(__DATE__, __TIME__));
  #endif
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
  LcdWriteString(POD_NAME); 
  delay(2000);
  LCDClear();
  SPI.begin();
  pinMode(chipSelect, OUTPUT);
  pinMode(10, OUTPUT);
  DateTime now = RTC.now();

  // Create a file name based on the current date (format: "YourPodID_YYYY_MM_DD.txt")
  fileName = String(POD_NAME) + "_" + String(now.year()) + "_" + String(now.month()) + "_" + String(now.day()) + ".txt";

  // Print the file name to Serial for debugging
  Serial.println("File Name: " + fileName);

  // Your code to save data to the file goes here

  delay(1000); // Delay for a second before updating the file name again



  char fileNameArray[fileName.length()+1];
  fileName.toCharArray(fileNameArray, sizeof(fileNameArray));

    sdReady = sd.begin(chipSelect);
    if (!sdReady) {
        Serial.println("Error: Card failed, or not present");
          digitalWrite(chipSelect, HIGH);

    }
    else{
      Serial.println("sd present");

    }
      digitalWrite(10, LOW);

  // while (!sd.begin(chipSelect))
  /*******************SOIL MOISTURE SENSOR PIN SETUP************/
  pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
  digitalWrite(soilPower, HIGH);//Set to LOW so no power is flowing through the sensor
   /*******************SOIL MOISTURE SENSOR PIN SETUP************/
    Serial.println("Done Setup"); //Success for init of Light sensor

}
void loop() {
  String data;
  String co2_data;
  Serial.println("started program"); //Success for init of Light sensor

  char fileNameArray[fileName.length()+1];
  fileName.toCharArray(fileNameArray, sizeof(fileNameArray));

  DateTime now = RTC.now();
 

  /* LCD Cursors */
  static int cursorX = 0;
  static int cursorY = 0;

  LCDClear();

 data += String(POD_NAME) + delimiter + String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + delimiter + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second())+ delimiter + delimiter; // Format to write into SD card
//data += String(POD_NAME) + delimiter;

  /*Temperature reading Logic */
  float read_temp1;
  float read_temp2;
  float read_ph;
  //check if there is a valid temperature reading
  if ((temperature1.get_error() == Ezo_board::SUCCESS) && (temperature1.get_last_received_reading() > -1000.0)) //Temp 1 check
  {

    
    //send readings from temp sensor to pH sensor
    ph.send_read_with_temp_comp(temperature1.get_last_received_reading());
    
  } 
  else 
  {
    ph.send_read_cmd();
    
  }
  temperature1.send_read_cmd();

  delay(1000);                                          // read command needs a second to process

  //receive_reading(ph);                                  //get the reading from the PH circuit
  //Serial.print("  ");
  //read_ph = ph.get_last_received_reading();
  receive_reading(temperature1);                         //get the reading from the RTD circuit
  read_temp1 = temperature1.get_last_received_reading();
  /* LCD Print pH and Temperature */
 // setStr(String(read_ph), 0, 10, BLACK);

  /* Converting Int to char* for setStr */
  char str[20];
  int temp_int;
  temp_int = float(read_temp1);

  if(temp_int < 0)
    temp_int = 0;

  sprintf(str,"%d", (temp_int - 2));
data += String(str) + delimiter;
  //for(int i = 0; i < 20; i++)
   // setPixel(30+i, 10, WHITE);
   
  LcdXY(0,0);
  LcdWriteString("Temp1:");
  char temp1Str[10]; // Create a character array to hold the temperature value
  dtostrf(read_temp1, 4, 2, temp1Str); // Convert the float to a C-string with one decimal place
  LcdWriteString(temp1Str);
  //LcdWriteString(String(read_temp1));  
  LcdWriteString("C");
  //data += String(temp1Str) + delimiter;
  if ((temperature2.get_error() == Ezo_board::SUCCESS) && (temperature2.get_last_received_reading() > -1000.0)) //Temperature 2 part
  {
    
  }
  temperature2.send_read_cmd();

  delay(1000);                                          // read command needs a second to process

  receive_reading(temperature2);                         //get the reading from the RTD circuit
  read_temp2 = temperature2.get_last_received_reading();

  /* Converting Int to char* for setStr */
  temp_int = float(read_temp2);
  

  if(temp_int < 0)
    temp_int = 0;

  sprintf(str,"%d", (temp_int - 2));
data += String(str) + delimiter;
     
  LcdWriteString(", Temp2:"); 
  char temp1Str2[10]; // Create a character array to hold the temperature value
  dtostrf(read_temp2, 4, 2, temp1Str2); // Convert the float to a C-string with one decimal place
  LcdWriteString(temp1Str2);
  //LcdWriteString(temp1Str2);  
  LcdWriteString("C");
  //data += String(temp1Str2) + delimiter;
    /*Temperature reading Logic */
    
    /* CO2 Sensor Reading Logic */
   strcpy(computerdata, "R");
   memset(Co2_data, 0, sizeof(Co2_data));
   delay(900);
   j=0;                                       //reset i, we will need it later 
   
    
    Wire.beginTransmission(CO2_I2C_ADDRESS);  //call the circuit by its ID number.
    Wire.write(computerdata);                 //transmit the command that was sent through the serial port.
    Wire.endTransmission();                   //end the I2C data transmission.

    
    if (strcmp(computerdata, "sleep") != 0)
    {                                         //if the command that has been sent is NOT the sleep command, wait the correct amount of time and request data.
                                              //if it is the sleep command, we do nothing. Issuing a sleep command and then requesting data will wake the Co2 sensor.

      delay(time_);                           //wait the correct amount of time for the circuit to complete its instruction. 
       
      Wire.requestFrom(CO2_I2C_ADDRESS, 20, 0);       //call the circuit and request 20 bytes (this may be more than we need)
      
      code = Wire.read();                     //the first byte is the response code, we read this separately.

   
    while (Wire.available() && j < (sizeof(Co2_data) - 1))
    {                                       //are there bytes to receive.
      
      in_char = Wire.read();                //receive a byte.
      Co2_data[j] = (in_char);              //load this byte into our array.
      
      j += 1;                               //incur the counter for the array element.
      if (in_char == 0)
      {                                     //if we see that we have been sent a null command.
        break;                              //exit the while loop.
      }
   
    }
    Co2_data[j] = '\0';
    co2_data = Co2_data;

       
  }
  Co2_int=atoi(Co2_data);
  /*Serial.print("CO2:");
  Serial.println(Co2_int);*/
  data += co2_data;
  data += delimiter;
  serial_event = false;                     //reset the serial event flag
  
  /* LCD Print CO2 sensor */
  int co2_ppm = co2_data.toInt();

  
  //co2_ppm = 2.2777*Co2_int - 845.92;//Yellow 2
  //co2_ppm = 1.4946*Co2_int - 3963.8;//Yellow 1
  //co2_ppm = 2.6299*Co2_int - 325.41;//Green 2
  //co2_ppm = 2.6066*Co2_int + 202.99;//Green 1
  //co2_ppm = 6.1671*Co2_int + 470.95;//Pink
  //co2_ppm = 2.4658*Co2_int - 149.56;//Orange
  //co2_ppm = 0.9331*Co2_int - 36.584;//White 1
  co2_ppm = CO2_CAL_SLOPE * Co2_int + CO2_CAL_OFFSET;

  LcdXY(0,2);
  LcdWriteString("CO2: ");
  sprintf(str, "%d", co2_ppm);
  LcdWriteString(str);
  LcdWriteString("ppm");
    /* CO2 Sensor Reading Logic */
    
    
    /* Soil Moisture Sensor Reading Logic */
  int read_soil = readSoil();
  int smspercent = 0;
  data += String(read_soil) + delimiter;


  /* LCD Print CO2 sensor */

  //smspercent = -0.2212*(read_soil)+130.6; //FOR Yellow 1, Green 2, Pink, Orange ONLY
  //smspercent = -0.2097577945*(read_soil)+122.9301632; //FOR( Green 1)
  //smspercent = -0.1703249011*(read_soil)+100.4838616; //FOR( Yellow 2)
  //smspercent = -0.1697792869*(read_soil)+122.9301632; //FOR( White 1)
  smspercent = SOIL_MOISTURE_SLOPE * read_soil + SOIL_MOISTURE_OFFSET;
  //smspercent = (read_soil /5.6); 

  //smspercent = abs(smspercent - 113); //This is the % value for soil moisture sensor to be printed on LCD. Old formula
  //smspercent = 113-smspercent; //This is the % value for soil moisture sensor to be printed on LCD. New formula 

  //smspercent = 113 - (599/5.6);
  //smspercent = 113 - (50/5.6);
  //smspercent = 113 - (650/5.6);

  // read_soil = pow(E,((read_soil - 944.88)/81.284))*100; 
  // smspercent = read_soil *s100;
  // smspercent = smspercent / 555;
  if(smspercent > 100)
  {
    smspercent = 100;
  }

 else if (smspercent < 0)
 {  
  smspercent = 0;
 }


  sprintf(str, "%d", smspercent);

  
  LcdXY(0,4);
  LcdWriteString("S.M.S: ");
  LcdWriteString(str);
  LcdWriteString("%");

  delay(2000);

    /* Soil Moisture Sensor Reading Logic */ 
  
    /* Light Sensor Reading Logic */
  float UVindex = uv.readUV();
  // the index is multiplied by 100 so to get the
  // integer index, divide by 100!
  UVindex /= 100.0;  
  Serial.print("UV Index: ");
  Serial.println(UVindex);

  LCDClear();

  sprintf(str, "%d", uv.readVisible());
  data += String(str) + delimiter;

  LcdXY(0,0);
  LcdWriteString("Vis: ");
  LcdWriteString(str);
  delay(800);
  sprintf(str, "%d", uv.readIR());
  data += String(str) + delimiter;

   LcdXY(0,2);
  LcdWriteString("IR: ");
  LcdWriteString(str);
  delay(800);
  dtostrf(UVindex, 4, 2, str);
  data += String(str) + delimiter;

  LcdXY(0,4);
  LcdWriteString("UV: ");
  LcdWriteString(str);
   
  delay(2000);
  LCDClear();
    /* Light Sensor Reading Logic */
  
  
  /* SD Card Writing Logic */
  // while(!sd.begin(chipSelect))
  // {
  //     #if SerialStream
  //     Serial.println("error in loop");
  //     #endif
  //     sd.begin(chipSelect);
  //     break;
  // }
  digitalWrite(10, LOW);

  if (sdReady || sd.begin(chipSelect))
  {
        sdReady = true;
        digitalWrite(10, HIGH);
        file.open(fileNameArray, O_CREAT | O_APPEND | O_WRITE);
        file.println(data);
        #if SerialStream
        Serial.println(data);
        #endif
        file.close();
        delay(250);
        digitalWrite(10,LOW);
  }
  else
  {
        Serial.println("Error: Card failed, or not present 1");
  }
}

   /* SD Card Writing Logic */
// function to decode the reading after the read command was issued

void receive_reading(Ezo_board &Sensor)
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
  int readSoil()
{

    digitalWrite(soilPower, HIGH);//turn D7 "On"
    delay(10);//wait 10 milliseconds 
     val = analogRead(soilPin);//Read the SIG value form sensor 
    return val;//send current moisture value
}
