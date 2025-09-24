#!/bin/bash
pkill -9 jackd
pkill -9 alsa
pkill -9 all
export GST_DEBUG=3,timing:5
/home/fs/VoIP-Smartlite/rpi/voip_rpi
