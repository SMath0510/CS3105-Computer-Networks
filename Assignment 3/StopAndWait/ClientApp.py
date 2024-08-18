# sender.py - The sender in the reliable data transfer protocol
import packet
import socket
import sys
import _thread
import time
import udt

import argparse
import os

from timer import Timer

PACKET_SIZE = 25600
RECEIVER_ADDR = ('localhost', 8080)
SENDER_ADDR = ('localhost', 8081)
SLEEP_INTERVAL = 0.02 # in seconds -> interval between checks for timeouts
TIMEOUT_INTERVAL = 0.2 # in seconds
WINDOW_SIZE = 1


class Sender:
    
    def __init__ (self, socket_, filename, debug = False):
        self.socket_ = socket_
        self.filename = filename
        self.debug = debug
        
        # Shared resources across threads
        self.base = 0
        self.mutex = _thread.allocate_lock()
        self.send_timer = Timer(TIMEOUT_INTERVAL)
        
    # Sets the window size
    def set_window_size(self, num_packets):
        base = self.base
        return min(WINDOW_SIZE, num_packets - base)

    # Send thread
    def send(self):
        filename = self.filename
        socket_ = self.socket_

        # Open the file
        try:
            file = open(filename, 'rb')
        except IOError:
            print('Unable to open', filename)
            return
        
        # Add all the packets to the buffer
        packets = []
        seq_num = 0
        while True:
            data = file.read(PACKET_SIZE)
            if not data:
                break
            packets.append(packet.make(seq_num, data))
            seq_num += 1

        num_packets = len(packets)
        if self.debug: print('I gots', num_packets)
        window_size = self.set_window_size(num_packets)
        next_to_send = 0
        self.base = 0

        # Start the receiver thread
        _thread.start_new_thread(self.receive, ())

        while self.base < num_packets:
            self.mutex.acquire()
            # Send all the packets in the window
            while next_to_send < self.base + window_size:
                print('Sending packet', next_to_send)
                udt.send(packets[next_to_send], socket_, RECEIVER_ADDR)
                next_to_send += 1

            # Start the timer
            if not self.send_timer.running():
                # print('Starting timer')
                self.send_timer.start()

            # Wait until a timer goes off or we get an ACK
            while self.send_timer.running() and not self.send_timer.timeout():
                self.mutex.release()
                # print('Sleeping')
                time.sleep(SLEEP_INTERVAL)
                self.mutex.acquire()

            if self.send_timer.timeout():
                # Looks like we timed out
                print('Timeout')
                self.send_timer.stop()
                next_to_send = self.base
            else:
                print('Shifting window')
                window_size = self.set_window_size(num_packets)
            self.mutex.release()

        # Send empty packet as sentinel
        for i in range(10):
            udt.send(packet.make_empty(), socket_, RECEIVER_ADDR)
        # udt.send(packet.make_empty(), sock, RECEIVER_ADDR)
        file.close()
        
    # Receive thread
    def receive(self):
        socket_ = self.socket_

        while True:
            pkt, _ = udt.recv(socket_)
            ack, _ = packet.extract(pkt)

            # If we get an ACK for the first in-flight packet
            print('Got ACK', ack)
            if (ack >= self.base):
                self.mutex.acquire()
                self.base = ack + 1
                print('Base updated', self.base)
                self.send_timer.stop()
                self.mutex.release()

# Main function
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generic Client (Sender) Application',
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
    parser.add_argument("-w", "--window_size", type=int, default=1,
                        help="Window size, default: 1")
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
    
    WINDOW_SIZE = 1 ## Stop And Wait Protocol

    socket_ = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    socket_.bind(SENDER_ADDR)
    
    sender = Sender(socket_, filename)
    sender.send()
    socket_.close()
