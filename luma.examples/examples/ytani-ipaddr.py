#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2014-18 Richard Hull and contributors
# See LICENSE.rst for details.
# PYTHON_ARGCOMPLETE_OK

"""
Simple println capabilities.
"""

import os
import time
import subprocess
from demo_opts import get_device
from luma.core.virtual import terminal
from PIL import ImageFont

font_set = ( "tiny.ttf", 6 )
#font_set = ( "ProggyTiny.ttf", 24 )
#font_set = ( "creep.bdf", 16 )
#font_set = ( "miscfs_.ttf", 12 )
font_set = ( "FreePixel.ttf", 15 )

def make_font(name, size):
    font_path = os.path.abspath(os.path.join(
        os.path.dirname(__file__), 'fonts', name))
    return ImageFont.truetype(font_path, size)


def get_ipaddr():
    proc = subprocess.run(["hostname -I | cut -f 1 -d ' '"], \
            shell=True, stdout=subprocess.PIPE)
    ip_str = proc.stdout.decode('UTF-8').strip('\n')
    return ip_str

def main():
    (font_name, font_size) = font_set
    font = make_font(font_name, font_size) if font_name else None
    term = terminal(device, font)

    while True:
        term.println(get_ipaddr())
        time.sleep(1)

    return

if __name__ == "__main__":
    try:
        device = get_device()
        main()
    except KeyboardInterrupt:
        pass
