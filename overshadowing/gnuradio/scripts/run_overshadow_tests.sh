#!/bin/bash

#for a in {0..99}
#do
#    for b in {0..9}
#    do
#        echo -$a.$b
#        ./qpsk_overshadowing.py --num-packets 1024 --delay 56 --attacker-gain-db -$a.$b --out-file /dev/stdout #overshadow_output/$a-$b
#    done
#done

#seq -w -100 0.1 -0.1 | parallel "echo {} $1 $2"
seq -w -100 0.1 -0.1 | parallel "./qpsk_overshadowing.py --num-packets 1024 --delay 56 --attacker-gain-db {} --in-file /data/samples/ATTACK_PADDED.raw --channel-noise $1 --processing-noise $2 --out-file overshadow_output/out-$1-$2-{}"
