# Smart Parking System with ESP32

An embedded systems and IoT project developed using ESP32, ultrasonic sensing, servo motor actuation and OLED I2C display communication.

This project simulates an automated smart parking gate system capable of:

* Detecting approaching vehicles using an HC-SR04 ultrasonic sensor
* Controlling a servo-driven parking barrier
* Monitoring parking occupancy in real time
* Displaying parking information on an OLED display
* Handling parking-full conditions with visual LED alerts
* Managing vehicle entry and exit logic

---

# Technologies & Components

## Hardware

* ESP32 DevKit V1
* HC-SR04 Ultrasonic Sensor
* OLED Display (SSD1306 I2C)
* SG90 Servo Motor
* LEDs
* Push Button
* Breadboard & Jumper Wires

## Software

* Arduino IDE
* Embedded C++
* Adafruit SSD1306 Library
* ESP32Servo Library

---

# Features

## Vehicle Detection

The ultrasonic sensor continuously measures distance to detect approaching vehicles.

## Automatic Barrier Control

When a vehicle is detected and parking space is available:

* the servo motor opens the barrier,
* occupancy is updated,
* OLED display is refreshed.

## Parking Occupancy Management

The system tracks:

* available parking spaces,
* occupied spaces,
* parking-full state.

## OLED Interface

The OLED display provides:

* real-time distance measurements,
* parking occupancy,
* gate status,
* system messages.

## Parking Full Alert

When parking capacity reaches zero:

* the gate remains closed,
* the red LED blinks continuously,
* the OLED displays a "FULL" warning.

---

# Pin Mapping

| Component    | ESP32 Pin |
| ------------ | --------- |
| HC-SR04 TRIG | GPIO 18   |
| HC-SR04 ECHO | GPIO 19   |
| Servo Motor  | GPIO 23   |
| Green LED    | GPIO 26   |
| Red LED      | GPIO 27   |
| Exit Button  | GPIO 32   |
| OLED SDA     | GPIO 21   |
| OLED SCL     | GPIO 22   |

---

# System Logic

1. The ultrasonic sensor detects a vehicle.
2. If parking space is available:

   * occupancy is updated,
   * the gate opens,
   * OLED information refreshes.
3. After a timeout:

   * the gate closes automatically.
4. The exit button decreases occupancy count.
5. When parking becomes full:

   * new entries are blocked,
   * warning alerts are activated.

---

# Project Goals

This project was developed to practice:

* Embedded Systems Programming
* Sensor Integration
* Real-Time System Logic
* IoT Fundamentals
* Hardware/Software Integration
* State Management
* Basic Automation Systems

---

# Future Improvements

Potential future upgrades include:

* Wi-Fi remote monitoring
* Web dashboard
* Mobile application
* MQTT communication
* Database logging
* RFID access control
* License plate recognition
* Multiple parking sensors

---

# Author

**Danilo Augusto Salvego dos Santos**

GitHub: [AugustoSalvego](https://github.com/AugustoSalvego)
