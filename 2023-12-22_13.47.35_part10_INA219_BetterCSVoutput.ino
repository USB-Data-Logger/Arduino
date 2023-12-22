#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219_0x40(0x40);
Adafruit_INA219 ina219_0x41(0x41);
Adafruit_INA219 ina219_0x44(0x44);
Adafruit_INA219 ina219_0x45(0x45);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_PIN_18 18
#define BUTTON_PIN_19 19

unsigned long lastDebounceTime_18 = 0;
unsigned long lastDebounceTime_19 = 0;
unsigned long debounceDelay = 20; // Debounce delay
bool buttonState_18 = HIGH;
bool lastButtonState_18 = HIGH;
bool buttonState_19 = HIGH;
bool lastButtonState_19 = HIGH;

unsigned long powerOnTimeStart = 0;
unsigned long projectStartTime = 0;
bool projectTimerActive = false;

int refreshRateIndex = 2; // Start with 100 Hz
const int refreshRates[3] = {1000, 100, 10}; // Delays for 1 Hz, 10 Hz, and 100 Hz
unsigned long lastRefreshTime = 0;

void setup() {
  pinMode(BUTTON_PIN_18, INPUT_PULLUP);
  pinMode(BUTTON_PIN_19, INPUT_PULLUP);
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;); // Loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Initializing...");
  display.display();
  delay(2000); // Pause for 2 seconds

  powerOnTimeStart = millis();

  // Initialize the INA219 sensors
  if (!ina219_0x40.begin()) {
    Serial.println("Failed to find INA219 with address 0x40");
    while (1) { delay(10); }
  }
  if (!ina219_0x41.begin()) {
    Serial.println("Failed to find INA219 with address 0x41");
    while (1) { delay(10); }
  }
  if (!ina219_0x44.begin()) {
    Serial.println("Failed to find INA219 with address 0x44");
    while (1) { delay(10); }
  }
  if (!ina219_0x45.begin()) {
    Serial.println("Failed to find INA219 with address 0x45");
    while (1) { delay(10); }
  }
}

void loop() {
  int reading_18 = digitalRead(BUTTON_PIN_18);
  int reading_19 = digitalRead(BUTTON_PIN_19);

  // Debounce button on pin 18
  if (reading_18 != lastButtonState_18) {
    lastDebounceTime_18 = millis();
  }
  if ((millis() - lastDebounceTime_18) > debounceDelay) {
    if (reading_18 != buttonState_18) {
      buttonState_18 = reading_18;
      if (buttonState_18 == LOW) {
        projectTimerActive = !projectTimerActive;
        projectStartTime = millis();
      }
    }
  }
  lastButtonState_18 = reading_18;

  // Debounce button on pin 19
  if (reading_19 != lastButtonState_19) {
    lastDebounceTime_19 = millis();
  }
  if ((millis() - lastDebounceTime_19) > debounceDelay) {
    if (reading_19 != buttonState_19) {
      buttonState_19 = reading_19;
      if (buttonState_19 == LOW) {
        refreshRateIndex = (refreshRateIndex + 1) % 3;
      }
    }
  }
  lastButtonState_19 = reading_19;

  // Perform the refresh action at the set refresh rate
  if (millis() - lastRefreshTime >= refreshRates[refreshRateIndex]) {
    lastRefreshTime = millis();

    // Start CSV Line
    Serial.print("On time,");
    printElapsedTime(millis() - powerOnTimeStart);
    Serial.print("Project Time,");
    printElapsedTime(projectTimerActive ? millis() - projectStartTime : 0);

    // Reading and printing sensor data for each sensor
    readAndPrintSensorData(ina219_0x40, "Sensor 1");
    readAndPrintSensorData(ina219_0x41, "Sensor 2");
    readAndPrintSensorData(ina219_0x44, "Sensor 3");
    readAndPrintSensorData(ina219_0x45, "Sensor 4");

    // End CSV Line
    Serial.println();

    // Update OLED display
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Rate: ");
    switch(refreshRateIndex) {
      case 0: display.print("1Hz"); break;
      case 1: display.print("10Hz"); break;
      case 2: display.print("100Hz"); break;
    }
    display.setCursor(0, 10);
    display.print("Baud: 115200");
    display.setCursor(0, 20);
    display.print("Proj Start: ");
    display.println(projectTimerActive ? "On" : "Off");
    display.display();
  }
}

void readAndPrintSensorData(Adafruit_INA219& sensor, const String& sensorName) {
  float shuntvoltage = sensor.getShuntVoltage_mV();
  float busvoltage = sensor.getBusVoltage_V();
  float current_mA = sensor.getCurrent_mA();
  float power_mW = sensor.getPower_mW();

  Serial.print(", " + sensorName + ", ");
  Serial.print("Shunt Voltage (mV)," + String(shuntvoltage) + ", ");
  Serial.print("Bus Voltage (V)," + String(busvoltage) + ", ");
  Serial.print("Current (A)," + String(current_mA / 1000.0) + ", ");
  Serial.print("Power (W)," + String(power_mW / 1000.0));
}

void printElapsedTime(unsigned long time) {
  unsigned int hours = time / 3600000;
  unsigned int mins = (time % 3600000) / 60000;
  unsigned int secs = (time % 60000) / 1000;
  unsigned int msecs = time % 1000;

  Serial.print(hours < 10 ? "0" : "");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(mins < 10 ? "0" : "");
  Serial.print(mins);
  Serial.print(":");
  Serial.print(secs < 10 ? "0" : "");
  Serial.print(secs);
  Serial.print(".");
  Serial.print(msecs);
  Serial.print(", ");
}
