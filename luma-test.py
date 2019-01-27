#!/usr/bin/env python3

import time
from PIL import Image, ImageDraw, ImageFont
from luma.core.interface.serial import i2c
#from luma.core.render import canvas
from luma.oled.device import ssd1327

FONT_DIR  = '/home/pi/font'
FONT_NAME = 'misakifont/misaki_gothic.ttf'
FONT_PATH = FONT_DIR + '/' + FONT_NAME
print(FONT_PATH)

def print_sec(t1=0, prefix_text=''):
    t = time.time()
    print('%s:%.2f' % (prefix_text, (t -t1)))
    return t

t1 = time.time()

serial = i2c(port=1, address=0x3C)
t1 = print_sec(t1, 'i2c()')

device = ssd1327(serial)
t1 = print_sec(t1, 'ssd1327()')

image = Image.new("RGB", device.size)
t1 = print_sec(t1, 'Image.new()')

draw = ImageDraw.Draw(image)
t1 = print_sec(t1, 'ImageDraw.Draw')


font = ImageFont.truetype(FONT_PATH, 8, encoding='unic')
t1 = print_sec(t1, 'ImageFont.truetype()')

text = '本日は晴天なり。'
draw.text((10, 20), text, font=font, fill=255)
t1 = print_sec(t1, 'text' + ':255')

draw.text((10, 30), text, font=font, fill=192)
t1 = print_sec(t1, 'text' + ':192')

draw.text((10, 40), text, font=font, fill=128)
t1 = print_sec(t1, 'text' + ':128')

device.display(image)
t1 = print_sec(t1, 'device.display()')

time.sleep(5)
    
print('---')

    
