for i in test*js
do
echo $i
js $i|grep "OK\|BAD"
done
