import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

ser = serial.Serial('/dev/tty.usbmodem1', 115200)
lats, lons =,

fig, ax = plt.subplots()
line, = ax.plot(,, 'ro-', label='Trajektoria Drona')
ax.set_title("Lokalizacja Drona (Remote ID + AI)")
ax.set_xlabel("Długość geograficzna")
ax.set_ylabel("Szerokość geograficzna")

def update(frame):
    if ser.in_waiting > 0:
        data = ser.readline().decode('utf-8').strip()
        if data.startswith("GPS:"):
            # Format: "GPS: 52.2297, 21.0122"
            _, lat, lon = data.replace(",", "").split()
            lats.append(float(lat))
            lons.append(float(lon))
            
            line.set_data(lons, lats)
            ax.relim()
            ax.autoscale_view()
    return line,

ani = FuncAnimation(fig, update, interval=100)
plt.legend()
plt.show()