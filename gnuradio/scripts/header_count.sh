#!/bin/bash

find ~/git/firefly/gnuradio/samples/results_overshadowing -type f | xargs -I {} sh -c "echo {}; echo {} >> output_files; cat {} | ~/git/firefly/byte_aligner/header_counter >> output"
