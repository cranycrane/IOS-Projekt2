#!/bin/bash

ipcs -tm | grep "$(whoami)" | awk '{print $1};' | xargs -L1 ipcrm -m

ipcs -ts | grep "$(whoami)" | awk '{print $1};' | xargs -L1 ipcrm -s

find /dev/shm -user "$(whoami)" -delete

killall proj2