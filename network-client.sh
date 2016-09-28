#!/bin/bash

echo "Measuring the network performance with different iperf configurations"
echo "This benchmark requires the iperf tool to be installed."

if [ "$#" != 1 ] ; then
  echo "Usage: ./network-client.sh <server-ip>"
  exit 1
fi

for i in 16 64 256 512 1024 2048 4096 ; do
  iperf -l $i -N -c $1
done
