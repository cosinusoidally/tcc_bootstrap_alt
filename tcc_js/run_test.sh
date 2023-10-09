for i in $(seq 10)
do
echo test$i
js test$i.js|tail -n 1|grep "OK\|BAD"
done
