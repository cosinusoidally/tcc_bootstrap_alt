./mk_clean

cd tcc_3

set -e

tcc tcc.c -I ../woody/usr/include/ -ldl
cd ../tcc_10

echo "tcc_3 to tcc_10"
../tcc_3/a.out ../tcc_3/tcc.c ../tcc_3/tcc.c ../tcc_3/tcc.c tcc.c -o tcc.o -c tcc.c
tcc tcc.o -ldl

echo "tcc_10 to tcc_19"
cd ../tcc_19

../tcc_10/a.out -o tcc.o -c tcc.c
../tcc_10/a.out -o libtcc1.o -c libtcc1_min.c
tcc libtcc1.o tcc.o -ldl

./a.out -c libtcc1.c
./a.out -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

./a.out -c libtcc1.c
./a.out -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

./a.out -c libtcc1.c
./a.out -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

cd ..

cd tcc_23

echo "tcc_19 to tcc_23"
../tcc_19/a.out tcc.c -c
../tcc_19/a.out libtcc1.c -c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

./a.out -c libtcc1.c
./a.out -I ../woody/usr/include/ -I . -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

echo "tcc_23 to tcc_24"
cd ../tcc_24

../tcc_23/a.out -I ../woody/usr/include/ -I . -c tcc.c
../tcc_23/a.out -c libtcc1.c
tcc libtcc1.o tcc.o -ldl

echo "tcc_24 to tcc_26"
cd ../tcc_26

../tcc_24/a.out -I ../woody/usr/include/ -I include -c tcc.c -DONE_SOURCE
../tcc_24/a.out -c ../tcc_24/libtcc1.c
tcc libtcc1.o tcc.o -ldl

./a.out -c ./lib/libtcc1.c
./a.out -I ../woody/usr/include/ -I ./include -c tcc.c -DONE_SOURCE
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

./a.out -c ./lib/libtcc1.c
./a.out -I ../woody/usr/include/ -I ./include -c tcc.c -DONE_SOURCE
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

./a.out -c ./lib/libtcc1.c
./a.out -I ../woody/usr/include/ -I ./include -c tcc.c -DONE_SOURCE
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

mv a.out ../artifacts/tcc_26_boot.exe

cd ..

./artifacts/tcc_26_boot.exe -c tcc_bootstrap.c

sha256sum tcc_bootstrap.o
sha256sum -c sum
