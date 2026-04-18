# DetectorRemoreID
project for hacknarok

## Structure:

```
nrf-remote-id-aiot/
├── common/                 # Wspólny kod dla obu płytek
│   ├── include/
│   │   └── drone_id_types.h # Definicje struktur danych drona (ASTM)
│   └── src/
│       └── odid_parser.c   # Implementacja parsera (na bazie SquidRID/OpenDroneID)
├── nrf54_core/             # GŁÓWNY PROJEKT (nRF54L15)
│   ├── CMakeLists.txt
│   ├── prj.conf            # Konfiguracja: BLE, AI, UART, PWM, I2C
│   ├── app.overlay         # Definicja pinów (BME280, UART, Joystick)
│   ├── src/
│   │   ├── main.c          # Orkiestracja: BLE scan + UART receive
│   │   ├── ai_logic.c      # Interfejs do modelu TinyML
│   │   └── ui_control.c    # Obsługa wyświetlacza i buzzera
│   └── ml_model/           # Wyeksportowana biblioteka z Edge Impulse
│       ├── model-parameters/
│       └── tflite-model/
├── nrf7002_scanner/        # PROJEKT POMOCNICZY (nRF7002)
│   ├── CMakeLists.txt
│   ├── prj.conf            # Konfiguracja: Wi-Fi scan mode, UART
│   ├── app.overlay         # Definicja pinów UART
│   └── src/
│       └── main.c          # Logika skanowania Wi-Fi i wysyłki po UART
├── docs/                   # Dokumentacja i schematy połączeń
├── scripts/                # Skrypty Python do zbierania danych (trening AI)
└── README.md
```
