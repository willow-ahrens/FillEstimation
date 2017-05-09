#!/bin/sh

NUM=`basename $0 | sed 's/[^0-9]//g'`
${top_srcdir}/tests/format_battery.sh ${NUM} hbata

# eof
