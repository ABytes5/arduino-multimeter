// Required libraries for OLED display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin assignments
const int select_button = 2;
const int right_button = 3;
const int OhmMeter = A0;
const int CapacitancMeter = A1;
const int VoltMeter = A2;
const int continuityPin = A3;
const int R1 = 4;
const int R2 = 5;
const int R3 = 6;
const int ChargePin = 13;
const int DischargePin = 11;
const int buzzerPin = 8;

// Global variables
boolean is_select = false;
int navigator = 0;
float R = 0.00, V = 0.00, C = 0.00;
boolean kilo = false, nano = false;
bool continuityDetected = false;

// Initialize OLED
void OLED_init() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display_clear();
}

void display_clear() {
  display.clearDisplay();
  display.display();
}

// Display text on OLED
void display_text(int sz, int x, int y, String str) {
  display.setTextSize(sz);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(str);
}

// Display number on OLED
void display_number(int sz, int x, int y, double num) {
  display.setTextSize(sz);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(num);
}

// Measure resistance using three known reference resistors
void calculate_resistor() {
  const float v_ref = 4.94;
  const float r_ref[3] = {1000.0, 10000.0, 100000.0};
  float readings[3] = {0};
  int pins[3] = {R1, R2, R3};

  for (int i = 0; i < 3; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], HIGH);
    for (int j = 0; j < 20; j++) {
      readings[i] += analogRead(OhmMeter);
      delay(3);
    }
    readings[i] /= 20;
    pinMode(pins[i], INPUT);
  }

  float resistors[3];
  for (int i = 0; i < 3; i++) {
    if (readings[i] < 1022.9) {
      float voltage = readings[i] * v_ref / 1024.0;
      resistors[i] = (voltage * r_ref[i]) / (v_ref - voltage) / 1000.0;
    } else {
      resistors[i] = 0.0;
    }
  }

  if (resistors[0] < 2 && resistors[1] < 101 && resistors[2] < 1001) R = resistors[0] * 1000;
  else if (resistors[1] < 101) R = resistors[1];
  else if (resistors[2] < 2000) R = resistors[2];
  else R = 0.0;

  kilo = (R >= 1);
  if (!kilo) R *= 1000;
}

// Measure capacitance using RC time constant method
void calculate_capacitance() {
  const float r_ref = 10000.0;
  unsigned long start_time = millis();

  digitalWrite(ChargePin, HIGH);
  while (analogRead(CapacitancMeter) < 648);
  unsigned long elapsed_time = millis() - start_time;

  float microFarads = (float)elapsed_time / r_ref * 1000.0;
  if (microFarads > 1.0) {
    C = microFarads;
    nano = false;
  } else {
    C = microFarads * 1000.0;
    nano = true;
  }

  digitalWrite(ChargePin, LOW);
  pinMode(DischargePin, OUTPUT);
  digitalWrite(DischargePin, LOW);
  while (analogRead(CapacitancMeter) > 0);
  pinMode(DischargePin, INPUT);
}

// Measure voltage using voltage divider
void calculate_voltage() {
  const float R1 = 10000.0, R2 = 5100.0, v_ref = 5.0;
  float adc_value = 0.0;

  for (int i = 0; i < 20; i++) {
    adc_value += analogRead(VoltMeter);
    delay(3);
  }
  adc_value /= 20;

  float voltage = (adc_value * v_ref) / 1024.0;
  V = voltage / (R2 / (R1 + R2));
}

// Check continuity and activate buzzer
void calculate_continuity() {
  continuityDetected = analogRead(continuityPin) > 100;
  digitalWrite(buzzerPin, continuityDetected ? HIGH : LOW);
}

void setup() {
  Serial.begin(9600);
  OLED_init();

  pinMode(right_button, INPUT_PULLUP);
  pinMode(select_button, INPUT_PULLUP);
  pinMode(ChargePin, OUTPUT);
  digitalWrite(ChargePin, LOW);

  pinMode(continuityPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  // Handle mode switching
  if (digitalRead(right_button) == LOW) {
    navigator = (navigator + 1) % 4;
    while (digitalRead(right_button) == LOW);
    delay(5);
    Serial.println(navigator);
  }

  if (digitalRead(select_button) == LOW) {
    is_select = true;
    while (digitalRead(select_button) == LOW);
  }

  // Resistor Mode
  if (navigator == 0) {
    display.clearDisplay();
    display_text(2, 17, 8, "Resistor");
    display.display();
    while (is_select) {
      calculate_resistor();
      display.clearDisplay();
      display_text(1, 0, 0, "Resistor");
      display_text(2, 12, 8, "R=");
      display_number(2, 42, 8, R);
      if (kilo) display_text(1, 115, 15, "k");
      display.display();
      if (digitalRead(select_button) == LOW) {
        is_select = false;
        while (digitalRead(select_button) == LOW);
      }
    }
  }

  // Voltage Mode
  else if (navigator == 1) {
    display.clearDisplay();
    display_text(2, 17, 8, "Voltage");
    display.display();
    while (is_select) {
      calculate_voltage();
      display.clearDisplay();
      display_text(1, 0, 0, "Voltage");
      display_text(2, 12, 8, "V=");
      display_number(2, 42, 8, V);
      display_text(1, 115, 15, "v");
      display.display();
      if (digitalRead(select_button) == LOW) {
        is_select = false;
        while (digitalRead(select_button) == LOW);
      }
    }
  }

  // Capacitor Mode
  else if (navigator == 2) {
    display.clearDisplay();
    display_text(2, 12, 8, "Capacitor");
    display.display();
    while (is_select) {
      calculate_capacitance();
      display.clearDisplay();
      display_text(1, 0, 0, "Capacitor");
      display_text(2, 12, 8, "C=");
      display_number(2, 42, 8, C);
      display_text(1, 115, 22, nano ? "nF" : "uF");
      display.display();
      if (digitalRead(select_button) == LOW) {
        is_select = false;
        while (digitalRead(select_button) == LOW);
      }
    }
  }

  // Continuity Mode
  else if (navigator == 3) {
    display.clearDisplay();
    display_text(2, 12, 8, "Continuity");
    display.display();
    while (is_select) {
      calculate_continuity();
      display.clearDisplay();
      display_text(1, 0, 0, "Continuity");
      display_text(2, 12, 8, continuityDetected ? "YES" : "NO");
      display.display();
      if (digitalRead(select_button) == LOW) {
        is_select = false;
        while (digitalRead(select_button) == LOW);
      }
    }
  }
}
