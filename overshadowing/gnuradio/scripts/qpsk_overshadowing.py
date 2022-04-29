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
from gnuradio import channels
from gnuradio.filter import firdes
from gnuradio import digital
from gnuradio import gr
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from qpsk_decoder import qpsk_decoder  # grc-generated hier_block
import satellites.hier


class qpsk_overshadowing(gr.top_block):

    def __init__(self, attacker_gain_db=0, channel_noise=0.000001, delay=56, in_file='/data/satnogs/AQUA_PADDED.raw', num_packets=1024, out_file='/data/satnogs/OVERSHADOW.raw', padding_bytes=32, processing_noise=0.2):
        gr.top_block.__init__(self, "qpsk_overshadowing")

        ##################################################
        # Parameters
        ##################################################
        self.attacker_gain_db = attacker_gain_db
        self.channel_noise = channel_noise
        self.delay = delay
        self.in_file = in_file
        self.num_packets = num_packets
        self.out_file = out_file
        self.padding_bytes = padding_bytes
        self.processing_noise = processing_noise

        ##################################################
        # Variables
        ##################################################
        self.qpsk = qpsk = digital.constellation_rect([0.707+0.707j, -0.707+0.707j, -0.707-0.707j, 0.707-0.707j], [0, 1, 2, 3],
        4, 2, 2, 1, 1).base()
        self.attacker_gain_multiplier = attacker_gain_multiplier = pow(10, attacker_gain_db/20)

        ##################################################
        # Blocks
        ##################################################
        self.satellites_rms_agc_0_0 = satellites.hier.rms_agc(alpha=1e-2, reference=1.0)
        self.satellites_rms_agc_0 = satellites.hier.rms_agc(alpha=1e-2, reference=1.0)
        self.qpsk_decoder_0 = qpsk_decoder(
            eq_gain=0.01,
            phase_bw=6.28/100.0,
            timing_loop_bw=6.28/100.0,
        )
        self.digital_constellation_modulator_0_0 = digital.generic_mod(
            constellation=qpsk,
            differential=True,
            samples_per_symbol=4,
            pre_diff_code=True,
            excess_bw=0.35,
            verbose=False,
            log=False)
        self.digital_constellation_modulator_0 = digital.generic_mod(
            constellation=qpsk,
            differential=True,
            samples_per_symbol=4,
            pre_diff_code=True,
            excess_bw=0.35,
            verbose=False,
            log=False)
        self.channels_channel_model_3 = channels.channel_model(
            noise_voltage=processing_noise,
            frequency_offset=0.0,
            epsilon=1.0,
            taps=[1.0 + 1.0j],
            noise_seed=0,
            block_tags=False)
        self.channels_channel_model_2 = channels.channel_model(
            noise_voltage=channel_noise,
            frequency_offset=0.0,
            epsilon=1.0,
            taps=[1.0 + 1.0j],
            noise_seed=0,
            block_tags=False)
        self.channels_channel_model_1 = channels.channel_model(
            noise_voltage=channel_noise,
            frequency_offset=0.0,
            epsilon=1.0,
            taps=[1.0 + 1.0j],
            noise_seed=0,
            block_tags=False)
        self.blocks_xor_xx_0 = blocks.xor_bb()
        self.blocks_unpack_k_bits_bb_0_0 = blocks.unpack_k_bits_bb(8)
        self.blocks_unpack_k_bits_bb_0 = blocks.unpack_k_bits_bb(8)
        self.blocks_not_xx_0 = blocks.not_bb()
        self.blocks_multiply_const_vxx_2 = blocks.multiply_const_cc(0.316)
        self.blocks_multiply_const_vxx_1 = blocks.multiply_const_cc(160)
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_cc(10e-7)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_cc(attacker_gain_multiplier)
        self.blocks_head_0_0_0 = blocks.head(gr.sizeof_char*1, 1024*3*num_packets)
        self.blocks_head_0_0 = blocks.head(gr.sizeof_char*1, 1024*3*num_packets)
        self.blocks_file_source_1 = blocks.file_source(gr.sizeof_char*1, in_file, True, 0, 0)
        self.blocks_file_source_1.set_begin_tag(pmt.PMT_NIL)
        self.blocks_file_sink_0_0 = blocks.file_sink(gr.sizeof_char*1, out_file, False)
        self.blocks_file_sink_0_0.set_unbuffered(False)
        self.blocks_delay_0_0 = blocks.delay(gr.sizeof_char*1, int(delay))
        self.blocks_and_const_xx_0 = blocks.and_const_bb(1)
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.analog_random_uniform_source_x_0 = analog.random_uniform_source_b(0, 255, 0)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_random_uniform_source_x_0, 0), (self.blocks_head_0_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.satellites_rms_agc_0_0, 0))
        self.connect((self.blocks_and_const_xx_0, 0), (self.blocks_file_sink_0_0, 0))
        self.connect((self.blocks_delay_0_0, 0), (self.blocks_xor_xx_0, 0))
        self.connect((self.blocks_file_source_1, 0), (self.blocks_head_0_0_0, 0))
        self.connect((self.blocks_file_source_1, 0), (self.blocks_unpack_k_bits_bb_0, 0))
        self.connect((self.blocks_head_0_0, 0), (self.digital_constellation_modulator_0, 0))
        self.connect((self.blocks_head_0_0_0, 0), (self.digital_constellation_modulator_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.channels_channel_model_2, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.channels_channel_model_1, 0))
        self.connect((self.blocks_multiply_const_vxx_1, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_2, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_not_xx_0, 0), (self.blocks_and_const_xx_0, 0))
        self.connect((self.blocks_unpack_k_bits_bb_0, 0), (self.blocks_delay_0_0, 0))
        self.connect((self.blocks_unpack_k_bits_bb_0_0, 0), (self.blocks_xor_xx_0, 1))
        self.connect((self.blocks_xor_xx_0, 0), (self.blocks_not_xx_0, 0))
        self.connect((self.channels_channel_model_1, 0), (self.blocks_multiply_const_vxx_1, 0))
        self.connect((self.channels_channel_model_2, 0), (self.blocks_multiply_const_vxx_2, 0))
        self.connect((self.channels_channel_model_3, 0), (self.satellites_rms_agc_0, 0))
        self.connect((self.digital_constellation_modulator_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.digital_constellation_modulator_0_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.qpsk_decoder_0, 0), (self.blocks_unpack_k_bits_bb_0_0, 0))
        self.connect((self.satellites_rms_agc_0, 0), (self.qpsk_decoder_0, 0))
        self.connect((self.satellites_rms_agc_0_0, 0), (self.channels_channel_model_3, 0))


    def get_attacker_gain_db(self):
        return self.attacker_gain_db

    def set_attacker_gain_db(self, attacker_gain_db):
        self.attacker_gain_db = attacker_gain_db
        self.set_attacker_gain_multiplier(pow(10, self.attacker_gain_db/20))

    def get_channel_noise(self):
        return self.channel_noise

    def set_channel_noise(self, channel_noise):
        self.channel_noise = channel_noise
        self.channels_channel_model_1.set_noise_voltage(self.channel_noise)
        self.channels_channel_model_2.set_noise_voltage(self.channel_noise)

    def get_delay(self):
        return self.delay

    def set_delay(self, delay):
        self.delay = delay
        self.blocks_delay_0_0.set_dly(int(self.delay))

    def get_in_file(self):
        return self.in_file

    def set_in_file(self, in_file):
        self.in_file = in_file
        self.blocks_file_source_1.open(self.in_file, True)

    def get_num_packets(self):
        return self.num_packets

    def set_num_packets(self, num_packets):
        self.num_packets = num_packets
        self.blocks_head_0_0.set_length(1024*3*self.num_packets)
        self.blocks_head_0_0_0.set_length(1024*3*self.num_packets)

    def get_out_file(self):
        return self.out_file

    def set_out_file(self, out_file):
        self.out_file = out_file
        self.blocks_file_sink_0_0.open(self.out_file)

    def get_padding_bytes(self):
        return self.padding_bytes

    def set_padding_bytes(self, padding_bytes):
        self.padding_bytes = padding_bytes

    def get_processing_noise(self):
        return self.processing_noise

    def set_processing_noise(self, processing_noise):
        self.processing_noise = processing_noise
        self.channels_channel_model_3.set_noise_voltage(self.processing_noise)

    def get_qpsk(self):
        return self.qpsk

    def set_qpsk(self, qpsk):
        self.qpsk = qpsk

    def get_attacker_gain_multiplier(self):
        return self.attacker_gain_multiplier

    def set_attacker_gain_multiplier(self, attacker_gain_multiplier):
        self.attacker_gain_multiplier = attacker_gain_multiplier
        self.blocks_multiply_const_vxx_0.set_k(self.attacker_gain_multiplier)




