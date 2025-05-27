import re
import csv

# File paths
input_file = "Data_6.txt"
output_file = "Audio_values_converted.csv"

# Initialize lists to store buffer values
audioBufferLeft = []
audioBufferRight = []

# Regular expressions for matching buffer values
left_pattern = re.compile(r"audioBufferLeft\[\d+\]\s+int32_t\s+(-?\d+)")
right_pattern = re.compile(r"audioBufferRight\[\d+\]\s+int32_t\s+(-?\d+)")

# Parse the file
with open(input_file, 'r') as file:
    for line in file:
        # Check for left buffer values
        left_match = left_pattern.search(line)
        if left_match:
            audioBufferLeft.append(int(left_match.group(1)))

        # Check for right buffer values
        right_match = right_pattern.search(line)
        if right_match:
            audioBufferRight.append(int(right_match.group(1)))

# Ensure both buffers have the same length
min_length = min(len(audioBufferLeft), len(audioBufferRight))
audioBufferLeft = audioBufferLeft[:min_length]
audioBufferRight = audioBufferRight[:min_length]

# Write the cleaned data to a CSV file
with open(output_file, 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["audioBufferLeft", "audioBufferRight"])  # Header
    writer.writerows(zip(audioBufferLeft, audioBufferRight))  # Data rows

print(f"Data successfully written to {output_file}")
