#! /bin/sh
for pid in $(ps -e | grep "process" | awk '{print $1}'); do
    echo $pid
    kill -9 $pid
done
