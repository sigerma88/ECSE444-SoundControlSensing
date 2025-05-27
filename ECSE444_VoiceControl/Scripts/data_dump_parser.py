import csv
import serial
import re
import time
import argparse

# Parse command-line arguments
parser = argparse.ArgumentParser(description='Data dump parser')
parser.add_argument('--port', '-p', type=str, required=True,
                    help='Serial port (e.g., /dev/tty.usbmodem1103 for macOS or COM3 for Windows)')
parser.add_argument('--baudrate', '-br', type=int, required=True,
                    help='Baud rate (e.g., 115200)')
args = parser.parse_args()

# Serial port
ser = serial.Serial(args.port, args.baudrate, timeout=1)

# Current time
current_time = time.strftime('%Y-%m-%d %H:%M:%S')

# Key lines
data_dump_start = 'Dumping QSPI data'
data_dump_end = 'Finished dumping QSPI data'
data_sensor_start = 'Reading data for sensor '
data_sensor_end = 'Finished reading data for sensor '
temp_line = r'.*Temperature = ([\d\.\-]+)'
hum_line = r'.*Humidity = ([\d\.\-]+)'
acc_line = r'.*Accelerometer x = ([\d\.\-]+), y = ([\d\.\-]+), z = ([\d\.\-]+)'
gyro_line = r'.*Gyroscope x = ([\d\.\-]+), y = ([\d\.\-]+), z = ([\d\.\-]+)'


def dump_temp_data():
    # Open CSV file
    filename = 'temp_data_' + current_time + '.csv'
    with open(filename, mode='w') as temp_data_file:
        temp_data_writer = csv.writer(
            temp_data_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

        # Write header
        temp_data_writer.writerow(['Temperature (Celsius)'])

        while True:
            # Read line
            line = ser.readline().decode('utf-8').strip()

            # Match temperature line and write to CSV
            match = re.search(temp_line, line)
            if match:
                temperature = float(match.group(1))
                temp_data_writer.writerow([temperature])
                print(f'Temperature: {temperature}')

            # Check if the sensor data has ended
            elif line.startswith(data_sensor_end):
                print('Temperature data ended')
                break


def dump_hum_data():
    # Open CSV file
    filename = 'hum_data_' + current_time + '.csv'
    with open(filename, mode='w') as hum_data_file:
        hum_data_writer = csv.writer(
            hum_data_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

        # Write header
        hum_data_writer.writerow(['Humidity (%)'])

        while True:
            # Read line
            line = ser.readline().decode('utf-8').strip()

            # Match humidity line and write to CSV
            match = re.search(hum_line, line)
            if match:
                humidity = float(match.group(1))
                hum_data_writer.writerow([humidity])
                print(f'Humidity: {humidity}')

            # Check if the sensor data has ended
            elif line.startswith(data_sensor_end):
                print('Humidity data ended')
                break


def dump_acc_data():
    # Open CSV file
    filename = 'acc_data_' + current_time + '.csv'
    with open(filename, mode='w') as acc_data_file:
        acc_data_writer = csv.writer(
            acc_data_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

        # Write header
        acc_data_writer.writerow(
            ['Acceleration X (m/s^2)', 'Acceleration Y (m/s^2)', 'Acceleration Z (m/s^2)'])

        while True:
            # Read line
            line = ser.readline().decode('utf-8').strip()

            # Match accelerometer line and write to CSV
            match = re.search(acc_line, line)
            if match:
                acc_x = float(match.group(1))
                acc_y = float(match.group(2))
                acc_z = float(match.group(3))
                acc_data_writer.writerow([acc_x, acc_y, acc_z])
                print(f'Acceleration: ({acc_x}, {acc_y}, {acc_z})')

            # Check if the sensor data has ended
            elif line.startswith(data_sensor_end):
                print('Accelerometer data ended')
                break


def dump_gyro_data():
    # Open CSV file
    filename = 'gyro_data_' + current_time + '.csv'
    with open(filename, mode='w') as gyro_data_file:
        gyro_data_writer = csv.writer(
            gyro_data_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

        # Write header
        gyro_data_writer.writerow(
            ['Gyroscope X (deg/s)', 'Gyroscope Y (deg/s)', 'Gyroscope Z (deg/s)'])

        while True:
            # Read line
            line = ser.readline().decode('utf-8').strip()

            # Match gyroscope line and write to CSV
            match = re.search(gyro_line, line)
            if match:
                gyro_x = float(match.group(1))
                gyro_y = float(match.group(2))
                gyro_z = float(match.group(3))
                gyro_data_writer.writerow([gyro_x, gyro_y, gyro_z])
                print(f'Gyroscope: ({gyro_x}, {gyro_y}, {gyro_z})')

            # Check if the sensor data has ended
            elif line.startswith(data_sensor_end):
                print('Gyroscope data ended')
                break


def read_data_dump():

    # Wait for data dump to start
    while True:
        line = ser.readline().decode('utf-8').strip()
        if line.startswith(data_dump_start):
            print('Data dump started')
            break

    while True:

        # Read line
        line = ser.readline().decode('utf-8').strip()

        # Check if data dump has ended
        if line.startswith(data_dump_end):
            print('Data dump ended')
            break

        # Check if sensor data has started
        if line.startswith(data_sensor_start + '0'):
            print('Reading temperature data...')
            dump_temp_data()
        elif line.startswith(data_sensor_start + '1'):
            print('Reading humidity data...')
            dump_hum_data()
        elif line.startswith(data_sensor_start + '2'):
            print('Reading accelerometer data...')
            dump_acc_data()
        elif line.startswith(data_sensor_start + '3'):
            print('Reading gyroscope data...')
            dump_gyro_data()


print('Reading data dump...')
read_data_dump()
ser.close()
exit(0)
