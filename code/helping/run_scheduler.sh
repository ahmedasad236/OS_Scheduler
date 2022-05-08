#! /bin/sh
echo "Enter the algorithm you want"
read algorithm
../helping/kill_running_processes.sh
rm ../process/process_info.txt
gcc ../process/scheduler.c -o SC && ./SC &
gcc ../process/process_generator.c -o PG && yes $algorithm | ./PG &
