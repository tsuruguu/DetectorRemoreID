import serial
import json
import socket
import time
from datetime import datetime

# --- KONFIGURACJA ---
SERIAL_PORT = '/dev/tty.usbmodem12345' # Sprawdź swój port!
BAUD_RATE = 115200
PANEL_IP = '127.0.0.1'
PANEL_PORT = 5005

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Brama Detector -> Panel uruchomiona. Nasłuch na {SERIAL_PORT}...")
    except Exception as e:
        print(f"Błąd portu szeregowego: {e}")
        return

    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            
            if line.startswith("{"):
                try:
                    raw_data = json.loads(line)
                    
                    # 1. Transformacja współrzędnych (nRF wysyła lat * 10^7)
                    lat_float = float(raw_data.get('lat', 0)) / 10000000.0
                    lon_float = float(raw_data.get('lon', 0)) / 10000000.0
                    
                    # 2. Tworzenie formatu pod panel Skylink
                    panel_entry = {
                        "timestamp": datetime.now().isoformat(),
                        "drone_id": raw_data.get('drone_id', 'Unknown'),
                        "position": [lat_float, lon_float], # Kluczowe: tablica [lat, lon]
                        "rssi": raw_data.get('rssi'),
                        "score": 1.0 if raw_data.get('dist', 100) < 50 else 0.5, # Przykład logiki zaufania
                        "ai_dist": raw_data.get('dist') # Dodatkowe info
                    }

                    # 3. Wysłanie listy obiektów (panel_data.json oczekuje listy [ {...} ])
                    payload = json.dumps([panel_entry]) 

                    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                        s.connect((PANEL_IP, PANEL_PORT))
                        s.sendall(payload.encode('utf-8') + b'\n')
                    
                    print(f"Zdetektowano: {panel_entry['drone_id']} na [{lat_float}, {lon_float}]")
                
                except Exception as e:
                    print(f"Błąd przetwarzania: {e}")

if __name__ == "__main__":
    main()