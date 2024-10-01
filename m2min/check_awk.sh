#!/bin/sh

export LC_ALL=C

awk -W posix 'BEGIN { exit }'
if [ $? -eq 0 ]; then
  CMD="-W posix"
else
  CMD=""
fi

SNAME="bash"

# prefer ash for to_bin.sh
ash --help

if [ $? -eq 0 ]; then
  SNAME="ash"
fi

EXTRA_ARGS=" -v use_to_bin=$SNAME"

# checking if we can use printf to output bytes
COUNT=$(awk 'BEGIN {printf("%c",0);exit}'|wc -c)

if [ $COUNT -ne 1 ]; then
  EXTRA=$EXTRA_ARGS
fi

COUNT=$(awk 'BEGIN {printf("%c",255);exit}'|wc -c)

if [ $COUNT -ne 1 ]; then
  EXTRA=$EXTRA_ARGS
fi

echo $CMD $EXTRA
