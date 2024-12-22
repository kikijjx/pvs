import serial

ser = serial.Serial('COM8', 9600)

while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        print(line)