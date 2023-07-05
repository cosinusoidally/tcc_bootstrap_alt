echo ""
echo "$PWD:"
echo ""
echo "global symbols from libtcc1...."
for i in lib*o;do
  echo $i
  readelf -a $i|grep GLOBAL
done
echo ""
echo "undefined symbols:"
for i in *.o;do
  echo $i
  readelf -a $i|grep UND|grep __
done

# supress non-zero exit codes since there will be a non-zero exit code if there
# are no undefined symbols (which is expected).
exit 0
