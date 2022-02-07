from qpsk_overshadowing import qpsk_overshadowing

in_file = "/data/samples/ATTACK_PADDED.raw"
out_dir = "/data/samples/results_overshadowing/"
padding_bytes = 64
num_packets = 1024

channel_noise_victim = [0, 1]
channel_noise_attacker = [0, 1]
overshadow_strength = [ x / 10 for x in range(0,101) ]
out_format = "OUT_{noise_victim}_{noise_attacker}_{overshadow}.raw"

if __name__ == "__main__":

    for noise_victim in channel_noise_victim:
        for noise_attacker in channel_noise_attacker:
            for overshadow in overshadow_strength:
                out_file = out_format.format(
                    noise_victim = noise_victim,
                    noise_attacker = noise_attacker,
                    overshadow = overshadow
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
                qpsk.wait()
