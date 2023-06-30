./mk_clean

cd tcc_3

set -e

tcc tcc.c -I /home/foo/boot/woody/usr/include/ -ldl
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

mv a.out ../tcc_19_boot1

sha256sum tcc_19_boot1

cd tcc_23

./configure
../tcc_19_boot1 tcc.c -c
../tcc_19_boot1 libtcc1.c -c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

./a.out -c libtcc1.c
./a.out -I /home/foo/boot/woody/usr/include/ -I . -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

cd ../tcc_24
./configure
../tcc_23/a.out -I /home/foo/boot/woody/usr/include/ -I . -c tcc.c
../tcc_23/a.out -c libtcc1.c
tcc libtcc1.o tcc.o -ldl

echo "version 26"
cd ../tcc_26
./configure
../tcc_24/a.out -I /home/foo/boot/woody/usr/include/ -I include -c tcc.c -DONE_SOURCE
../tcc_24/a.out -c ../tcc_24/libtcc1.c
tcc libtcc1.o tcc.o -ldl

./a.out -c libtcc1.c
./a.out -I /home/foo/boot/woody/usr/include/ -I . -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

./a.out -c libtcc1.c
./a.out -I /home/foo/boot/woody/usr/include/ -I . -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

./a.out -c libtcc1.c
./a.out -I /home/foo/boot/woody/usr/include/ -I . -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

mv a.out ../tcc_26_boot1
make clean

./configure

tcc tcc.c -ldl -DONE_SOURCE

./a.out -c libtcc1.c
./a.out -I /home/foo/boot/woody/usr/include/ -I . -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

./a.out -c libtcc1.c
./a.out -I /home/foo/boot/woody/usr/include/ -I . -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

./a.out -c libtcc1.c
./a.out -I /home/foo/boot/woody/usr/include/ -I . -c tcc.c
tcc libtcc1.o tcc.o -ldl
rm tcc.o
rm libtcc1.o

mv a.out ../tcc_26_boot2

cd ..
sha256sum tcc_26_boot1
sha256sum tcc_26_boot2

./tcc_26_boot1 -c tcc_bootstrap.c

sha256sum tcc_bootstrap.o
sha256sum -c sum
