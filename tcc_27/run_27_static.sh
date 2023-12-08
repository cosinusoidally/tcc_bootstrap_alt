rm libc_boot4*.o tcc_new.o libtcc1_new.o libc_static.o ../artifacts/tcc-27-static.exe

./run_27_static.kaem

chmod +x ../artifacts/tcc_27_boot_static.exe

../artifacts/tcc_27_boot_static.exe -I ../woody/usr/include/ -I include -c tcc.c -DONE_SOURCE -o tcc_new2.o

sha256sum *o
ls -l *o
ls -l ../artifacts/tcc_27_boot_static.exe
diff -u -s libc_boot4_new.o libc_boot4.o
diff -u -s libtcc1_new.o libtcc1.o
diff -u -s tcc_new.o tcc.o
