#!/bin/sh
# (c) 2018 Yoichi Tanibayashi
#
MYNAME=`basename $0`

BINDIR=${HOME}/bin
LOGDIR=${HOME}/tmp

MISAKI_FONT=${BINDIR}/MisakiFont.py

LOGFILE=${LOGDIR}/${MYNAME}.log

if [ ! -d ${LOGDIR} ]; then
    mkdir ${LOGDIR}
fi

exec ${MISAKI_FONT} > ${LOGFILE} 2>&1

