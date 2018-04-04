# OLED-SSD1331

## ハードウェア

* [0.95インチ SPI OLEDディスプレイ 96x64ドット フルカラー SSD1331](https://www.amazon.co.jp/gp/product/B0711RKXB5/)

## 接続

* GND   --> 20, GND
* VCC
* SCL
* SDA
* RES
* DC
* CS    --> 24, BCM 8, SPI0 CE0

## 参考(オリジナル)

* [(GitHubGift)TheRayTracer/SSD1331.py](https://gist.github.com/TheRayTracer/dd12c498e3ecb9b8b47f)

## 変更点

```python:*.py
- SSD1331_PIN_CS = 23
+ SSD1331_PIN_CS = 8	# SPI0 CD0
```
