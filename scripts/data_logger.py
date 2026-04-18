import serial
import csv
import time

# Konfiguracja portu (sprawdź w systemie, np. /dev/tty.usbmodemXXX lub COM_X)
SERIAL_PORT = '/dev/tty.usbmodem1' 
BAUD_RATE = 115200
FILE_NAME = "drone_dataset.csv"

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Połączono z {SERIAL_PORT}. Zbieranie danych... (Ctrl+C aby zakończyć)")
        
        with open(FILE_NAME, mode='a', newline='') as file:
            writer = csv.writer(file)
            # Nagłówki: RSSI, Temp, Hum, Ground_Truth_Dist (wpisujesz ręcznie podczas testu)
            if file.tell() == 0:
                writer.writerow(["timestamp", "rssi", "temp", "hum", "distance_m"])

            while True:
                line = ser.readline().decode('utf-8').strip()
                if line.startswith("DATA:"):
                    # Format z nRF54: "DATA: -65, 22.5, 45.0" 
                    parts = line.replace("DATA: ", "").split(",")
                    if len(parts) == 3:
                        rssi, temp, hum = parts
                        # Tutaj możesz dodać aktualną realną odległość drona dla etykietowania
                        dist_gt = input(f"RSSI: {rssi}dBm. Podaj realną odległość (m): ")
                        writer.writerow([time.time(), rssi, temp, hum, dist_gt])
                        print(f"Zapisano: RSSI={rssi}, Dist={dist_gt}m")
                        
    except KeyboardInterrupt:
        print("\nZakończono zbieranie danych.")
    finally:
        ser.close()

if __name__ == "__main__":
    main()