#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: qpsk_overshadowing
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


class qpsk_overshadowing(gr.top_block):

    def __init__(self, in_file_attack='/data/satnogs/AQUA_PADDED.raw', out_file='/data/satnogs/OVERSHADOW.raw', overshadow_strength=5, padding_bytes=32):
        gr.top_block.__init__(self, "qpsk_overshadowing")

        ##################################################
        # Parameters
        ##################################################
        self.in_file_attack = in_file_attack
        self.out_file = out_file
        self.overshadow_strength = overshadow_strength
        self.padding_bytes = padding_bytes

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 32000

        ##################################################
        # Blocks
        ##################################################
        self.qpsk_masked_encoder_0 = qpsk_masked_encoder(
            excess_bw=0.350,
            freq_offset=0,
            noise=1,
            noise_volt=0.0001,
            time_offset=1.000,
        )
        self.qpsk_encoder_0 = qpsk_encoder(
            excess_bw=0.350,
            freq_offset=0,
            noise=1,
            noise_volt=0.0001,
            time_offset=1.000,
        )
        self.qpsk_decoder_0 = qpsk_decoder(
            eq_gain=0.01,
            phase_bw=6.28/100.0,
            timing_loop_bw=6.28/100.0,
        )
        self.blocks_vector_source_x_0 = blocks.vector_source_b([0]*(1024-padding_bytes)+[1]*(1024+padding_bytes+padding_bytes)+[0]*(1024-padding_bytes), True, 1, [])
        self.blocks_throttle_1_0_0 = blocks.throttle(gr.sizeof_char*1, samp_rate,True)
        self.blocks_throttle_1_0 = blocks.throttle(gr.sizeof_char*1, samp_rate,True)
        self.blocks_throttle_1 = blocks.throttle(gr.sizeof_char*1, samp_rate,True)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_cc(overshadow_strength)
        self.blocks_file_source_1_0 = blocks.file_source(gr.sizeof_char*1, 'in_file', True, 0, 0)
        self.blocks_file_source_1_0.set_begin_tag(pmt.PMT_NIL)
        self.blocks_file_source_1 = blocks.file_source(gr.sizeof_char*1, in_file_attack, True, 0, 0)
        self.blocks_file_source_1.set_begin_tag(pmt.PMT_NIL)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_char*1, out_file, False)
        self.blocks_file_sink_0.set_unbuffered(False)
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.analog_agc3_xx_0 = analog.agc3_cc(1e-3, 1e-4, 1.0, 1.0, 1)
        self.analog_agc3_xx_0.set_max_gain(65536)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_agc3_xx_0, 0), (self.qpsk_decoder_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.analog_agc3_xx_0, 0))
        self.connect((self.blocks_file_source_1, 0), (self.blocks_throttle_1_0, 0))
        self.connect((self.blocks_file_source_1_0, 0), (self.blocks_throttle_1_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_throttle_1, 0), (self.qpsk_masked_encoder_0, 1))
        self.connect((self.blocks_throttle_1_0, 0), (self.qpsk_masked_encoder_0, 0))
        self.connect((self.blocks_throttle_1_0_0, 0), (self.qpsk_encoder_0, 0))
        self.connect((self.blocks_vector_source_x_0, 0), (self.blocks_throttle_1, 0))
        self.connect((self.qpsk_decoder_0, 0), (self.blocks_file_sink_0, 0))
        self.connect((self.qpsk_encoder_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.qpsk_masked_encoder_0, 0), (self.blocks_multiply_const_vxx_0, 0))


    def get_in_file_attack(self):
        return self.in_file_attack

    def set_in_file_attack(self, in_file_attack):
        self.in_file_attack = in_file_attack
        self.blocks_file_source_1.open(self.in_file_attack, True)

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

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_1.set_sample_rate(self.samp_rate)
        self.blocks_throttle_1_0.set_sample_rate(self.samp_rate)
        self.blocks_throttle_1_0_0.set_sample_rate(self.samp_rate)




def argument_parser():
    parser = ArgumentParser()
    parser.add_argument(
        "--in-file-attack", dest="in_file_attack", type=str, default='/data/satnogs/AQUA_PADDED.raw',
        help="Set Input file: bytes to overshadow [default=%(default)r]")
    parser.add_argument(
        "--out-file", dest="out_file", type=str, default='/data/satnogs/OVERSHADOW.raw',
        help="Set Output file [default=%(default)r]")
    parser.add_argument(
        "--overshadow-strength", dest="overshadow_strength", type=eng_float, default="5.0",
        help="Set Overshadowing Multiplier [default=%(default)r]")
    parser.add_argument(
        "--padding-bytes", dest="padding_bytes", type=intx, default=32,
        help="Set Padding Bytes [default=%(default)r]")
    return parser


def main(top_block_cls=qpsk_overshadowing, options=None):
    if options is None:
        options = argument_parser().parse_args()
    tb = top_block_cls(in_file_attack=options.in_file_attack, out_file=options.out_file, overshadow_strength=options.overshadow_strength, padding_bytes=options.padding_bytes)

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    tb.start()

    try:
        input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
