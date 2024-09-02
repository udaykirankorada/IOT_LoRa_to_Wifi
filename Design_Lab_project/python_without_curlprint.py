import paramiko
import serial
import time


def send_command(hostname, username, password, command):
    # Create an SSH client
    client = paramiko.SSHClient()
    # Automatically add the server's host key
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

    try:
        # Connect to the server
        client.connect(hostname, username=username, password=password)
        # Send the command
        stdin, stdout, stderr = client.exec_command(command)
        # Read and return the output
        output = stdout.read().decode("utf-8")
        return output
    finally:
        # Close the connection
        client.close()


# Serial port configuration
serial_port = 'COM13'  # Change this to your serial port
baud_rate = 9600

# SSH configuration
hostname = '10.130.1.1'
username = 'root'
password = 'dragino'

# Open serial connection
ser = serial.Serial(serial_port, baud_rate, timeout=1)

try:
    while True:
        # Read data from serial port
        try:
            data = ser.readline().strip().decode("utf-8")
        except UnicodeDecodeError:
            # Print raw bytes if decoding fails
            data = ser.readline().strip()
            print("Received raw bytes:", data)
        else:
            if not data.startswith("curl"):
                print(data)
            else:
                # Add a short delay before sending the command
                time.sleep(0.1)
                # Run command received from serial
                output = send_command(hostname, username, password, data)
        # Add a short delay before reading again
        time.sleep(0.1)
except KeyboardInterrupt:
    # Close the serial connection on Ctrl+C
    ser.close()
    print("Serial connection closed.")
