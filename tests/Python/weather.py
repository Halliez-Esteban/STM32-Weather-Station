import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import re

#Connection with the STM32 using UART 
#USB CDC isn't available on NUCLEO 64
PORT = "COM3"
BAUD_RATE = 115200

#Variables declaration
time = [] #Time scale
timeCounter = 0 #Counter of items in arrays
temperature = [] #Temperature array
humidity = [] #Humidity array
pressure = [] #Pressure array

#Plotting graphs using PyPlot
fig, (ax_temp, ax_press, ax_hum) = plt.subplots(3, 1, figsize=(8, 10))
ax_press.ticklabel_format(style='plain', axis='y') #Forcing full tick printing (e.g 1018hPa)
ax_press.yaxis.set_major_formatter(ticker.ScalarFormatter(useOffset=False))

#Plots the three data lines on one canva
line_temp, = ax_temp.plot([], [], marker="o", color="orange", linestyle="-")
line_press, = ax_press.plot([], [], marker="o", color="blue", linestyle="-")
line_hum, = ax_hum.plot([], [], marker="o", color="grey", linestyle="-")

#Axis setup
ax_temp.set_ylabel("Temperature (°C)")
ax_temp.set_title("Temperature across time")

ax_press.set_ylabel("Pression (hPa)")
ax_press.set_title("Pression across time")

ax_hum.set_xlabel("Time (s)")
ax_hum.set_ylabel("Humidity (%)")
ax_hum.set_title("Humidity across time")

plt.ion()
fig.tight_layout()
plt.show()

try:
    ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
    print(f"Connected to port {PORT}. Waiting data...")

    # Temporary variables to stock a whole set of datas 
    curr_temp = None
    curr_press = None
    curr_hum = None
    
    while True:
        if ser.in_waiting > 0:
            # Read data from STM32
            line = ser.readline().decode('utf-8', errors='ignore').strip()

            #Uncomment the next line to debug (same as Putty on COM 3 BaudRate 115200)
            #print(f"From STM32 : {line}")

            #Finding Datas in logs & Putting them in the corresponding array
            temp_match = re.search(r'Temperature\s*:\s*([+-]?\d*\.\d+|\d+)', line)
            press_match = re.search(r'Pressure\s*:\s*([+-]?\d*\.\d+|\d+)', line)
            hum_match = re.search(r'Humidity\s*:\s*([+-]?\d*\.\d+|\d+)', line)
            
            if (temp_match):
                curr_temp = float(temp_match.group(1))
                print(f"Found : {curr_temp}")

            if (press_match):
                curr_press = float(press_match.group(1))
                print(f"Found : {curr_press}")

            if (hum_match):
                curr_hum = float(hum_match.group(1))
                print(f"Found : {curr_hum}")

            if curr_temp is not None and curr_press is not None and curr_hum is not None:
                time.append(timeCounter)
                temperature.append(curr_temp)
                pressure.append(curr_press)
                humidity.append(curr_hum)
                
                print(f"Added t={timeCounter}s -> T={curr_temp}, P={curr_press}, H={curr_hum}")
                timeCounter += 1
                
                # Reset for next cycle
                curr_temp, curr_press, curr_hum = None, None, None

                # Updating canvas
                line_temp.set_data(time, temperature)
                line_press.set_data(time, pressure)
                line_hum.set_data(time, humidity)

                # Rescaling axis
                for ax in (ax_temp, ax_press, ax_hum):
                    ax.relim()
                    ax.autoscale_view()

                #Adding new value to figures without redrawing the whole canva
                fig.canvas.draw()
                fig.canvas.flush_events()

#Ctrl+C on Terminal Interrupts the Programm         
except KeyboardInterrupt:
    print("\nStopped Programm.")
    if ser is not None and ser.is_open:
        ser.close()