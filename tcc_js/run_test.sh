for i in test*js
do
echo $i
js $i|tail -n 1|grep "OK\|BAD"
done
