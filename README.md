# 🚀 ESP32 ESP-IDF Projects

This repository contains my embedded firmware development projects built using the official **ESP-IDF framework** for ESP32.

It serves as a structured portfolio covering communication protocols, peripheral drivers, RTOS-based systems, and networking applications.

---

## 🛠 Development Environment

- **Operating System:** Ubuntu Linux  
- **ESP-IDF Version:** v6.1-dev-2441-gffb63db38b  
- **Programming Language:** C  
- **Build System:** CMake (via idf.py)  
- **Version Control:** Git (SSH workflow)  
- **Target MCU:** ESP32  

Check ESP-IDF version:

```bash
idf.py --version
```

---

## 📌 Framework & Technologies

- ESP-IDF (Espressif IoT Development Framework)
- FreeRTOS
- C Programming Language
- CMake Build System
- Git (SSH workflow)

---

## 📂 Repository Structure

```
ESP32-ESP-IDF-Projects
│
├── Protocol
│     └── ESP_NOW
│           ├── Sender
│           ├── Receiver
│           └── README.md   (Detailed project explanation)
│
└── (Future modules will be added here)
```

---

## 🚀 Build & Flash Instructions

From the project root directory:

### 🔹 Build Project

```bash
idf.py build
```

### 🔹 Flash Firmware

Check available serial ports:

```bash
ls /dev/ttyUSB*
```

Flash firmware:

```bash
idf.py -p /dev/ttyUSB0 flash
```

(Replace `/dev/ttyUSB0` with your detected port.)

### 🔹 Monitor Serial Output

```bash
idf.py monitor
```

Exit monitor:

```
Ctrl + ]
```

### 🔹 Clean Build

```bash
idf.py fullclean
```

---

## 📚 Project Categories

This repository will include implementations related to:

- Communication Protocols (ESP-NOW, WiFi, BLE)
- Peripheral Drivers (SPI, UART, I2C, ADC)
- FreeRTOS Applications
- Sensor Interfaces
- Robotics Firmware Modules
- IoT Networking Applications
- Industrial Communication (Modbus RTU / TCP - Planned)

---

## 🎯 Objective

To build scalable, production-style firmware using ESP-IDF with:

- Modular structure
- Clean architecture
- Professional Git workflow
- Expandable project organization
- Industry-ready embedded development practices

---

## 👨‍💻 Author

Shankar S  
Embedded Systems & ESP32 Firmware Development  

GitHub: https://github.com/Sh007s

