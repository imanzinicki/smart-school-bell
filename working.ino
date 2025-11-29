#define BLYNK_TEMPLATE_ID "TMPL2rzVvh0n2"
#define BLYNK_TEMPLATE_NAME "iot based water monitoring system"
#define BLYNK_AUTH_TOKEN "H5XQZAf5nw-vZsF_LwE9Cxkb7ZM6R713"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "Hyacinthe";
char pass[] = "12345678";

#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pHSensorPin = A0;
const float adcMax = 1023.0;
const float vRef = 1.0;
float neutralPH = 7.0;
float neutralVoltageTarget = 2.50;
float slope = 0.059;
const int CAL_READS = 30;
const int AVG_COUNT = 8;
float dividerFactor = 3.2;
float neutralVoltage = 2.50;
float avgBuffer[AVG_COUNT];
int bufIdx = 0;

#define RED_LED D5
#define YELLOW_LED D6
#define GREEN_LED D7
#define BUZZER D8

//------------------------------------------------------------
// BLYNK BUZZER CONTROL
//------------------------------------------------------------
BLYNK_WRITE(V3) {
  int value = param.asInt();
  digitalWrite(BUZZER, value ? HIGH : LOW);

  if (value == 1) {
    for (int i = 0; i < 2; i++) {
      digitalWrite(RED_LED, HIGH);
      delay(4000);
      digitalWrite(RED_LED, LOW);
      delay(4000);
    }
    digitalWrite(RED_LED, HIGH);
  } else {
    digitalWrite(RED_LED, LOW);
  }
}

float readProbeVoltage();
float runningAverageVoltage(float newV);
float readPH();

//------------------------------------------------------------
// SETUP
//------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(200);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WELCOME TO IOT");
  lcd.setCursor(0,1);
  lcd.print("WATER MONITORING");
  digitalWrite(RED_LED, HIGH);
  delay(3000);
  digitalWrite(RED_LED, LOW);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Using Sensors:");
  lcd.setCursor(0,1);
  lcd.print("Temp + pH Sensor");
  delay(3000);
  lcd.clear();

  sensors.begin();

  // -------------------------------
  // FIXED WIFI + BLYNK CONNECTION
  // -------------------------------
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // -------------------------------

  // pH calibration
  float sumAdcV = 0.0;
  for (int i = 0; i < CAL_READS; ++i) {
    int adc = analogRead(pHSensorPin);
    float adcV = ((float)adc / adcMax) * vRef;
    sumAdcV += adcV;
    delay(120);
  }
  float avgAdcV = sumAdcV / CAL_READS;
  dividerFactor = neutralVoltageTarget / avgAdcV;
  neutralVoltage = neutralVoltageTarget;

  for (int i = 0; i < AVG_COUNT; i++) avgBuffer[i] = avgAdcV * dividerFactor;
}

//------------------------------------------------------------
// LOOP
//------------------------------------------------------------
void loop() {
  Blynk.run();

  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  float pH = readPH();

  // LCD Display
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(tempC,1);
  lcd.print("C   ");

  lcd.setCursor(0,1);
  lcd.print("pH:");
  lcd.print(pH,2);
  lcd.print("     ");

  // Wi-Fi indicator
  lcd.setCursor(13,0);
  if (WiFi.status() != WL_CONNECTED) lcd.print("CN");
  else lcd.print("OK");

  // LED Logic
  if (pH < 6.5) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
  }
  else if (pH >= 6.5 && pH <= 7.5) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
  }
  else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(GREEN_LED_
