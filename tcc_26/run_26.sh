tcc libtcc1.o tcc.o -ldl

echo "rebuild tcc_26 with tcc_26"
./a.out -c ./lib/libtcc1.c -o libtcc1_new.o
./a.out -I ../woody/usr/include/ -I ./include -c tcc.c -DONE_SOURCE -o tcc_new.o
../check_syms.sh
tcc libtcc1_new.o tcc_new.o -ldl
rm tcc_new.o
rm libtcc1_new.o

./a.out -c ./lib/libtcc1.c -o libtcc1_new.o
./a.out -I ../woody/usr/include/ -I ./include -c tcc.c -DONE_SOURCE -o tcc_new.o
../check_syms.sh
tcc libtcc1_new.o tcc_new.o -ldl
rm tcc_new.o
rm libtcc1_new.o

./a.out -c ./lib/libtcc1.c -o libtcc1_new.o
./a.out -I ../woody/usr/include/ -I ./include -c tcc.c -DONE_SOURCE -o tcc_new.o
../check_syms.sh
tcc libtcc1_new.o tcc_new.o -ldl

sha256sum *_new.o