def argument_parser():
    parser = ArgumentParser()
    parser.add_argument(
        "--attacker-gain-db", dest="attacker_gain_db", type=eng_float, default="0.0",
        help="Set attacker_gain_db [default=%(default)r]")
    parser.add_argument(
        "--channel-noise", dest="channel_noise", type=eng_float, default="1.0u",
        help="Set Background Channel Noise (V) [default=%(default)r]")
    parser.add_argument(
        "--delay", dest="delay", type=intx, default=56,
        help="Set Delay used for computing bit error rate [default=%(default)r]")
    parser.add_argument(
        "--in-file", dest="in_file", type=str, default='/data/satnogs/AQUA_PADDED.raw',
        help="Set Input file: bytes to overshadow [default=%(default)r]")
    parser.add_argument(
        "--num-packets", dest="num_packets", type=intx, default=1024,
        help="Set Number of Packets [default=%(default)r]")
    parser.add_argument(
        "--out-file", dest="out_file", type=str, default='/data/satnogs/OVERSHADOW.raw',
        help="Set Output file [default=%(default)r]")
    parser.add_argument(
        "--padding-bytes", dest="padding_bytes", type=intx, default=32,
        help="Set Padding Bytes [default=%(default)r]")
    parser.add_argument(
        "--processing-noise", dest="processing_noise", type=eng_float, default="200.0m",
        help="Set Noise in Processing (V) [default=%(default)r]")
    return parser


def main(top_block_cls=qpsk_overshadowing, options=None):
    if options is None:
        options = argument_parser().parse_args()
    tb = top_block_cls(attacker_gain_db=options.attacker_gain_db, channel_noise=options.channel_noise, delay=options.delay, in_file=options.in_file, num_packets=options.num_packets, out_file=options.out_file, padding_bytes=options.padding_bytes, processing_noise=options.processing_noise)

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
