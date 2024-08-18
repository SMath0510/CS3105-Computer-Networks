# receiver.py - The receiver in the reliable data transer protocol
import packet
import socket
import sys
import udt

import time

RECEIVER_ADDR = ('localhost', 8080)

# Receive packets from the sender
def receive(sock, filename):
    # Open the file for writing
    try:
        file = open(filename, 'wb')
    except IOError:
        print('Unable to open', filename)
        return
    
    expected_num = 0

    # Get current time
    start_time = None

    while True:
        # Get the next packet from the sender
        pkt, addr = udt.recv(sock)
        if start_time is None:
            start_time = time.time()
        if not pkt:
            break
        seq_num, data = packet.extract(pkt)
        print('Got packet', seq_num)
        
        # Send back an ACK
        if seq_num == expected_num:
            print('Got expected packet')
            print('Sending ACK', expected_num)
            pkt = packet.make(expected_num)
            udt.send(pkt, sock, addr)
            expected_num += 1
            file.write(data)
        else:
            print('Sending ACK', expected_num - 1)
            pkt = packet.make(expected_num - 1)
            udt.send(pkt, sock, addr)
    
    end_time = time.time()
    print('Time elapsed:', end_time - start_time)
    file.close()
    # Append the time elapsed to the file gbn_time.txt
    with open('gbn_time.txt', 'a') as f:
        f.write(str(end_time - start_time) + '\n')

# Main function
if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Expected filename as command line argument')
        exit()
        
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(RECEIVER_ADDR) 
    filename = sys.argv[1]
    receive(sock, filename)
    sock.close()
