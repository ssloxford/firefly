#!/bin/bash

find ~/git/firefly/gnuradio/samples/results_overshadowing_noise1_1 -type f | xargs -I {} sh -c "echo {}; echo {} >> ~/git/firefly/gnuradio/samples/overshadowing_files_noise1_1; cat {} | ~/git/firefly/byte_aligner/header_counter >> ~/git/firefly/gnuradio/samples/overshadowing_noise1_1"
