#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: qpsk_overshadowing_no_agc
# Author: root
# GNU Radio version: 3.8.2.0

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from gnuradio import analog
from gnuradio import blocks
import pmt
from gnuradio import gr
from gnuradio.filter import firdes
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from qpsk_decoder import qpsk_decoder  # grc-generated hier_block
from qpsk_encoder import qpsk_encoder  # grc-generated hier_block
from qpsk_masked_encoder import qpsk_masked_encoder  # grc-generated hier_block


class qpsk_overshadowing_no_agc(gr.top_block):

    def __init__(self, channel_noise_attacker=1, channel_noise_victim=1, frequency_offset_attacker=0, frequency_offset_victim=0, in_file='/data/satnogs/AQUA_PADDED.raw', noise_voltage_attacker=0.0001, noise_voltage_victim=0.0001, num_packets=1024, out_file='/data/satnogs/OVERSHADOW.raw', overshadow_strength=5, padding_bytes=32, timing_offset_attacker=1, timing_offset_victim=1):
        gr.top_block.__init__(self, "qpsk_overshadowing_no_agc")

        ##################################################
        # Parameters
        ##################################################
        self.channel_noise_attacker = channel_noise_attacker
        self.channel_noise_victim = channel_noise_victim
        self.frequency_offset_attacker = frequency_offset_attacker
        self.frequency_offset_victim = frequency_offset_victim
        self.in_file = in_file
        self.noise_voltage_attacker = noise_voltage_attacker
        self.noise_voltage_victim = noise_voltage_victim
        self.num_packets = num_packets
        self.out_file = out_file
        self.overshadow_strength = overshadow_strength
        self.padding_bytes = padding_bytes
        self.timing_offset_attacker = timing_offset_attacker
        self.timing_offset_victim = timing_offset_victim

        ##################################################
        # Blocks
        ##################################################
        self.qpsk_masked_encoder_0 = qpsk_masked_encoder(
            excess_bw=0.350,
            freq_offset=frequency_offset_attacker,
            noise=channel_noise_attacker,
            noise_volt=noise_voltage_attacker,
            time_offset=timing_offset_attacker,
        )
        self.qpsk_encoder_0 = qpsk_encoder(
            excess_bw=0.350,
            freq_offset=frequency_offset_victim,
            noise=channel_noise_victim,
            noise_volt=noise_voltage_victim,
            time_offset=timing_offset_victim,
        )
        self.qpsk_decoder_0 = qpsk_decoder(
            eq_gain=0.01,
            phase_bw=6.28/100.0,
            timing_loop_bw=6.28/100.0,
        )
        self.blocks_vector_source_x_0 = blocks.vector_source_b([0]*(1024-padding_bytes)+[1]*(1024+padding_bytes+padding_bytes)+[0]*(1024-padding_bytes), True, 1, [])
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_cc(overshadow_strength)
        self.blocks_head_0_0_0_0 = blocks.head(gr.sizeof_char*1, 1024*3*num_packets)
        self.blocks_head_0_0_0 = blocks.head(gr.sizeof_char*1, 1024*3*num_packets)
        self.blocks_head_0_0 = blocks.head(gr.sizeof_char*1, 1024*3*num_packets)
        self.blocks_file_source_1 = blocks.file_source(gr.sizeof_char*1, in_file, True, 0, 0)
        self.blocks_file_source_1.set_begin_tag(pmt.PMT_NIL)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_char*1, out_file, False)
        self.blocks_file_sink_0.set_unbuffered(False)
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.analog_random_uniform_source_x_0 = analog.random_uniform_source_b(0, 255, 0)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_random_uniform_source_x_0, 0), (self.blocks_head_0_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.qpsk_decoder_0, 0))
        self.connect((self.blocks_file_source_1, 0), (self.blocks_head_0_0_0, 0))
        self.connect((self.blocks_head_0_0, 0), (self.qpsk_encoder_0, 0))
        self.connect((self.blocks_head_0_0_0, 0), (self.qpsk_masked_encoder_0, 0))
        self.connect((self.blocks_head_0_0_0_0, 0), (self.qpsk_masked_encoder_0, 1))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_vector_source_x_0, 0), (self.blocks_head_0_0_0_0, 0))
        self.connect((self.qpsk_decoder_0, 0), (self.blocks_file_sink_0, 0))
        self.connect((self.qpsk_encoder_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.qpsk_masked_encoder_0, 0), (self.blocks_multiply_const_vxx_0, 0))


    def get_channel_noise_attacker(self):
        return self.channel_noise_attacker

    def set_channel_noise_attacker(self, channel_noise_attacker):
        self.channel_noise_attacker = channel_noise_attacker
        self.qpsk_masked_encoder_0.set_noise(self.channel_noise_attacker)

    def get_channel_noise_victim(self):
        return self.channel_noise_victim

    def set_channel_noise_victim(self, channel_noise_victim):
        self.channel_noise_victim = channel_noise_victim
        self.qpsk_encoder_0.set_noise(self.channel_noise_victim)

    def get_frequency_offset_attacker(self):
        return self.frequency_offset_attacker

    def set_frequency_offset_attacker(self, frequency_offset_attacker):
        self.frequency_offset_attacker = frequency_offset_attacker
        self.qpsk_masked_encoder_0.set_freq_offset(self.frequency_offset_attacker)

    def get_frequency_offset_victim(self):
        return self.frequency_offset_victim

    def set_frequency_offset_victim(self, frequency_offset_victim):
        self.frequency_offset_victim = frequency_offset_victim
        self.qpsk_encoder_0.set_freq_offset(self.frequency_offset_victim)

    def get_in_file(self):
        return self.in_file

    def set_in_file(self, in_file):
        self.in_file = in_file
        self.blocks_file_source_1.open(self.in_file, True)

    def get_noise_voltage_attacker(self):
        return self.noise_voltage_attacker

    def set_noise_voltage_attacker(self, noise_voltage_attacker):
        self.noise_voltage_attacker = noise_voltage_attacker
        self.qpsk_masked_encoder_0.set_noise_volt(self.noise_voltage_attacker)

    def get_noise_voltage_victim(self):
        return self.noise_voltage_victim

    def set_noise_voltage_victim(self, noise_voltage_victim):
        self.noise_voltage_victim = noise_voltage_victim
        self.qpsk_encoder_0.set_noise_volt(self.noise_voltage_victim)

    def get_num_packets(self):
        return self.num_packets

    def set_num_packets(self, num_packets):
        self.num_packets = num_packets
        self.blocks_head_0_0.set_length(1024*3*self.num_packets)
        self.blocks_head_0_0_0.set_length(1024*3*self.num_packets)
        self.blocks_head_0_0_0_0.set_length(1024*3*self.num_packets)

    def get_out_file(self):
        return self.out_file

    def set_out_file(self, out_file):
        self.out_file = out_file
        self.blocks_file_sink_0.open(self.out_file)

    def get_overshadow_strength(self):
        return self.overshadow_strength

    def set_overshadow_strength(self, overshadow_strength):
        self.overshadow_strength = overshadow_strength
        self.blocks_multiply_const_vxx_0.set_k(self.overshadow_strength)

    def get_padding_bytes(self):
        return self.padding_bytes

    def set_padding_bytes(self, padding_bytes):
        self.padding_bytes = padding_bytes
        self.blocks_vector_source_x_0.set_data([0]*(1024-self.padding_bytes)+[1]*(1024+self.padding_bytes+self.padding_bytes)+[0]*(1024-self.padding_bytes), [])

    def get_timing_offset_attacker(self):
        return self.timing_offset_attacker

    def set_timing_offset_attacker(self, timing_offset_attacker):
        self.timing_offset_attacker = timing_offset_attacker
        self.qpsk_masked_encoder_0.set_time_offset(self.timing_offset_attacker)

    def get_timing_offset_victim(self):
        return self.timing_offset_victim

    def set_timing_offset_victim(self, timing_offset_victim):
        self.timing_offset_victim = timing_offset_victim
        self.qpsk_encoder_0.set_time_offset(self.timing_offset_victim)




