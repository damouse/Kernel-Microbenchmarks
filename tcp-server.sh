#!/bin/bash

rm ./tcp-server
gcc -O -w -std=gnu99 -o tcp-server tcp-server.c -lm

echo "Starting tcp server"
./tcp-server

