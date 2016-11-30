#!/usr/bin/env python
# coding: utf-8
# wlan-extract-bytes: extract n bytes at index i for each packet in a PCAP file
# Copyright (C) 2016 Iñaki Ucar <i.ucar86@gmail.com>

import argparse, pcapy, struct

DLT_IEEE802_11 = 105
DLT_PRISM_HEADER = 119
DLT_IEEE802_11_RADIO = 127

def check_negative(value):
    ivalue = int(value)
    if ivalue < 0:
        raise argparse.ArgumentTypeError("%s is an invalid positive int value" % value)
    return ivalue

parser = argparse.ArgumentParser(description='Extracts "n" bytes at index "i".')
parser.add_argument('-v', dest='verbose', action='store_true', help='verbose')
parser.add_argument('-o', dest='offset', default=0, type=check_negative, help='offset')
parser.add_argument('-u', dest='unpack', required=True, type=str, help='unpacking format (struct.unpack)')
parser.add_argument('file', type=str, help='PCAP file')
args = parser.parse_args()

reader = pcapy.open_offline(args.file)
if reader.datalink() == DLT_IEEE802_11:
    pass
elif reader.datalink() == DLT_PRISM_HEADER:
    args.offset += 144
elif reader.datalink() == DLT_IEEE802_11_RADIO:
    header, payload = reader.next()
    rt_len, = struct.unpack_from('<xxH', payload)
    args.offset += rt_len
else:
    raise "Unknown datalink type"
reader = pcapy.open_offline(args.file)

while True:
    try:
        header, payload = reader.next()

        value, = struct.unpack_from(args.unpack, payload, args.offset)
        print value

    except pcapy.PcapError:
        break
