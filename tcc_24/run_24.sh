tcc libtcc1.o tcc.o -ldl

./a.out -I ../woody/usr/include/ -I . -I include -c tcc.c -DONE_SOURCE -o tcc_new.o
./a.out -c ../tcc_24/libtcc1.c -o libtcc1_new.o
