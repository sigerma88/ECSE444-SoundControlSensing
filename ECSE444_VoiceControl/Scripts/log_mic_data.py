import csv
import serial
import time
import re

# Serial port
ser = serial.Serial('/dev/tty.usbmodem1103', 115200, timeout=1)

# Open CSV file
filename = 'Audio_values_converted.csv'

with open(filename, mode='w') as file:
  writer = csv.writer(file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

  try:
    while True:
      line = ser.readline().decode('utf-8').strip()
      if line:
        writer.writerow([line])
        print(line)
  except KeyboardInterrupt:
    print('Interrupted')
    ser.close()
    file.close()
    exit(0)
