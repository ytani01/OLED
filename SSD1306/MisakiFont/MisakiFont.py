#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

# $ wget http://www.geocities.jp/littlimi/arc/misaki/misaki_ttf_2015-04-10.zip
FONT_PATH = '/home/pi/font/misakifont/misaki_gothic.ttf'

class MisakiFont:
    def __init__(self):
        self.str = ['','','','','','','','']
        self.char_width = 8
        self.char_height = 8
        self.cur_row = 0

        # Raspberry Pi pin configuration
        self.rst = 24

        #
        self.disp = Adafruit_SSD1306.SSD1306_128_64(rst=self.rst)

        # Initialize library.
        self.disp.begin()
        # Clear display.
        self.disp.clear()
        self.disp.display()

        # Create blank image for drawing.
        # Make sure to create image with mode '1' for 1-bit color.
        self.width = self.disp.width
        self.height = self.disp.height
        self.image = Image.new('1', (self.width, self.height))

        self.cols = int(self.width / self.char_width)
        self.rows = int(self.height / self.char_height)

        # Get drawing object to draw on image.
        self.draw = ImageDraw.Draw(self.image)
 
        # Draw a black filled box to clear the image.
        self.draw.rectangle((0,0,self.width,self.height), outline=0, fill=0)

        self.font = ImageFont.truetype(FONT_PATH, 8, encoding='unic')

    def clear(self):
        self.draw.rectangle((0,0,self.width,self.height), outline=0, fill=0)
        self.disp.image(self.image)
        #self.disp.clear()
        self.disp.display()

    def _draw1line(self, col, row, str):
        x = col * self.char_width
        y = row * self.char_height
        self.draw.text((x,y), str, font=self.font, fill=255)

    def println(self, str):
        self.str[self.cur_row] = str
        self.draw.rectangle((0,0,self.width,self.height), outline=0, fill=0)
        for r in range(self.rows):
            self._draw1line(0, r, self.str[r])
        self.disp.image(self.image)
        self.disp.display()
        self.cur_row += 1
        if self.cur_row > self.rows - 1:
            self.cur_row = self.rows - 1
            self.str.pop(0)
            self.str.append('')

if __name__ == '__main__':
    misakifont = MisakiFont()
    while True:
        misakifont.println('0123456789')
        misakifont.println('123456789')
        misakifont.println('2あいうえおかきくけこさしすせそたちつてと')
        misakifont.println('3あいうえおABC')
        misakifont.clear()
        misakifont.println('4ガギグゲゴ')
        misakifont.println('5ｶﾞｷﾞｸﾞｹﾞｺﾞ')
        misakifont.println('6あいうえお')
        misakifont.println('7あいうえお')
        print(misakifont.cur_row)
        misakifont.println('8あいうえお')
        print(misakifont.cur_row)
        misakifont.println('9あいうえお')
        print(misakifont.cur_row)
