#!/bin/bash

find ~/git/firefly/gnuradio/samples/results_overshadowing -type f | xargs -I {} sh -c "echo {}; echo {} >> ~/git/firefly/gnuradio/samples/overshadowing_files; cat {} | ~/git/firefly/byte_aligner/header_counter >> ~/git/firefly/gnuradio/samples/overshadowing"
