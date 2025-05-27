% Step 1: Read the CSV file
filename = 'Audio_values_converted.csv'; % Replace with your CSV file name
data = readmatrix(filename);

% Step 2: Extract the relevant column (assuming the data is in the first column)
signal = data(:, 2);

% Step 3: Ensure there are at least 2048 datapoints
if length(signal) < 2048
    error('Not enough datapoints in the signal. At least 2048 required.');
end
signal = signal(1:2048); % Use the first 2048 datapoints

% Step 4: Compute the FFT
Fs = 8000; % Sampling frequency in Hz (replace with your actual sampling rate)
N = 2048;  % Number of points
f = (0:N-1) * (Fs / N); % Frequency vector

% Perform FFT
fftSignal = fft(signal);

% Compute magnitude of FFT
magnitude = abs(fftSignal / N); % Normalize

% Only consider the first half of the spectrum (positive frequencies)
magnitude = magnitude(1:N/2+1);
f = f(1:N/2+1);

% Step 5: Plot the FFT result
figure;
plot(f, magnitude);
xlabel('Frequency (Hz)');
ylabel('Magnitude');
title('FFT of Signal');
grid on;