#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// =====================================================
// OLED DISPLAY CONFIGURATION
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =====================================================
// ESP32 PIN DEFINITIONS
// =====================================================

#define TRIG_PIN 18
#define ECHO_PIN 19
#define SERVO_PIN 23

#define GREEN_LED 26
#define RED_LED 27

#define EXIT_BUTTON 32

// =====================================================
// SYSTEM CONFIGURATION
// =====================================================

#define DETECTION_DISTANCE_CM 10.0
#define TOTAL_SPACES 20

#define GATE_CLOSED_ANGLE 0
#define GATE_OPEN_ANGLE 90

int availableSpaces = TOTAL_SPACES;
int occupiedSpaces = 0;

bool gateOpen = false;
bool sensorLocked = false;

bool lastExitButtonState = HIGH;

unsigned long lastExitButtonPress = 0;
const unsigned long DEBOUNCE_TIME = 300;

// =====================================================
// GATE TIMER
// =====================================================

unsigned long gateCloseTime = 0;
const unsigned long GATE_OPEN_DURATION = 3000;

// =====================================================
// FULL PARKING LED BLINK
// =====================================================

unsigned long lastBlinkTime = 0;
bool redLedState = false;

const unsigned long BLINK_INTERVAL = 400;

// =====================================================
// SERVO OBJECT
// =====================================================

Servo gateServo;

// =====================================================
// DISTANCE MEASUREMENT FUNCTION
// =====================================================

float measureDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  return duration * 0.0343 / 2.0;
}

// =====================================================
// OLED MAIN SCREEN
// =====================================================

void showMainScreen(String statusMessage, float distance) {

  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  oled.setTextSize(1);

  oled.setCursor(0, 0);
  oled.println("SMART PARKING");

  oled.setCursor(0, 12);
  oled.print("Available: ");
  oled.print(availableSpaces);
  oled.print("/");
  oled.println(TOTAL_SPACES);

  oled.setCursor(0, 24);
  oled.print("Occupied: ");
  oled.println(occupiedSpaces);

  oled.setCursor(0, 36);
  oled.print("Distance: ");

  if (distance < 0) {
    oled.println("No reading");
  } else {
    oled.print(distance, 1);
    oled.println(" cm");
  }

  oled.setCursor(0, 48);
  oled.print("Status: ");
  oled.println(statusMessage);

  oled.display();
}

// =====================================================
// LARGE OLED MESSAGE
// =====================================================

void showLargeMessage(String message) {

  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  oled.setTextSize(1);

  oled.setCursor(0, 0);
  oled.println("SMART PARKING");

  oled.setTextSize(2);

  oled.setCursor(0, 25);
  oled.println(message);

  oled.display();
}

// =====================================================
// FULL PARKING SCREEN
// =====================================================

void showParkingFullScreen() {

  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  oled.setTextSize(1);

  oled.setCursor(0, 0);
  oled.println("SMART PARKING");

  oled.setTextSize(2);

  oled.setCursor(0, 22);
  oled.println("FULL");

  oled.setTextSize(1);

  oled.setCursor(0, 50);
  oled.println("Entry blocked");

  oled.display();
}

// =====================================================
// GATE CONTROL
// =====================================================

void openGate() {

  gateOpen = true;

  gateServo.write(GATE_OPEN_ANGLE);

  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);

  gateCloseTime = millis() + GATE_OPEN_DURATION;

  Serial.println("Gate opened");
}

void closeGate() {

  gateOpen = false;

  gateServo.write(GATE_CLOSED_ANGLE);

  digitalWrite(GREEN_LED, LOW);

  if (availableSpaces > 0) {
    digitalWrite(RED_LED, HIGH);
  }

  Serial.println("Gate closed");
}

// =====================================================
// RED LED BLINK WHEN FULL
// =====================================================

void blinkFullParkingLed() {

  if (millis() - lastBlinkTime >= BLINK_INTERVAL) {

    lastBlinkTime = millis();

    redLedState = !redLedState;

    digitalWrite(RED_LED, redLedState);
  }
}

// =====================================================
// REGISTER VEHICLE ENTRY
// =====================================================

void registerEntry(float distance) {

  if (availableSpaces > 0) {

    availableSpaces--;
    occupiedSpaces++;

    Serial.println("Vehicle entry registered");

    Serial.print("Available spaces: ");
    Serial.println(availableSpaces);

    Serial.print("Occupied spaces: ");
    Serial.println(occupiedSpaces);

    showLargeMessage("ENTRY");

    delay(700);

    openGate();

    showMainScreen("ENTRY", distance);

  } else {

    closeGate();

    digitalWrite(GREEN_LED, LOW);

    showParkingFullScreen();
  }
}

// =====================================================
// REGISTER VEHICLE EXIT
// =====================================================

void registerExit(float distance) {

  if (occupiedSpaces > 0) {

    occupiedSpaces--;
    availableSpaces++;

    Serial.println("Vehicle exit registered");

    Serial.print("Available spaces: ");
    Serial.println(availableSpaces);

    Serial.print("Occupied spaces: ");
    Serial.println(occupiedSpaces);

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    showLargeMessage("EXIT");

    delay(700);

    showMainScreen("EXIT", distance);

  } else {

    Serial.println("Parking is empty");

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    showLargeMessage("EMPTY");

    delay(800);
  }
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(EXIT_BUTTON, INPUT_PULLUP);

  // Servo setup
  gateServo.setPeriodHertz(50);
  gateServo.attach(SERVO_PIN, 500, 2400);

  // OLED setup
  Wire.begin(21, 22);

  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {

    Serial.println("Failed to initialize OLED display");

    while (true);
  }

  closeGate();

  showLargeMessage("STARTED");

  delay(1200);

  Serial.println("System started");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  float distance = measureDistanceCm();

  bool exitButtonState = digitalRead(EXIT_BUTTON);

  unsigned long currentTime = millis();

  // =====================================================
  // EXIT BUTTON
  // =====================================================

  if (lastExitButtonState == HIGH && exitButtonState == LOW) {

    if (currentTime - lastExitButtonPress > DEBOUNCE_TIME) {

      registerExit(distance);

      lastExitButtonPress = currentTime;
    }
  }

  lastExitButtonState = exitButtonState;

  // =====================================================
  // FULL PARKING CONDITION
  // =====================================================

  if (availableSpaces == 0) {

    closeGate();

    digitalWrite(GREEN_LED, LOW);

    showParkingFullScreen();

    blinkFullParkingLed();

    delay(100);

    return;
  }

  // =====================================================
  // VEHICLE DETECTION
  // =====================================================

  bool vehicleDetected = false;

  if (distance > 0 && distance <= DETECTION_DISTANCE_CM) {

    vehicleDetected = true;
  }

  // Register only one entry per detected vehicle
  if (vehicleDetected && !sensorLocked && !gateOpen) {

    registerEntry(distance);

    sensorLocked = true;
  }

  // Unlock sensor after vehicle leaves
  if (!vehicleDetected && sensorLocked) {

    sensorLocked = false;
  }

  // =====================================================
  // AUTOMATIC GATE CLOSE
  // =====================================================

  if (gateOpen && millis() >= gateCloseTime) {

    closeGate();
  }

  // =====================================================
  // OLED STATUS MESSAGE
  // =====================================================

  String statusMessage;

  if (vehicleDetected) {

    statusMessage = "VEHICLE";

  } else if (gateOpen) {

    statusMessage = "OPEN";

  } else {

    statusMessage = "WAITING";
  }

  showMainScreen(statusMessage, distance);

  delay(200);
}
