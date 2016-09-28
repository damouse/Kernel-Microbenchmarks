#!/bin/bash

gcc -O -std=gnu99  -w -o udp-client udp-client.c -lm

echo "Starting udp client"
./udp-client

