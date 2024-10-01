#!/bin/sh

read line
printf "\\x$line"  > artifacts/out

while read line
do
  printf "\\x$line"  >> artifacts/out
done
