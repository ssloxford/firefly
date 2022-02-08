#!/bin/bash

for i in {0..403}
do
	until ./qpsk_instance.py $i
	do
		echo .
		sleep 0.1
	done
done
