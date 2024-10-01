# dash is a problem (hex escapes don't work) if you hit this then use bash
# or busybox ash

read i
printf "\x$i"  > $1

while read i
do
  printf "\x$i"  >> $1
done
