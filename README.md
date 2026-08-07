# Update Tracker
Thanks to Izzy for this suggestion! Here we will be tracking each version of the firmware.
| Version       | "Named" Ver.          | Pilot         | Date                | Description & Purpose                		        |
| ------------- | --------------        | ------------- | -------------       | ----------------------------------------------- |
| V1.0         	| Initial Upload        | Sascha Fowler | July 21, 2026       | Old SPOD Firmware Rework Initial Upload		     	|
| V1.1        	| Sensor Connections    | Sascha Fowler | August 4, 2026      | SPOD code redesign from V1 - Microcontroller moved to Minima, Tested with Yellow 3 SPOD - RTC Update, Serial/File Write, Sensors All Function |		     	|
| V1.2 (In-progress) 	| Calibration Embedding | Sascha Fowler | August 7, 2026      | Embedding calibration into the firmware - Issues with data and upload amounts	|


# Future Request
Firmware Requests:
-  Embed Calibrations - use V1.2 as basis since this is currently in progress
   - Many of the previous declaration are in char/int, so to apply calibration eqs., must convert these values into float
-  Integrate Old LCD Scripts (Legacy Ver. - Main Branch) - Sections for setup() and loop() Found In File "LCD_Legacy.cpp" in Folder
   -  Currently Firmware instigates DFU Issues when Uploaded to Minima
   - Likely Due to Infinite Loop in Firmware (Serial Becomes Backed Up)
     - To fix: Double Press reset on Minima to set into DFU mode - upload without LCD
- Check Firmware for stability issues from Ankit, most code is copy/pasted over to reflect the lab versions
- Check for instability in power issues from pins (CO2 sensor, LCD power, and SI1145)
- Change "filename =" from string to char
- Reduce memory needs by allocating all pin initializations at the start of the setup/loop
- Split sensors into .h and .cpp files to ease troubleshooting in the main .ino

Hardware Requests:
- Sometimes the CO2 sensor will cause Y2K behavior when disconnected while pod is on (Ex: 2000-1-20 --------- 2:00 -----)
  - To fix, must reupload to RTC to Minima
- Sometimes the Adafruit sensor will also cause Y2K behavior when disconnected
   - Double check Orange Pod - seemed to not change values when using V1.1 in late July 2026
- Find new port than ethernet cable to connect the CO2 sensor
- New soil Moisture sensor TBD
- New LCD screen TBD
