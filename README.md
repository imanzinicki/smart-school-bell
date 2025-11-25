1. Project Overview
The Smart School Bell System is an automated and manual bell-ringing system built using an ESP32, Blynk IoT, NTP time, LCD display, and manual push button.
It rings the school bell:
✔ Automatically according to a daily schedule
✔ Manually using a physical button
✔ Manually using the Blynk mobile app
The system also displays real-time clock information on an LCD
                                 Hardware component used:
Component	Description
ESP32	Main controller + Wi-Fi
Relay module	Powers/activates the bell
Buzzer	Beeps while bell is ringing
Push Button	Manual trigger
LCD 16x2 (I2C)	Shows system messages
Power Supply 5V	For relay + ESP32


                                   3. Software Used
Software	Purpose
Arduino IDE	Programming ESP32
Blynk IoT	Remote control
NTP Server	


WiFi.h & WiFiClient.h
Used to connect ESP32 to WiFi.
BlynkSimpleEsp32.h
Allows using the Blynk mobile app to control the ESP32.
time.h
Gets current time from the internet using NTP.
LiquidCrystal_I2C.h
Used to control the 16x2 LCD screen with I2C connection.
                                                   Important Definitions
Blynk Credentials
 
These tell Blynk which project your ESP32 belongs to.

  WiFi Credentials
 
Used to connect ESP32 to WiFi.

                                             Pin Configuration
PIN	Function
26	Relay control
27	Buzzer
25	Button

                             6. LCD Display Function
                          showLCD ()

                                 Prints two lines on the LCD.
 

