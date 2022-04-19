#!/bin/bash

function run_script {
	1>&2 echo $1
	NAME=$1
	OUT=$(cat ${NAME} | ~/firefly/byte_aligner/header_counter)
	echo "${NAME},${OUT}"
}
export -f run_script

SHELL=$(type -p bash) find ~/firefly/gnuradio/samples/results_overshadowing_noise1_2 -type f | parallel run_script > overshadowing_noise1_2
