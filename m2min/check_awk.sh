#!/bin/sh

awk -W posix 'BEGIN { exit }'
if [ $? -eq 0 ]; then
  CMD="-W posix"
else
  CMD=""
fi

# EXTRA=" -v use_to_bin=bash"

echo $CMD $EXTRA
