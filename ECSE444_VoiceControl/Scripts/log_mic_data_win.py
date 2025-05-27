import csv
import serial
import time

# Serial port configuration
ser = serial.Serial('COM3', 115200, timeout=1)

# Output CSV file
filename = 'Audio_values_converted.csv'

# Open CSV file for writing
with open(filename, mode='w', newline='') as file:  # 'newline=""' fixes blank lines in Windows
    writer = csv.writer(file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

    try:
        while True:
            try:
                # Read a line from the serial port
                raw_line = ser.readline()
                if raw_line:  # Ensure there's data to process
                    # Decode the line and clean it
                    line = raw_line.decode('utf-8', errors='replace').strip()
                    if line:  # Skip empty or invalid lines
                        # Write the line to the CSV file
                        writer.writerow([line])
                        print(line)
            except UnicodeDecodeError:
                print("Decoding error, skipping line.")
                continue
            except Exception as e:
                # Log any unexpected exceptions
                print(f"Error processing line: {e}")
    except KeyboardInterrupt:
        # Handle user interruption (Ctrl+C)
        print('Interrupted by user. Closing serial port and file.')
    finally:
        # Ensure resources are closed
        ser.close()
        file.close()
