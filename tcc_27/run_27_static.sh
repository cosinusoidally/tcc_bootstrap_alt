rm libc_boot4*.o tcc_new*.o libtcc1_new*.o libc_static*.o ../artifacts/tcc_27_boot_static*.exe

./run_27_static.kaem

chmod +x ../artifacts/tcc_27_boot_static.exe

# test static build

../artifacts/tcc_27_boot_static.exe -I ../woody/usr/include/ -I include -c tcc.c -DONE_SOURCE -o tcc_new2.o
../artifacts/tcc_27_boot_static.exe -c ./lib/libtcc1.c -o libtcc1_new2.o

../artifacts/tcc_27_boot_static.exe -c libc_boot4.c -o libc_boot4_new2.o

../artifacts/tcc_27_boot_static.exe -c libc_static.c -o libc_static_new2.o

../artifacts/tcc_27_boot_static.exe ../artifacts/tcc_27_boot_static_test.exe

../artifacts/tcc_27_boot_static.exe -nostdlib -static libc_static_new2.o libc_boot4_new2.o libtcc1_new2.o tcc_new2.o -o ../artifacts/tcc_27_boot_static_test.exe

chmod +x ../artifacts/tcc_27_boot_static_test.exe

sha256sum *o
ls -l *o
ls -l ../artifacts/tcc_27_boot_static.exe
diff -u -s libc_boot4_new.o libc_boot4.o
diff -u -s libtcc1_new.o libtcc1.o
diff -u -s tcc_new.o tcc.o
diff -u -s ../artifacts/tcc_27_boot_static.exe ../artifacts/tcc_27_boot_static_test.exe
