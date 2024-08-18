# receiver.py - The receiver in the reliable data transer protocol
import packet
import socket
import sys
import udt

import argparse
import os

import time

RECEIVER_ADDR = ('localhost', 8080)

# Receive packets from the sender
class Receiver:
    
    def __init__ (self, filename, socket_, debug = False):
        self.filename = filename
        self.socket_ = socket_
        self.debug = debug
        
        
    
    def receive(self, debug = False):
        socket_ = self.socket_
        filename = self.filename
        # Open the file for writing
        try:
            file = open(filename, 'wb')
        except IOError:
            print('Unable to open', filename)
            return
        
        expected_num = 0

        # Get current time
        start_time = None
        end_time = None

        while True:
            # Get the next packet from the sender
            pkt, addr = udt.recv(socket_)
            if start_time is None:
                start_time = time.time()
            end_time = time.time()
            if not pkt:
                break
            seq_num, data = packet.extract(pkt)
            if self.debug: print('Got packet', seq_num)
            
            # Send back an ACK
            if seq_num == expected_num:
                if self.debug: print('Got expected packet')
                print('Sending ACK', expected_num)
                pkt = packet.make(expected_num)
                udt.send(pkt, socket_, addr)
                expected_num += 1
                file.write(data)
            else:
                print('Sending ACK', expected_num - 1)
                pkt = packet.make(expected_num - 1)
                udt.send(pkt, socket_, addr)
        
        print('Time elapsed:', end_time - start_time)
        file.close()
        # Append the time elapsed to the file gbn_time.txt
        with open('sw_time.txt', 'a') as f:
            f.write(str(end_time - start_time) + '\n')

# Main function
if __name__ == '__main__':
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
    parser.add_argument("-d", "--directory_location", type=str, default=os.path.join(os.getcwd(), "data", "sender"),
                        help="Source folder for transmission, default: /<Current Working Directory>/data/sender/")

    args = vars(parser.parse_args())
    
    PACKET_SIZE = args['max_segment_size']
    RECEIVER_ADDR = (args['receiver_ip'], args['receiver_port'])
    SENDER_ADDR = (args['sender_ip'], args['sender_port'])
    SLEEP_INTERVAL = args['sleep_interval'] # in seconds -> interval between checks for timeouts
    TIMEOUT_INTERVAL = args['timeout'] # in seconds
    WINDOW_SIZE = args['window_size']
    filename = f"{args['www']}/{args['filename']}"
        
    socket_ = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    socket_.bind(RECEIVER_ADDR) 
    receiver = Receiver(filename, socket_, False)
    receiver.receive()
    socket_.close()
