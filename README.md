# Update Tracker
Thanks to Izzy for this suggestion! Here we will be tracking each version of the firmware.
| Version       | "Named" Ver.          | Pilot         | Date                | Description & Purpose                		        |
| ------------- | --------------        | ------------- | -------------       | ----------------------------------------------- |
| V1.0         	| Initial Upload        | Sascha Fowler | July 21, 2026       | Old SPOD Firmware Rework Initial Upload		     	|
| V1.1        	| Sensor Connections    | Sascha Fowler | August 4, 2026      | SPOD code redesign from V1 - Microcontroller moved to Minima, Tested with Yellow 3 SPOD - RTC Update, Serial/File Write, Sensors All Function |		     	|
| V1.2   	| Calibration Embedding | Sascha Fowler | August 7, 2026      | Embedding calibration into the firmware - Issues with data and upload amounts	|
| V2.0   	| LCD Upgrade | Alexander Hansen | October 5, 2026      | Initial script meant to upgrade to LCD ST7735 to replace obselesced module.	|
| V3.0   	| ST7735/SD & Format | Percy Smith | October 14, 2026      | Overall translation to lab formatting. LCD_Module script - bro is limping to functionality; I have learned only certain y coords work!! We are in hell but for now only 3 lines display on the LCD	|


# Calibration Configuration

SPOD V1.2 includes embedded calibration support:

- `spodID` in `SPOD_V1.2/SPOD_node.h` is the only POD selection.
- `SPOD_V1.2/calibration.h` contains all calibration functions and POD-specific CO2 equations fitted by the sibling `SPOD-Calibration` workflow against the LI-840A, with a raw-value fallback for unsupported PODs. Channels without fitted equations are returned unchanged.
- Orange is intentionally excluded because its July CO2 record contains false zeros; it logs raw CO2 until the sensor problem is corrected and new calibration data are collected.
- `CALIBRATION` in `SPOD_V1.2/SPOD_node.h` controls whether calibration is applied. It is enabled for the fitted July 2026 CO2 equations.
- SD and serial rows preserve both values in separate adjacent columns: `Uncalibrated_CO2`, `Calibrated_CO2`, `Uncalibrated_Soil`, and `Calibrated_Soil`. The LCD displays the calibrated columns.

When preparing firmware for another POD, change only `spodID`.

# Sensor Modules

SPOD V1.2 keeps hardware-specific sensor code out of the main sketch:

- `light_sensor.h/.cpp`: SI1145 visible-light, infrared, and UV readings.
- `moisture_sensor.h/.cpp`: soil-sensor power switching and raw ADC readings.
- `temperature_sensors.h/.cpp`: both EZO RTD temperature circuits.
- `ph_sensor.h/.cpp`: optional EZO pH readings and temperature compensation.
- `co2_sensor.h/.cpp`: EZO CO2 commands, response parsing, and validation.

`SPOD_V1.2.ino` coordinates typed sensor results, calibration, LCD display, SD logging, and numeric-only serial output. Disabled sensors and failed readings leave their CSV fields empty.

# Future Request

Firmware Requests:

-  Integrate Old LCD Scripts (Legacy Ver. - Main Branch) - Sections for setup() and loop() Found In File "LCD_Legacy.cpp" in Folder
   -  Currently Firmware instigates DFU Issues when Uploaded to Minima
   - Likely Due to Infinite Loop in Firmware (Serial Becomes Backed Up)
     - To fix: Double Press reset on Minima to set into DFU mode - upload without LCD
- Check Firmware for stability issues from Ankit, most code is copy/pasted over to reflect the lab versions
- Check for instability in power issues from pins (CO2 sensor, LCD power, and SI1145)
- Change "filename =" from string to char
- Reduce memory needs by allocating all pin initializations at the start of the setup/loop

Hardware Requests:

- Sometimes the CO2 sensor will cause Y2K behavior when disconnected while pod is on (Ex: 2000-1-20 --------- 2:00 -----)
  - To fix, must reupload to RTC to Minima
- Sometimes the Adafruit sensor will also cause Y2K behavior when disconnected
   - Double check Orange Pod - seemed to not change values when using V1.1 in late July 2026
- Find new port than ethernet cable to connect the CO2 sensor
- New soil Moisture sensor TBD
- New LCD screen TBD
