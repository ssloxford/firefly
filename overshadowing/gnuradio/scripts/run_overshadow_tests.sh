#!/bin/bash

#for a in {0..99}
#do
#    for b in {0..9}
#    do
#        echo -$a.$b
#        ./qpsk_overshadowing.py --num-packets 1024 --delay 56 --attacker-gain-db -$a.$b --out-file /dev/stdout #overshadow_output/$a-$b
#    done
#done

seq -w -100 0.1 -0.1 | parallel './qpsk_overshadowing.py --num-packets 1024 --delay 56 --attacker-gain-db {} --out-file overshadow_output/out{}'
