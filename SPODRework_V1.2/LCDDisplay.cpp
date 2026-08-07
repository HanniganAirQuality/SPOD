//Code for Uploading to the LCD Screen

//Header Section
/*  Header Files for LCD Screen  */
#if LCDDisplay
  #include "LCD_PCD8544.h"      //PCD8544 controller  datasheet   84x48 pixel black and white LCDs
                                //https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf
#endif

//setup() section //ERROR HAPPENS IN SETUP?????
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
    LcdWriteString('SPOD' + spodID);
    delay(2000);
    LCDClear();
  #endif

  //loop() sections

  #if LCDDisplay
    static int cursorX = 0;
    static int cursorY = 0;
  #endif

  #if LCDDisplay //LCD SCREEN Print (legacy)
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
    #endif
    delay(1000);
    LCDClear();

    // Temp 1 && Temp 2
    #if TEMPERATURE_ONE_ENABLED || TEMPERATURE_TWO_ENABLED
      LcdXY(0, 0);
      #if TEMPERATURE_ONE_ENABLED && !TEMPERATURE_TWO_ENABLED
        LcdWriteString("Temp1:");
        LcdWriteString(temp1Str);
        LcdWriteString("C");
        LcdWriteString("  Temp2:");
        LcdWriteString("NoSensor");
        delay(800);
      #elif TEMPERATURE_TWO_ENABLED && !TEMPERATURE_ONE_ENABLED
        LcdWriteString("Temp1:");
        LcdWriteString("NoSensor");
        LcdWriteString("  Temp2:");
        LcdWriteString(temp2Str);
        LcdWriteString("C");
        delay(800);
      #else
        LcdWriteString("Temp1:");
        LcdWriteString(temp1Str);
        LcdWriteString("C");
        delay(800);
        LcdWriteString("  Temp2:");
        LcdWriteString(temp2Str);
        LcdWriteString("C");
        delay(800);
      #endif
    #endif

    //Soil moisture
    #if MOISTURE_ENABLED
      char moistureStr[10];
      sprintf(moistureStr, "%d", smspercent);
      LcdXY(0, 3);
      LcdWriteString("S.M.S: ");
      LcdWriteString(moistureStr);
      LcdWriteString("%");
      delay(800);
    #endif

    // CO2
    #if CO2_ENABLED
      LcdXY(0, 5);
      LcdWriteString("CO2: ");
      sprintf(str, "%d", Co2_data);
      LcdWriteString(str);
      LcdWriteString("ppm");
    #endif
    delay(1000);
    LCDClear();
  #endif

  /* Note: See that the LCD for the temperature used to be in the conditional declaration, I moved this into the LCD section in the loop, so see what was commented out - might be place where error occured
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
    if (temp_int < 0){
      temp_int = 0;
    }
    sprintf(str, "%d", (temp_int - 2));
    //LcdXY(0, 0);                          // FOR OLD DISPLAY FILE
    //LcdWriteString("Temp1:");             // FOR OLD DISPLAY FILE
    char temp1Str[10];                    // Create a character array to hold the temperature value
    dtostrf(read_temp1, 4, 2, temp1Str);  // Convert the float to a C-string with one decimal place
    //LcdWriteString(temp1Str);             // FOR OLD DISPLAY FILE
    //LcdWriteString(String(read_temp1));
    //LcdWriteString("C");  // FOR OLD DISPLAY FILE
  #endif
  */