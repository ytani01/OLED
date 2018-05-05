#!/bin/sh
#

PYTHON_CMD="python3"
OPTS="-d ssd1331 --interface spi --width 96"

while [ "x$1" != "x" ]; do
  CMDLINE="${PYTHON_CMD} $1 ${OPTS}"
  echo "CMDLINE=${CMDLINE}"
  eval ${CMDLINE}
  shift
done
