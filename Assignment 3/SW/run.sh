#!/bin/bash
# latency=50
# loss=0.1
# Define the range of values for the nested loop
sudo fuser -k 8080/udp

for latency in "50" "100" "150" "200" "250" "500"; do
    for loss in "0.1" "0.5" "1" "1.5" "2" "5"; do

        # Reset
        sudo tc qdisc del dev lo root 

        echo "$loss"" ""$latency" >>sw_time.txt
        # Bandwidth
        sudo tc qdisc add dev lo root netem rate 800kbit
        # Loss
        sudo tc qdisc change dev lo root netem loss "$loss"%
        # Latency
        sudo tc qdisc change dev lo root netem delay "$latency"ms 10ms distribution normal
        # Run Code in 2 terminals
        python3 sender.py loco.jpg &
        pid1=$! 
        python3 receiver.py "out_img/""$loss""_""$latency".jpg &
        pid2=$!

        wait $pid1 $pid2
        sleep 5
        sudo fuser -k 8080/udp
    done
done