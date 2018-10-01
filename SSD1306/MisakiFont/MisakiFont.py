#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import Adafruit_SSD1306
from PIL import Image, ImageDraw, ImageFont
import textwrap
import mojimoji
import time

from ipaddr import ipaddr

# $ wget http://www.geocities.jp/littlimi/arc/misaki/misaki_ttf_2015-04-10.zip
FONT_PATH = '/home/pi/font/misakifont/misaki_gothic.ttf'

class MisakiFont:
    def __init__(self, zenkaku=True, fontsize=8, rst=24):
        self.zenkaku_flag = zenkaku
        self.fontsize = fontsize
        self.rst = rst

        self.str = []
        self.char_width = self.fontsize
        self.char_height = self.fontsize
        self.cur_row = 0
        self.enable = True

        # initialize display
        self.disp = Adafruit_SSD1306.SSD1306_128_64(rst=self.rst)
        #self.disp = Adafruit_SSD1306.SSD1306_96_16(rst=self.rst)
        try:
            self.disp.begin()
        except:
            self.enable =False
            return
        self.disp.clear()
        self.disp.display()

        # load font
        self.font = ImageFont.truetype(FONT_PATH, self.fontsize, encoding='unic')
        (self.char_width, self.char_height) = self.font.getsize('８')
        self.char_height += 1
        #print((self.char_width, self.char_height))
        
        # cols and rows
        self.cols = int(self.disp.width / self.char_width)
        self.rows = int(self.disp.height / self.char_height)
        for i in range(self.rows):
            self.str.append("")

        # Create blank image for drawing.
        # Make sure to create image with mode '1' for 1-bit color.
        self.image = Image.new('1', (self.disp.width, self.disp.height))

        # Get drawing object to draw on image.
        self.draw = ImageDraw.Draw(self.image)
 
        # Draw a black filled box to clear the image.
        self.draw.rectangle((0,0,self.disp.width,self.disp.height), outline=0, fill=0)


    def clear(self):
        if not self.enable:
            return
        self.draw.rectangle((0,0,self.disp.width,self.disp.height), outline=0, fill=0)
        self.disp.image(self.image)
        #self.disp.clear()
        self.disp.display()
        self.cur_row = 0
        for i in range(self.rows):
            self.str[i] = ''

    def _draw1line(self, col, row, str):
        if not self.enable:
            return
        x = col * self.char_width
        y = row * self.char_height
        self.draw.text((x,y), str, font=self.font, fill=255)

    def println1(self, str):
        if not self.enable:
            return
        self.str[self.cur_row] = str
        self.draw.rectangle((0,0,self.disp.width,self.disp.height), outline=0, fill=0)
        for r in range(self.rows):
            self._draw1line(0, r, self.str[r])
        self.disp.image(self.image)
        self.disp.display()
        self.cur_row += 1
        if self.cur_row > self.rows - 1:
            self.cur_row = self.rows - 1
            self.str.pop(0)
            self.str.append('')

    def println(self, s):
        if not self.enable:
            return
        if self.zenkaku_flag:
            s=mojimoji.han_to_zen(s)
        for s1 in textwrap.fill(s, self.cols).split('\n'):
            self.println1(s1)

if __name__ == '__main__':
    misakifont = MisakiFont()
    print('font ' + str(misakifont.fontsize))
    print('char ' + str(misakifont.char_width) + 'x' + str(misakifont.char_height))
    print('disp ' + str(misakifont.cols) + 'x' + str(misakifont.rows))
    while True:
        misakifont.println('123456789')
        misakifont.println('2あいうえおかきくけこさしすせそたちつてと')
        misakifont.println('3あいうえおABC')
        time.sleep(2)
        misakifont.clear()
        misakifont.println('あいうえお')
        misakifont.println('0123456789')
        misakifont.println('ｶﾞｷﾞｸﾞｹﾞｺﾞ')
        misakifont.println('')
        misakifont.println('font ' + str(misakifont.fontsize))
        misakifont.println(str(misakifont.char_width) + 'x' +
                           str(misakifont.char_height) + 'pixels')
        misakifont.println(str(misakifont.cols) + ' cols ' +
                           str(misakifont.rows) + ' rows')
        misakifont.println(ipaddr().ip_addr())
        break
