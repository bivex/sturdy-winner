#!/bin/bash
# Optimized libreactor runner with CPU pinning and performance settings

# Stop any existing libreactor processes and clean up port
echo "Stopping existing libreactor processes..."
./stop.sh

# Setup CPU affinity for network interrupts
/usr/local/bin/setup-irq-affinity.sh

# Run libreactor with CPU pinning (one process per CPU) and disable logging
/usr/bin/taskset -c 0 ./libreactor-server --disable-log &
sleep 0.1
/usr/bin/taskset -c 1 ./libreactor-server --disable-log &
sleep 0.1
/usr/bin/taskset -c 2 ./libreactor-server --disable-log &

echo "Libreactor started with 3 processes (one per CPU) - logging disabled"
echo "Test with: curl http://localhost:2342/json"

# Wait for processes
wait
