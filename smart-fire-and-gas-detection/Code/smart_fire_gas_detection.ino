#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME "Smart Fire Gas Detection"
#define BLYNK_AUTH_TOKEN ""
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Wokwi Wi-Fi defaults. Replace with your network details for hardware.
const char WIFI_SSID[] = "Wokwi-GUEST";
const char WIFI_PASS[] = "";

#define DHT_PIN 4
#define DHT_TYPE DHT22
#define MQ_PIN 34
#define LDR_PIN 32

#define GREEN_LED 27
#define YELLOW_LED 26
#define RED_LED 25
#define BUZZER 14

#define VP_TEMP V0
#define VP_HUM V1
#define VP_GAS V2
#define VP_LIGHT V3
#define VP_FLAME_STATUS V4
#define VP_ALARM V5
#define VP_MUTE V6
#define VP_STATUS V7

DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Demo thresholds only. Calibrate against the actual sensor and environment.
const float TEMP_WARNING = 30.0;
const float TEMP_DANGER = 33.0;
const float HUM_WARNING = 75.0;
const float HUM_DANGER = 85.0;
const int GAS_WARNING = 3300;
const int GAS_DANGER = 3800;
const int LIGHT_WARNING = 2500;
const int LIGHT_DANGER = 3200;

float temperature = 0.0;
float humidity = 0.0;
int gasValue = 0;
int lightValue = 0;

bool dhtValid = false;
bool warningDetected = false;
bool dangerDetected = false;
bool lightAlarm = false;
bool tempDanger = false;
bool humDanger = false;
bool gasDanger = false;
bool buzzerMuted = false;
bool previousDanger = false;

unsigned long previousLCD = 0;
unsigned long previousSerial = 0;
unsigned long previousBlynk = 0;
unsigned long lastEventTime = 0;
unsigned long lastWifiTry = 0;
unsigned long lastBlynkTry = 0;

const unsigned long INTERVAL = 2000;
const unsigned long BLYNK_INTERVAL = 2000;
const unsigned long EVENT_REPEAT = 60000;
const unsigned long WIFI_RETRY = 15000;
const unsigned long BLYNK_RETRY = 10000;

int displayPage = 0;

BLYNK_CONNECTED() {
  Blynk.syncVirtual(VP_MUTE);
}

BLYNK_WRITE(VP_MUTE) {
  buzzerMuted = (param.asInt() == 1);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  noTone(BUZZER);

  dht.begin();
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("SMART FIRE/GAS");
  lcd.setCursor(0, 1);
  lcd.print("DETECTION SYSTEM");

  Serial.println();
  Serial.println("SMART FIRE AND GAS DETECTION SYSTEM + BLYNK");
  Serial.println("ESP32 sensor monitoring started");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS, 6);
  Blynk.config(BLYNK_AUTH_TOKEN);

  delay(2000);
  lcd.clear();
}

void readSensors() {
  float newTemperature = dht.readTemperature();
  float newHumidity = dht.readHumidity();

  dhtValid = !isnan(newTemperature) && !isnan(newHumidity);
  if (dhtValid) {
    temperature = newTemperature;
    humidity = newHumidity;
  }

  gasValue = analogRead(MQ_PIN);
  lightValue = analogRead(LDR_PIN);
}

void evaluateSensors() {
  warningDetected = false;
  dangerDetected = false;
  tempDanger = false;
  humDanger = false;
  gasDanger = false;
  lightAlarm = false;

  if (dhtValid) {
    if (temperature >= TEMP_DANGER) {
      dangerDetected = true;
      tempDanger = true;
    }
    if (humidity >= HUM_DANGER) {
      dangerDetected = true;
      humDanger = true;
    }
    if (!dangerDetected &&
        (temperature >= TEMP_WARNING || humidity >= HUM_WARNING)) {
      warningDetected = true;
    }
  }

  if (gasValue >= GAS_DANGER) {
    dangerDetected = true;
    gasDanger = true;
  } else if (gasValue >= GAS_WARNING) {
    warningDetected = true;
  }

  // LDR indicates high light level; it does not confirm a flame.
  if (lightValue >= LIGHT_DANGER) {
    dangerDetected = true;
    lightAlarm = true;
  } else if (lightValue >= LIGHT_WARNING) {
    warningDetected = true;
  }
}

void updateOutputs(String &ledStatus, String &buzzerStatus,
                   String &systemStatus, int &alarmState) {
  if (dangerDetected) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    if (buzzerMuted) {
      noTone(BUZZER);
      buzzerStatus = "MUTED (app)";
    } else {
      tone(BUZZER, 2000);
      buzzerStatus = "ON";
    }

    ledStatus = "RED";
    systemStatus = "DANGER";
    alarmState = 2;
  } else if (warningDetected) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);

    ledStatus = "YELLOW";
    buzzerStatus = "OFF";
    systemStatus = "WARNING";
    alarmState = 1;
  } else {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);

    ledStatus = "GREEN";
    buzzerStatus = "OFF";
    systemStatus = "NORMAL";
    alarmState = 0;
  }
}

