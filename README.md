# DetectorRemoreID
project for hacknarok

---

## 1. nRF7002-DK: Multiprotocol "Consolidated Sniffer"

This board serves as the primary hardware interface for eavesdropping on drones' digital "license plates" (Remote ID).

- **Wi-Fi Remote ID Detection:** Utilizing the nRF7002 companion IC, the board is capable of capturing Wi-Fi Beacon and Wi-Fi NaN (Neighbor Awareness Networking) frames. This is a critical feature, as most modern commercial drones (e.g., DJI) broadcast their Remote ID data over Wi-Fi.

- **Bluetooth Remote ID Detection:** The main host processor (nRF5340) supports Bluetooth 5.4, specifically including Long Range (Coded PHY) mode. This enables the system to receive Remote ID packets from significantly greater distances than standard Bluetooth—potentially reaching hundreds of meters under optimal conditions.

- **Software Support:** The system integrates the opendroneid-core-c library to decode raw over-the-air data. This allows for the extraction of vital flight parameters, including: the drone's GPS coordinates, altitude, vector velocity, flight direction, and the location of the ground control station (operator).

- **SSID Scanning:** Even if a drone does not have an active Remote ID module, the nRF7002 can detect the Wi-Fi signals emitted by the aircraft's control or video link, providing a baseline detection of nearby unidentified aerial activity.


## 2. nRF54L15-DK: "AI Brain" and Precise Signal Analysis

Despite only using a single board for this role, the nRF54L15 serves as the high-performance computational core of the project.

-- **Edge AI (TinyML):** The board leverages its 128 MHz ARM Cortex-M33 core to run specialized neural networks. Fully supported by the Edge Impulse platform, it allows for the deployment of custom models trained to recognize drone presence based on unique signal patterns.

-- **RSSI Analysis (Signal Strength):** Since the setup lacks an antenna matrix for Direction Finding (AoA), the system relies on AI-driven RSSI regression. The neural network is trained to learn the non-linear relationship between signal strength and physical distance, filtering out environmental noise and multipath interference.

-- **Bluetooth 6.0 Ready:** The nRF54L15 hardware supports the latest Channel Sounding features. While typically requiring two boards, it is prepared to perform centimeter-level distance measurements if paired with a compatible device or a modern smartphone.

-- **RISC-V Coprocessor:** To ensure the main processor isn't bogged down by I/O tasks, the integrated RISC-V coprocessor (FLPR) handles ultra-precise sensor data collection. This is particularly useful for measuring the timing-critical pulses of the HC-SR04 ultrasonic sensor.


## 3. Sensors and Their Role in Drone Detection

The provided hardware components allow for sophisticated sensor fusion, significantly increasing the overall reliability of the detection system.


| Sensor | Capability / Project Role |
| ------- | ------------------------|
| BME280 |"Atmospheric Correction: Air pressure, humidity, and temperature directly affect radio wave propagation. By feeding this environmental data into the neural network, the AI model can more accurately estimate the drone's distance based on its RSSI." |
| TEMT6000 | "Optical Strobe Detection: Drones are required to have anti-collision lighting. The TEMT6000 can detect rapid changes in light intensity (strobe patterns), providing an optical confirmation that complements the radio-based detection." |
| HC-SR04 | Short-Range Verification: This ultrasonic sensor provides high-precision distance data for objects within 4 meters. It is used to verify drone takeoff and landing events near the monitoring station. |
| Joystick & Display | "Operator Interface: The 3-axis joystick allows for manual menu navigation or simulated antenna aiming, while the HC595-driven display shows the real-time distance to the closest detected drone." |

---

## Structure:

```
DetectorRemoteID/
├── common/                         # Wspólne zasoby dla obu płytek (nRF70 i nRF54)
│   ├── include/
│   │   ├── drone_id_types.hpp      # Struktury danych OpenDroneID
│   │   └── inter_board_protocol.h  # Definicja ramek komunikacji UART/SPI między DK
│   └── lib/
│       └── opendroneid/            # Biblioteka opendroneid-core-c (jako moduł/submodule)
├── nrf7002_sniffer/                # Płytka "Uszy": Wi-Fi/BLE Scanner
│   ├── src/
│   │   ├── main.c                  # Koordynacja skanowania
│   │   ├── wifi_scanner.c          # Obsługa ramek Wi-Fi Beacon/NaN
│   │   └── ble_scanner.c           # Obsługa Bluetooth Long Range (Coded PHY) 
│   ├── boards/                     # Specyficzne konfiguracje dla nRF7002-DK
│   ├── prj.conf
│   └── CMakeLists.txt
├── nrf54_brain/                    # Płytka "Mózg": AI, Sensory, UI
│   ├── sysbuild.cmake              # Konfiguracja sysbuild dla wielu obrazów (App + RISC-V)
│   ├── src/                        # Główny procesor (Cortex-M33)
│   │   ├── main.cpp                # Fuzja danych i logika biznesowa
│   │   ├── ai_engine.cpp           # Wrapper dla modelu Edge Impulse 
│   │   ├── bme280_handler.cpp      # Korekcja atmosferyczna dla RSSI
│   │   ├── ui_manager.cpp          # Obsługa Joysticka i wyświetlacza HC595
│   │   └── data_aggregator.cpp     # Odbieranie danych z "Uszu" (nRF7002)
│   ├── src_flpr/                   # Koprocesor RISC-V (FLPR)
│   │   └── hcsr04_offload.c        # Ultra-precyzyjny pomiar czasu dla ultradźwięków
│   ├── ml_model/                   # Wyeksportowana biblioteka C++ z Edge Impulse
│   ├── boards/                     # Overlaye dla nRF54L15-DK (pinctrl dla sensorów)
│   ├── prj.conf
│   └── CMakeLists.txt
├── scripts/                        # Skrypty do zbierania danych (Python/Data Forwarder)
└── west.yml                        # Jeśli chcesz zarządzać zależnościami SDK
```
