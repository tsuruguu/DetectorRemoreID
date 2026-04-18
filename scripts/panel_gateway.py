import serial
import json
import paho.mqtt.client as mqtt # Biblioteka do komunikacji sieciowej

# --- KONFIGURACJA ---
SERIAL_PORT = '/dev/tty.usbmodem12345' # Wpisz swój port (ls /dev/tty.usbmodem*)
BAUD_RATE = 115200
MQTT_BROKER = "broker.hivemq.com" # Przykładowy broker, panel musi go widzieć
MQTT_TOPIC = "skylink/drones/detected"

def main():
    # 1. Połącz się z płytką przez USB
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    
    # 2. Połącz się z brokerem dla panelu
    client = mqtt.Client()
    client.connect(MQTT_BROKER, 1883, 60)
    
    print(f"Gateway uruchomiony. Przekazywanie danych z {SERIAL_PORT} do panelu...")

    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            
            # Zakładamy, że nRF54 wysyła dane w formacie JSON
            if line.startswith("{"):
                try:
                    # Opcjonalna walidacja JSON
                    drone_data = json.loads(line)
                    
                    # Wysyłka do panelu Skylink
                    client.publish(MQTT_TOPIC, json.dumps(drone_data))
                    print(f"Przesłano: {drone_data['drone_id']} (RSSI: {drone_data['rssi']})")
                except Exception as e:
                    print(f"Błąd parsowania: {e}")

if __name__ == "__main__":
    main()