#!/usr/bin/env python3

import sys
from qpsk_overshadowing import qpsk_overshadowing

in_file = "/data/samples/ATTACK_PADDED.raw"
out_dir = "/data/samples/results_overshadowing/"
padding_bytes = 64
num_packets = 1024

channel_noise_victim = [0, 1]
channel_noise_attacker = [0, 1]
overshadow_strength = [ x / 10 for x in range(0,101) ]
out_format = out_dir + "OUT_{noise_victim}_{noise_attacker}_{overshadow}.raw"

if __name__ == "__main__":
    start_count = 0
    if len(sys.argv) > 1:
        try:
            start_count = int(sys.argv[1])
        except ValueError:
            start_count = 0

    run_count = 0

    for noise_victim in channel_noise_victim:
        for noise_attacker in channel_noise_attacker:
            for overshadow in overshadow_strength:
                if run_count == start_count:
                    print("Run {}: Parameters {}, {}, {}".format(run_count, noise_victim, noise_attacker, overshadow))

                    out_file = out_format.format(
                        noise_victim = noise_victim,
                        noise_attacker = noise_attacker,
                        overshadow = int(overshadow*10)
                    )

                    qpsk = qpsk_overshadowing(
                        in_file = in_file,
                        out_file = out_file,
                        padding_bytes = padding_bytes,
                        num_packets = num_packets,
                        channel_noise_victim = noise_victim,
                        channel_noise_attacker = noise_attacker,
                        overshadow_strength = overshadow
                    )

                    qpsk.start()
                    try:
                        qpsk.wait()
                    except AssertionError:
                        print(":(")

                run_count += 1
