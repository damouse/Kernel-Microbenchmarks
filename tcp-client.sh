#!/bin/bash

rm ./tcp-client
gcc -O -w -std=gnu99 -o tcp-client tcp-client.c -lm

echo "Starting tcp client"
./tcp-client

