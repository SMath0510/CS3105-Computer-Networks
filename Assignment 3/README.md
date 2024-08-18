
# Generic Client (Sender) Application

This Python script is a generic client application that can be used to send files using different protocols like Selective Repeat, GoBackN, or StopAndWait. It provides a command-line interface to specify various parameters for the transmission.


### Prerequisites

Make sure you have Python 2.7 and Python 3 installed on your system.

## Usage

### Running the Code

```
python main_client.py [flags]
```

### Flags

- `-f`, `--filename`: Specifies the file to be sent. Default: `loco.jpg`.
- `-a`, `--sender_ip`: Specifies the sender's IP address. Default: `127.0.0.1`.
- `-b`, `--sender_port`: Specifies the sender's port number. Default: `8081`.
- `-x`, `--receiver_ip`: Specifies the receiver's IP address. Default: `127.0.0.1`.
- `-y`, `--receiver_port`: Specifies the receiver's port number. Default: `8080`.
- `-m`, `--sequence_number_bits`: Specifies the total number of bits used in sequence numbers. Default: `4`.
- `-w`, `--window_size`: Specifies the window size. Default: `7`.
- `-s`, `--max_segment_size`: Specifies the maximum segment size. Default: `25600`.
- `-n`, `--total_packets`: Specifies the total packets to be transmitted. Default: `ALL`.
- `-t`, `--timeout`: Specifies the timeout duration. Default: `0.2`.
- `-z`, `--sleep_interval`: Specifies the sleep interval. Default: `0.02`.
- `-p`, `--protocol`: Specifies the protocol to be used (`SelectiveRepeat`, `GoBackN`, `StopAndWait`). Default: `SelectiveRepeat`.
- `-d`, `--directory_location`: Specifies the source folder for transmission. Default: `/<Current Working Directory>/data/sender/`.

### Example

```
python main_client.py -f example.txt -a 192.168.0.1 -b 8081 -x 192.168.0.2 -y 8080 -m 4 -w 7 -s 1500 -n 100 -t 0.2 -z 0.02 -p SelectiveRepeat -d /path/to/data/
```

This command will run the client application with the specified parameters to send the `example.txt` file using the Selective Repeat protocol.



# Generic Server (Receiver) Application

This Python script is a generic client application (sender) that can be used to transmit files using different protocols like Selective Repeat, GoBackN, or StopAndWait. It provides a command-line interface to specify various parameters for the transmission.

## Usage


### Running the Code

```
python main_server.py [flags]
```

### Flags

- `-f`, `--filename`: Specifies the file to be sent. Default: `loco.jpg`.
- `-a`, `--sender_ip`: Specifies the sender's IP address. Default: `127.0.0.1`.
- `-b`, `--sender_port`: Specifies the sender's port number. Default: `8081`.
- `-x`, `--receiver_ip`: Specifies the receiver's IP address. Default: `127.0.0.1`.
- `-y`, `--receiver_port`: Specifies the receiver's port number. Default: `8080`.
- `-m`, `--sequence_number_bits`: Specifies the total number of bits used in sequence numbers. Default: `4`.
- `-w`, `--window_size`: Specifies the window size. Default: `7`.
- `-s`, `--max_segment_size`: Specifies the maximum segment size. Default: `25600`.
- `-n`, `--total_packets`: Specifies the total packets to be transmitted. Default: `ALL`.
- `-t`, `--timeout`: Specifies the timeout duration. Default: `0.2`.
- `-z`, `--sleep_interval`: Specifies the sleep interval. Default: `0.02`.
- `-p`, `--protocol`: Specifies the protocol to be used (`SelectiveRepeat`, `GoBackN`, `StopAndWait`). Default: `SelectiveRepeat`.
- `-d`, `--directory_location`: Specifies the source folder for transmission. Default: `/<Current Working Directory>/data/sender/`.


### Example

```
python main_server.py -f example.txt -a 192.168.0.1 -b 8081 -x 192.168.0.2 -y 8080 -m 4 -w 7 -s 1500 -n 100 -t 0.2 -z 0.02 -p SelectiveRepeat -d /path/to/data/
```

This command will run the server application with the specified parameters to receive the `example.txt` file using the Selective Repeat protocol.

### Notes

- The script automatically selects the appropriate Python version based on the protocol selected. Python 3 is used by default, but if Selective Repeat protocol is chosen, Python 2.7 will be used.
- Ensure that both Python 2.7 and Python 3 are installed on your system for proper execution.


Here's a concise README for the bash script:

---

# Network Simulation Script

This bash script simulates network conditions and tests different protocols for a client-server application. It uses `tc` (traffic control) to manipulate network parameters such as latency and packet loss.

## Usage

### Running the Script

1. Run the script with one of the following options:

   - To simulate network conditions and test the Selective Repeat protocol:
     ```
     bash run.sh -sr
     ```

   - To simulate network conditions and test the Stop and Wait protocol:
     ```
     bash run.sh -sw
     ```

   - To simulate network conditions and test the Go Back N protocol:
     ```
     bash run.sh -gb
     ```

### Notes

- The script runs the selected protocol with predefined latency and loss values for different combinations.
- It generates a file named `check_time.txt` in each protocol directory to record the latency and loss values for each test case.
- After running each combination of latency and loss values, the script resets the network configuration and kills any existing Python processes running on specific ports.


## Example

To simulate network conditions and test the Selective Repeat protocol:

```bash
bash run.sh -sr
```

This command will run the Selective Repeat protocol with predefined latency and loss values for different combinations.
