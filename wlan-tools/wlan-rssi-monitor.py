#!/usr/bin/env python
# coding: utf-8
# wlan-rssi-monitor: monitor the RSSI received from a given MAC
# Copyright (C) 2016 Iñaki Ucar  <i.ucar86@gmail.com>

import argparse, pcapy, struct, sys
from time import time
from collections import deque
from matplotlib import pyplot as plt

# Radiotap "present" field bits
RTAP_TSFT = 0
RTAP_FLAGS = 1
RTAP_RATE = 2
RTAP_CHANNEL = 3
RTAP_FHSS = 4
RTAP_DBM_ANTSIGNAL = 5
RTAP_DBM_ANTNOISE = 6
RTAP_LOCK_QUALITY = 7
RTAP_TX_ATTENUATION = 8
RTAP_DB_TX_ATTENUATION = 9
RTAP_DBM_TX_POWER = 10
RTAP_ANTENNA = 11
RTAP_DB_ANTSIGNAL = 12
RTAP_DB_ANTNOISE = 13
RTAP_FCS = 14 # XXX: Not standard, but madwifi uses it.
RTAP_EXT = 31 # Denotes extended "present" fields.

# plot class
class Plot:
    def __init__(self, maxT):
        self.x = deque()
        self.y = deque()
        self.init = time()
        self.maxT = maxT
        self.line, = plt.plot(self.x, self.y)
        plt.ylim([-100, 0])
        self._redraw()

    def _redraw(self):
        if (len(self.x) > 1):
            plt.xlim([min(self.x), max(self.x)])
        plt.draw()
        plt.pause(1e-4)

    # update plot with a new point
    def update(self, data):
        t = time() - self.init
        self.x.append(t)
        self.y.append(data)
        while self.x[-1] - self.x[0] > self.maxT:
            self.x.popleft()
            self.y.popleft()
        self.line.set_data(self.x, self.y)
        self._redraw()

class Parser:
    def __init__(self, dev, plot, xAxis=10):
        max_bytes = 200
        promiscuous = True
        read_timeout = 0 # in milliseconds
        self.pc = pcapy.open_live(dev, max_bytes, promiscuous, read_timeout)
        if plot:
            self.plot = Plot(xAxis)
        else: self.plot = None

    # what to do with every packet
    def recv_pkts(self, hdr, data):
        flags, = struct.unpack('<xxxxI', data[0:8])

        # we want this field
        if flags & 1 << RTAP_DBM_ANTSIGNAL:
            # skip stuff
            offset = 0
            if flags & 1 << RTAP_TSFT:
                offset += 8
            if flags & 1 << RTAP_FLAGS:
                offset += 1
            if flags & 1 << RTAP_RATE:
                offset += 1
            if flags & 1 << RTAP_CHANNEL:
                offset += 4
            if flags & 1 << RTAP_FHSS:
                offset += 2

            # get SSI Signal [dBm]
            power, = struct.unpack('b', data[8+offset])
            if self.plot:
            	self.plot.update(power)
            else:
            	print power
            	sys.stdout.flush()

    # fun!
    def run(self):
        try:
            # capture packets (indefinitely)
            self.pc.loop(-1, self.recv_pkts)
        except KeyboardInterrupt:
            pass

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Not so bad (nor so good) wireless RSSI monitor.')
    parser.add_argument('dev', type=str, help='network device')
    parser.add_argument('MAC', type=str, help='MAC address you want to monitor')
    parser.add_argument('-p', dest='p', action='store_true', help='plot data')
    args = parser.parse_args()

    parser = Parser(args.dev, args.p)
    parser.pc.setfilter('ether src ' + args.MAC)
    parser.run()
