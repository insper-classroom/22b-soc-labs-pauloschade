#!/bin/bash

case "$1" in
start)
    printf "starting webserver"
    ./root/webapp/main &
    sleep 5
    python3 ./root/webapp/app.py &
    [ $? = 0 ] && echo "OK" || echo "FAIL"
    ;; 
*)
    exit 1
    ;;
esac