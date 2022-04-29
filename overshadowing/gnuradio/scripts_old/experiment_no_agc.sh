#!/bin/bash

for i in {35..403}
do
	until ./qpsk_instance_no_agc.py $i
	do
		echo .
		sleep 0.1
	done
done
