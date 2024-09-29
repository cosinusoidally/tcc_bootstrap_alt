#!/bin/sh

awk -W posix 'BEGIN { exit }'
if [ $? -eq 0 ]; then
  echo "-W posix"
else
  echo ""
fi
