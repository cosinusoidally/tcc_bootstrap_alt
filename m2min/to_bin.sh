#!/bin/bash

read i
printf "\x$i"  > $1

while read i
do
  printf "\x$i"  >> $1
done
