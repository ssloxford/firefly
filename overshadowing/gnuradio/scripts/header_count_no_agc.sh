#!/bin/bash

find ~/git/firefly/gnuradio/samples/results_overshadowing_no_agc -type f | xargs -I {} sh -c "echo {}; echo {} >> ~/git/firefly/gnuradio/samples/overshadowing_files_no_agc; cat {} | ~/git/firefly/byte_aligner/header_counter >> ~/git/firefly/gnuradio/samples/overshadowing_no_agc"
