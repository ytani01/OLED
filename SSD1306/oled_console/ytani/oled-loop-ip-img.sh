#!/bin/sh
#

HOME="/home/pi"
BINDIR=${HOME}/bin

CMD=${BINDIR}/oled-ip-img.sh

while true; do
	echo ${CMD}
	${CMD}
	sleep 2
done
