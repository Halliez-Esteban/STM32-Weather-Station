# STM32 Real-Time Environmental Datalogger V1

[stm32_weather_station_diagram_en.pdf](https://github.com/user-attachments/files/31962754/stm32_weather_station_diagram_en.pdf)


## Overview
This repository contains an embedded, low-power environmental monitoring system based on the **STM32** microcontrollers. It periodically samples meteorological parameters, adds high-precision time tracking, updates a local interface, and redundantly logs data locally and externally via USB.

The project follows strict **IVVQ (Integration, Verification, Validation, and Quality)** practices suited for scientific instrumentation and high-reliability data acquisition.

## Key Features
* **Sensor Acquisition:** High-precision temperature, pressure, and humidity measurement using the **BME280** over $I^2C$.
* **Precise Timestamping:** Integration of an external **RTC module** for accurate event logging.
* **Dual Storage & Transmission:**
  * Local logging to **SD Card** via SPI/SDIO (FATFS file system).
  * Real-time serial data streaming to PC via **USB Virtual COM Port (CDC)**.
* **Local UI:** Real-time metrics visualization on an **OLED Display** ($I^2C$).

## Architecture & Communication Buses

<img width="431" height="311" alt="{2FB96C71-CA8A-4B17-8B23-81BFB7693776}" src="https://github.com/user-attachments/assets/29129c16-f8ac-4525-9bea-94c82c09ce14" />


## Hardware Components
* **MCU:** STM32 Microcontroller (ARM Cortex-M)
* **Sensor:** Bosch BME280 (Temperature, Pressure, Humidity)
* **RTC:** External Real-Time Clock Module (DS3231)
* **Display:** OLED I2C 1.30" (SSD1106)
* **Storage:** MicroSD Card Module
* **Interface:** USB Type-A / Micro-USB (Virtual COM Port)

## Repository Structure
```text
├── docs/            # Hardware schematics, datasheets
├── firmware/        # STM32 C/C++ source code (Drivers, Middleware, FatFS)
└── tests/           # Unit tests and simulation outputs
