# DetectorRemoreID
project for hacknarok

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
