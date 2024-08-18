#!/bin/bash
# latency=50
# loss=0.1
# Define the range of values for the nested loop
sudo fuser -k 8080/udp
sudo fuser -k 8081/udp

for latency in "50" "100" "150" "200" "250" "500"; do
    for loss in "0.1" "0.5" "1" "1.5" "2" "5"; do

        # Reset
        sudo tc qdisc del dev lo root 

        echo "$loss"" ""$latency" >>gbn_time.txt
        # Bandwidth
        sudo tc qdisc add dev lo root netem rate 20000kbit
        # Loss
        sudo tc qdisc change dev lo root netem loss "$loss"%
        # Latency
        sudo tc qdisc change dev lo root netem delay "$latency"ms 10ms distribution normal
        # Run Code in 2 terminals
        python2.7 ServerApp.py &
        pid1=$! 
        python2.7 ClientApp.py &
        pid2=$!

        wait $pid1 $pid2
        sleep 5
        sudo fuser -k 8080/udp
        sudo fuser -k 8081/udp
    done
done