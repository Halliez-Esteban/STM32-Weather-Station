import serial
import numpy as np
import matplotlib.pyplot as plt

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

#Plots the three data lines on one canva
line_temp, = ax_temp.plot([], [], marker="o", color="orange", linestyle="-")
line_press, = ax_press.plot([], [], marker="o", color="blue", linestyle="-")
line_hum, = ax_hum.plot([], [], marker="o", color="grey", linestyle="-")

#Axis setup
ax_temp.set_ylabel("Temperature (°C)")
ax_temp.set_title("Temperature across time")

ax_press.set_ylabel("Pression")
ax_press.set_title("Pression across time")

ax_hum.set_xlabel("Temps (s)")
ax_hum.set_ylabel("Humidity (%)")
ax_hum.set_title("Humidity across time")

plt.ion()
plt.show()

try:
    ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
    print(f"Connected to port {PORT}. Waiting data...")
    
    while True:
        if ser.in_waiting > 0:
            # Read data from STM32
            line = ser.readline().decode('utf-8', errors='ignore').strip()

            #Uncomment the next line to debug (same as Putty on COM 3 BaudRate 115200)
            #print(f"From STM32 : {line}")

            #Finding Datas in logs & Putting them in the corresponding array
            if (line.find("Temperature :")!=-1):
                time.append(timeCounter)
                temperature.append(float(line[14:18].strip()))
                print(f"Added : {temperature[-1]}")

            elif (line.find("Pressure :")!=-1):
                pressure.append(float(line[11:18].strip()))
                print(f"Added : {pressure[-1]}")

            elif (line.find("Humidity :")!=-1):
                humidity.append(float(line[11:16].strip()))
                print(f"Added : {humidity[-1]}")
                print(f"Added {time[-1]} components.")
                timeCounter += 1

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
    ser.close()