void sendBlynkData(int alarmState, const String &systemStatus,
                   unsigned long currentTime) {
  if (!Blynk.connected() ||
      currentTime - previousBlynk < BLYNK_INTERVAL) return;

  previousBlynk = currentTime;

  if (dhtValid) {
    Blynk.virtualWrite(VP_TEMP, temperature);
    Blynk.virtualWrite(VP_HUM, humidity);
  }
  Blynk.virtualWrite(VP_GAS, gasValue);
  Blynk.virtualWrite(VP_LIGHT, lightValue);
  Blynk.virtualWrite(VP_FLAME_STATUS, lightAlarm ? 1 : 0);
  Blynk.virtualWrite(VP_ALARM, alarmState);
  Blynk.virtualWrite(VP_STATUS, systemStatus);
}

void sendDangerEvent(unsigned long currentTime) {
  if (!dangerDetected || !Blynk.connected()) {
    previousDanger = dangerDetected;
    return;
  }

  bool justStarted = !previousDanger;
  bool repeatDue = currentTime - lastEventTime >= EVENT_REPEAT;

  if (justStarted || repeatDue) {
    String message = "DANGER detected:";
    if (gasDanger) message += " Gas ADC=" + String(gasValue);
    if (lightAlarm) message += " High light ADC=" + String(lightValue);
    if (tempDanger) message += " Temp=" + String(temperature, 1) + "C";
    if (humDanger) message += " Humidity=" + String(humidity, 0) + "%";

    Blynk.logEvent("danger_alert", message);
    lastEventTime = currentTime;
    Serial.println("Blynk event sent: " + message);
  }
  previousDanger = dangerDetected;
}

void updateLCD(unsigned long currentTime) {
  if (currentTime - previousLCD < INTERVAL) return;
  previousLCD = currentTime;

  lcd.clear();

  if (displayPage == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    if (dhtValid) {
      lcd.print(temperature, 1);
      lcd.print((char)223);
      lcd.print("C");
    } else {
      lcd.print("Sensor Error");
    }

    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    if (dhtValid) {
      lcd.print(humidity, 0);
      lcd.print("%");
    } else {
      lcd.print("--");
    }
  } else if (displayPage == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Gas: ");
    lcd.print(gasValue);
    lcd.setCursor(0, 1);
    lcd.print("Light: ");
    lcd.print(lightValue);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("System Status:");
    lcd.setCursor(0, 1);
    if (dangerDetected) lcd.print("DANGER DETECTED");
    else if (warningDetected) lcd.print("WARNING");
    else lcd.print("NORMAL");
  }

  displayPage = (displayPage + 1) % 3;
}

void printSerialStatus(unsigned long currentTime, const String &ledStatus,
                       const String &buzzerStatus, const String &systemStatus) {
  if (currentTime - previousSerial < INTERVAL) return;
  previousSerial = currentTime;

  Serial.println();
  Serial.println("==============================================");
  Serial.print("Time: ");
  Serial.print(currentTime / 1000);
  Serial.println(" seconds");

  Serial.println("--- Sensor readings ---");
  Serial.print("Temperature: ");
  if (dhtValid) Serial.print(String(temperature, 1) + " C");
  else Serial.print("INVALID - Check DHT22");
  Serial.println();

  Serial.print("Humidity: ");
  if (dhtValid) Serial.print(String(humidity, 1) + " %");
  else Serial.print("INVALID - Check DHT22");
  Serial.println();

  Serial.print("Gas ADC: ");
  Serial.print(gasValue);
  Serial.println(" / 4095");

  Serial.print("Light ADC: ");
  Serial.print(lightValue);
  Serial.println(" / 4095");

  Serial.println("--- System outputs ---");
  Serial.println("LED: " + ledStatus);
  Serial.println("Buzzer: " + buzzerStatus);
  Serial.println("Overall: " + systemStatus);
  Serial.println(WiFi.status() == WL_CONNECTED ? "WiFi: CONNECTED" : "WiFi: DISCONNECTED");
  Serial.println(Blynk.connected() ? "Blynk: CONNECTED" : "Blynk: DISCONNECTED");
  Serial.println("==============================================");
}

void loop() {
  unsigned long currentTime = millis();

  // Maintain network connections without making local sensing depend on them.
  if (WiFi.status() == WL_CONNECTED) {
    if (Blynk.connected()) {
      Blynk.run();
    } else if (currentTime - lastBlynkTry >= BLYNK_RETRY) {
      lastBlynkTry = currentTime;
      Blynk.connect(3000);
    }
  } else if (currentTime - lastWifiTry >= WIFI_RETRY) {
    lastWifiTry = currentTime;
    WiFi.reconnect();
  }

  readSensors();
  evaluateSensors();

  String ledStatus, buzzerStatus, systemStatus;
  int alarmState = 0;
  updateOutputs(ledStatus, buzzerStatus, systemStatus, alarmState);

  sendBlynkData(alarmState, systemStatus, currentTime);
  sendDangerEvent(currentTime);
  updateLCD(currentTime);
  printSerialStatus(currentTime, ledStatus, buzzerStatus, systemStatus);

  delay(200);
}
