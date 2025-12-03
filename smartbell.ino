
#define BLYNK_TEMPLATE_ID "TMPL2U6mvO8JT"
#define BLYNK_TEMPLATE_NAME "iot based smart school bell"
#define BLYNK_AUTH_TOKEN "pjNDyHQCIGYluwX5MqeyTVFtD6XEv4g_"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "time.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "Theo";
char pass[] = "";

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define RELAY_PIN 26
#define BUTTON_PIN 25
#define BUZZER_PIN 27

BlynkTimer timer;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7200;
const int daylightOffset_sec = 0;


String bellTimes[] = {
  "05:30",
  "06:00",
  "07:00",
  "08:00",
  "08:40",
  "08:45",
  "09:30",
  "09:10",
  "10:00",
  "10:20",
  "11:00",
  "11:40",
  "12:20",
  "14:00",
  "14:40",
  "15:20",
  "16:00",
  "17:35"
};
int numBells = sizeof(bellTimes) / sizeof(bellTimes[0]);
bool bellRang = false;



void showLCD(String L1, String L2) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(L1);
  lcd.setCursor(0, 1); lcd.print(L2);
}



void ringBell() {
  showLCD("BELL RINGING", "Please wait...");
  Serial.println("Bell ringing...");

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(9000);

  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  showLCD("BELL DONE", "Ready");
  delay(1500);
}



void stopbell() {
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
}



void checkButton() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    showLCD("BUTTON PRESSED", "Manual Bell");
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(BUZZER_PIN, HIGH);
    
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);
  }
}


void displayTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    showLCD("TIME ERROR", "NTP FAIL!");
    return;
  }

  char timeString[16];
  strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);

  lcd.setCursor(0, 0);
  lcd.print("Time:");
  lcd.print(timeString);
  lcd.print(" ");

  lcd.setCursor(0, 1);
  lcd.print("Smart AutoBell ");
}


// ---------------- BELL SCHEDULER ----------------
void checkSchedule() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  char currentTime[6];
  strftime(currentTime, sizeof(currentTime), "%H:%M", &timeinfo);
  String now = String(currentTime);

  for (int i = 0; i < numBells; i++) {
    if (now == bellTimes[i]) {
      if (!bellRang) {
        showLCD("AUTO MODE", "Bell Ringing");
        ringBell();
        bellRang = true;
      }
      return;
    }
  }

  static String lastMinute = "";
  if (now != lastMinute) {
    bellRang = false;
    lastMinute = now;
  }
}


BLYNK_WRITE(V1) {
  int value = param.asInt();
  if (value == 1) {
    showLCD("BLYNK BUTTON", "Manual Bell");
    ringBell();
  }
}



void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN,INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.init();
  lcd.backlight();

  showLCD("WELCOME TO", "SMART AUTO BELL");
  delay(2000);

  showLCD("SYSTEM", "INITIALIZING...");
  delay(1500);

  showLCD("Connecting", "WiFi...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    lcd.setCursor(0, 1);
    lcd.print("WiFi...");
  }

  showLCD("WiFi OK", WiFi.localIP().toString());
  delay(1500);

  showLCD("Connecting", "Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  showLCD("SYNCING TIME", "Please wait...");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(1500);

  showLCD("TIME SYNCED", "SYSTEM READY!");
  delay(2000);

  timer.setInterval(1000L, displayTime);
  timer.setInterval(300L, checkButton);
  timer.setInterval(5000L, checkSchedule);
}



void loop() {
  Blynk.run();
  timer.run();
}
