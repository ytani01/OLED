#!/bin/sh

echo
echo "----- OLED Display setup [SSD1306]"
echo "(Push Enter)"
read Wait

cp oled /bin/oled
chmod a+x /bin/oled

export DEBIAN_FRONTEND=noninteractive
apt-get -y install console-data

/bin/grep  "128x64-60" /etc/fb.modes
while [ ${?} = 1 ]
do
cat<<'END1' >> /etc/fb.modes

mode "128x64-60"
    # D: 25.175 MHz, H: 31.469 kHz, V: 59.94 Hz
    geometry 128 64 128 64 8
    timings 39722 48 16 33 10 96 2
endmode
END1
done

cat<<'END2' > /etc/systemd/system/oled.service
[Unit]
Description = oled daemon
#After = multi-user.target
After=display-manager.service
[Service]
ExecStartPre=/bin/bash -c "/bin/sleep 10; /bin/chvt 1; /bin/fbset '128x64-60'; /bin/setfont 'alt-8x8'; export TERM=linux; setterm -blank 0 > /dev/tty1"
ExecStopPost=/bin/bash -c "/bin/fbset '800x600-60'; /bin/setfont 'default8x16'"
ExecStart = /bin/oled
Restart = always
Type = simple
[Install]
WantedBy = multi-user.target
END2

systemctl enable oled 

echo --------------------------------------------------------
echo Please execute "/usr/bin/raspi-config" AND Enable I2C!!!
echo --------------------------------------------------------
