#!/bin/bash

gcc -O -std=gnu99 -w -o udp-server udp-server.c -lm

echo "Starting udp server"
./udp-server

