#!/bin/bash

gcc -O -w -std=gnu99 -o pipe pipe.c -lm
./pipe test
