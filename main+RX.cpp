#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HX711_ADC.h>
#include <VirtualWire.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

// HX711 pins
const int HX711_dout = 4; // MCU > HX711 dout pin
const int HX711_sck = 5; // MCU > HX711 sck pin
HX711_ADC LoadCell(HX711_dout, HX711_sck);

// EEPROM address for calibration value
const int calVal_eepromAddress = 0;
unsigned long t = 0;

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// VirtualWire configuration
#define ledPin 6
#define signalPin 11

bool weightDetected = false;
bool pillionDetected = false;
bool signal1Received = false;
bool signal2Received = false;

void setup() {
  // Serial communication setup
  Serial.begin(57600); 
  delay(10);
  Serial.println();
  Serial.println("Starting...");

  // Load cell initialization
  LoadCell.begin();
  float calibrationValue = 696.0; // Calibration value (can be set based on Calibration.ino)
#if defined(ESP8266)|| defined(ESP32)
  // EEPROM.begin(512); // Uncomment if using ESP8266/ESP32 and fetching calibration from EEPROM
#endif
  // EEPROM.get(calVal_eepromAddress, calibrationValue); // Uncomment to fetch calibration value from EEPROM

  unsigned long stabilizingTime = 2000; // Stabilizing time after power-up
  boolean tare = true; // Perform tare
  LoadCell.start(stabilizingTime, tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU > HX711 wiring and pin designations");
    while (1);
  } else {
    LoadCell.setCalFactor(calibrationValue); // Set calibration value
    Serial.println("Startup is complete");
  }

  // LCD initialization
  lcd.init();
  lcd.backlight(); // Turn on the backlight

  // VirtualWire initialization
  vw_set_rx_pin(signalPin);
  vw_setup(2000); // Bits per sec
  pinMode(ledPin, OUTPUT);
  vw_rx_start(); // Start the receiver PLL running
}

void loop() {
  // Check for new data from load cell
  LoadCell.update(); // Retrieves data from the load cell
  float weight = LoadCell.getData(); // Get output value
  
  // Display weight on LCD
  lcd.setCursor(0, 0); // Set cursor to first row
  lcd.print("                "); // Clear the line
  lcd.setCursor(0, 0); // Set cursor to first row again

  if (weight > 0) {
    weightDetected = true;
    if (weight > 80000) { // Weight in grams (80 kg)
      lcd.print("Pillion Detected");
      pillionDetected = true;
    } else {
      lcd.print("Weight Detected");
      pillionDetected = false;
    }
  } else {
    lcd.print("No Weight Detected");
    weightDetected = false;
    pillionDetected = false;
  }

  lcd.setCursor(0, 1); // Set cursor to second row
  lcd.print("Weight: ");
  lcd.print(weight / 1000.0); // Display weight in kg
  lcd.print(" kg");

  // Optional: Serial print to monitor
  Serial.print("Weight: ");
  Serial.print(weight / 1000.0); // Print weight in kg
  Serial.println(" kg");

  // VirtualWire message handling
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN; 

  if (vw_get_message(buf, &buflen)) {
    if (buf[0] == 'a') {  
      signal1Received = true;
    } else if (buf[0] == 'b') {
      signal1Received = false;
    } else if (buf[0] == 'c') {
      signal2Received = true;
    } else if (buf[0] == 'd') {
      signal2Received = false;
    }
  }

  // Turn on the LED if:
  // 1. Pillion is detected and both signals are received
  // 2. No pillion is detected but weight is detected and signal 1 is received
  if ((pillionDetected && signal1Received && signal2Received) ||
      (!pillionDetected && weightDetected && signal1Received)) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  // Delay to avoid flooding the serial monitor and LCD
  delay(500);
}