#!/bin/bash

# Function to kill Python processes running on specific ports
kill_python_processes() {
    sudo fuser -k 8080/udp
    sudo fuser -k 8081/udp
}

# Function to run the Python scripts with different protocols
run_scripts() {
    local protocol="$1"

    # Loop through latency and loss values
    for latency in "50" "100" "150" "200" "250" "500"; do
        for loss in "0.1" "0.5" "1" "1.5" "2" "5"; do

            # Reset network configuration
            sudo tc qdisc del dev lo root 

            # Write loss and latency values to file
            echo "$loss $latency" >> "$protocol/check_time.txt"

            # Bandwidth limit
            sudo tc qdisc add dev lo root netem rate 20000kbit

            # Loss
            sudo tc qdisc change dev lo root netem loss "$loss"% 

            # Latency
            sudo tc qdisc change dev lo root netem delay "$latency"ms 10ms distribution normal

            # Run server and client scripts
            if [ "$protocol" == "SelectiveRepeat" ]; then
                python2.7 "$protocol/ServerApp.py" &
            else
                python3 "$protocol/ServerApp.py" &
            fi
            pid1=$! 
            if [ "$protocol" == "SelectiveRepeat" ]; then
                python2.7 "$protocol/ClientApp.py" &
            else
                python3 "$protocol/ClientApp.py" &
            fi
            pid2=$!

            # Wait for both processes to finish
            wait $pid1 $pid2

            # Wait for a short duration before resetting
            sleep 5

            # Kill Python processes running on specific ports
            kill_python_processes
        done
    done
}

# Main script

# Kill Python processes running on specific ports at the beginning
kill_python_processes

# Check if only one flag is provided
if [[ $# -ne 1 ]]; then
    echo "Usage: bash run.sh [-sr | -sw | -gb]"
    exit 1
fi

# Loop through command-line arguments
case $1 in
    -sr)
        # Selective Repeat
        run_scripts "SelectiveRepeat"
        ;;
    -sw)
        # Stop and Wait
        run_scripts "StopAndWait"
        ;;
    -gb)
        # Go Back N
        run_scripts "GoBackN"
        ;;
    *)
        echo "Unknown option: $1"
        echo "Usage: bash run.sh [-sr | -sw | -gb]"
        exit 1
        ;;
esac

