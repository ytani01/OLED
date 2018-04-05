#!/bin/sh

HOME=/home/pi

HEAD_PBM="${HOME}/etc/bebridge-128x26.pbm"

#FONT="FreeMono"
FONT="FreeSans"
POINTSIZE=12

CMD_DISPIMG="${HOME}/bin/dispImg"

TEXT_PBM="${HOME}/tmp/text.pbm"
OUT_PBM="${HOME}/tmp/out.pbm"

#
# main
#

if [ ! -x ${CMD_DISPIMG} ]; then
	echo "command file not found: ${CMD_DISPIMG}"
	exit 1
fi

IPADDR=`hostname -I | sed 's/ .*$//'`
echo ${IPADDR}
LABEL_STR="\\nIP: ${IPADDR}\\n"

convert -font ${FONT} -pointsize ${POINTSIZE} \
	label:"${LABEL_STR}\n" ${TEXT_PBM}

convert -append ${HEAD_PBM} ${TEXT_PBM} ${OUT_PBM} 

TERM=xterm setterm -cursor off > /dev/tty1

exec sudo ${CMD_DISPIMG} ${OUT_PBM}
