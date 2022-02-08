#!/usr/bin/env python3

import sys
from qpsk_overshadowing import qpsk_overshadowing

in_file = "/data/samples/ATTACK_PADDED.raw"
out_dir = "/data/samples/results_overshadowing_noise1_2/"
padding_bytes = 64
num_packets = 1024
channel_noise_victim = 1
channel_noise_attacker = 1

noise_voltage = [
    0.0,
    0.0001,
    0.1,
    0.2,
    0.3,
    0.4,
    0.5,
    0.6,
    0.7,
    0.8,
    0.9,
    1.0,
    2.0,
    4.0
]
overshadow_strength = [ x / 10 for x in range(0,201) ]
out_format = out_dir + "OUT_{noise}_{overshadow}.raw"

if __name__ == "__main__":
    start_count = 0
    end_count = 1000000
    if len(sys.argv) > 1:
        try:
            start_count = int(sys.argv[1])
        except ValueError:
            start_count = 0
    if len(sys.argv) > 2:
        try:
            end_count = int(sys.argv[2])
        except ValueError:
            end_count = 1000000

    run_count = 0

    for noise in noise_voltage:
        for overshadow in overshadow_strength:
            if run_count >= start_count and run_count <= end_count:
                print("Run {}: Parameters {}, {}".format(run_count, noise, overshadow))

                out_file = out_format.format(
                    noise = noise,
                    overshadow = int(overshadow*10)
                )

                qpsk = qpsk_overshadowing(
                    in_file = in_file,
                    out_file = out_file,
                    padding_bytes = padding_bytes,
                    num_packets = num_packets,
                    channel_noise_victim = channel_noise_victim,
                    channel_noise_attacker = channel_noise_attacker,
                    noise_voltage_victim = noise,
                    noise_voltage_attacker = noise,
                    overshadow_strength = overshadow
                )

                qpsk.start()
                try:
                    qpsk.wait()
                except AssertionError:
                    print(":(")

            run_count += 1
