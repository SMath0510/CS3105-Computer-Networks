import os
import argparse

if __name__ == "__main__":
    
    ## Server app
    parser = argparse.ArgumentParser(description='Generic Server (Receiver) Application',
                                     prog='python \
                                           ClientApp.py \
                                           -f <filename> \
                                           -a <sender_ip> \
                                           -b <sender_port> \
                                           -x <receiver_ip> \
                                           -y <receiver_port> \
                                           -m <sequence_number_bits> \
                                           -w <window_size> \
                                           -s <max_segment_size> \
                                           -n <total_packets> \
                                           -t <timeout> \
                                           -z <sleep_time> \
                                           -p <protocol> \
                                           -d <directory_location>')
    
    parser.add_argument("-f", "--filename", type=str, default="loco.jpg",
                        help="File to be sent, default: index.html")
    parser.add_argument("-a", "--sender_ip", type=str, default="127.0.0.1",
                        help="Sender IP, default: 127.0.0.1")
    parser.add_argument("-b", "--sender_port", type=int, default=8081,
                        help="Sender Port, default: 8081")
    parser.add_argument("-x", "--receiver_ip", type=str, default="127.0.0.1",
                        help="Receiver IP, default: 127.0.0.1")
    parser.add_argument("-y", "--receiver_port", type=int, default=8080,
                        help="Receiver Port, default: 8080")
    parser.add_argument("-m", "--sequence_number_bits", type=int, default=4,
                        help="Total number of bits used in sequence numbers, default: 4")
    parser.add_argument("-w", "--window_size", type=int, default=7,
                        help="Window size, default: 7")
    parser.add_argument("-s", "--max_segment_size", type=int, default=25600,
                        help="Maximum segment size, default: 1500")
    parser.add_argument("-n", "--total_packets", type=str, default="ALL",
                        help="Total packets to be transmitted, default: ALL")
    parser.add_argument("-t", "--timeout", type=float, default=0.2,
                        help="Timeout, default: 10")
    parser.add_argument("-z", "--sleep_interval", type=float, default=0.02,
                        help="Sleep Interval, default: 0.02")
    parser.add_argument("-p", "--protocol", type=str, default="SelectiveRepeat",
                        help="Protocol, default: SelectiveRepeat. OPTIONS: GoBackN, SelectiveRepeat, StopAndWait")
    parser.add_argument("-d", "--directory_location", type=str, default=os.path.join(os.getcwd(), "data", "sender"),
                        help="Source folder for transmission, default: /<Current Working Directory>/data/sender/")
    
    args = vars(parser.parse_args())
    
    protocol = args.protocol
    sleep_interval = args.sleep_interval
    sender_ip = args.sender_ip
    sender_port = args.sender_port
    receiver_ip = args.receiver_ip
    receiver_port = args.receiver_port
    sequence_number_bits = args.sequence_number_bits
    window_size = args.window_size
    max_segment_size = args.max_segment_size
    total_packets = args.total_packets
    timeout = args.timeout
    directory_location = args.directory_location
    filename = args.filename
    sleep_interval = args.sleep_interval
    
    python_version = "python3"
    python_code_location = f"{protocol}/ServerApp.py"
    
    if protocol == "SelectiveRepeat":
        python_version = "python2.7"

    command = f"{python_version} {python_code_location} -a {sender_ip} -b {sender_port} -x {receiver_ip} -y {receiver_port} -m {sequence_number_bits} -w {window_size} -s {max_segment_size} -n {total_packets} -t {timeout} -d {directory_location} -f {filename}"
    
    # Run the command using os
    os.system(command)