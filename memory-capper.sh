#! /usr/bin/bash

limit=95

while true ; do
    time=$(date +%Y-%m-%dT%H:%M:%S%z)
    mem=$(free | grep Mem)
    total=$(echo "$mem" | awk '{print $2'})
    used=$(echo "$mem" | awk '{print $3'})
    current=$(expr $used \* 100 / $total)
    echo "[$time] Memory $current% ($used / $total)"

    if [[ $current -ge $limit ]] ; then
        program=$(ps -eo pid,ppid,cmd,%mem --sort=-%mem | head -n 2 | tail -n +2)
        echo "[$time] Voy a matar $program"
        pid=$(echo "$program" | awk '{print $1}')
        kill $program
    fi

    sleep 10
done