def argument_parser():
    parser = ArgumentParser()
    parser.add_argument(
        "--channel-noise-attacker", dest="channel_noise_attacker", type=intx, default=1,
        help="Set Attacker: Channel Noise [default=%(default)r]")
    parser.add_argument(
        "--channel-noise-victim", dest="channel_noise_victim", type=intx, default=1,
        help="Set Victim: Channel Noise [default=%(default)r]")
    parser.add_argument(
        "--frequency-offset-attacker", dest="frequency_offset_attacker", type=eng_float, default="0.0",
        help="Set Attacker: Frequency Offset [default=%(default)r]")
    parser.add_argument(
        "--frequency-offset-victim", dest="frequency_offset_victim", type=eng_float, default="0.0",
        help="Set Victim: Frequency Offset [default=%(default)r]")
    parser.add_argument(
        "--in-file", dest="in_file", type=str, default='/data/satnogs/AQUA_PADDED.raw',
        help="Set Input file: bytes to overshadow [default=%(default)r]")
    parser.add_argument(
        "--noise-voltage-attacker", dest="noise_voltage_attacker", type=eng_float, default="100.0u",
        help="Set Attacker: Noise Voltage [default=%(default)r]")
    parser.add_argument(
        "--noise-voltage-victim", dest="noise_voltage_victim", type=eng_float, default="100.0u",
        help="Set Victim: Noise Voltage [default=%(default)r]")
    parser.add_argument(
        "--num-packets", dest="num_packets", type=intx, default=1024,
        help="Set Number of Packets [default=%(default)r]")
    parser.add_argument(
        "--out-file", dest="out_file", type=str, default='/data/satnogs/OVERSHADOW.raw',
        help="Set Output file [default=%(default)r]")
    parser.add_argument(
        "--overshadow-strength", dest="overshadow_strength", type=eng_float, default="5.0",
        help="Set Overshadowing Multiplier [default=%(default)r]")
    parser.add_argument(
        "--padding-bytes", dest="padding_bytes", type=intx, default=32,
        help="Set Padding Bytes [default=%(default)r]")
    parser.add_argument(
        "--timing-offset-attacker", dest="timing_offset_attacker", type=eng_float, default="1.0",
        help="Set Attacker: Timing Offset [default=%(default)r]")
    parser.add_argument(
        "--timing-offset-victim", dest="timing_offset_victim", type=eng_float, default="1.0",
        help="Set Victim: Timing Offset [default=%(default)r]")
    return parser


def main(top_block_cls=qpsk_overshadowing_no_agc, options=None):
    if options is None:
        options = argument_parser().parse_args()
    tb = top_block_cls(channel_noise_attacker=options.channel_noise_attacker, channel_noise_victim=options.channel_noise_victim, frequency_offset_attacker=options.frequency_offset_attacker, frequency_offset_victim=options.frequency_offset_victim, in_file=options.in_file, noise_voltage_attacker=options.noise_voltage_attacker, noise_voltage_victim=options.noise_voltage_victim, num_packets=options.num_packets, out_file=options.out_file, overshadow_strength=options.overshadow_strength, padding_bytes=options.padding_bytes, timing_offset_attacker=options.timing_offset_attacker, timing_offset_victim=options.timing_offset_victim)

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    tb.start()

    tb.wait()


if __name__ == '__main__':
    main()
