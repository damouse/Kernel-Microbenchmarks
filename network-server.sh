#!/bin/bash

echo "Measuring the network performance with different iperf configurations"
echo "This benchmark requires the iperf tool to be installed."

if [ "$#" != 0 ] ; then
  echo "Usage: ./network-server.sh"
  exit 1
fi

iperf -s
