# build libc_min.o

# clear libc_min.o

../x86/bin/catm libc_min.o

time ../x86/bin/elf_loader_cc_x86.exe -l ../tcc_10/libc_boot.o -l ../tcc_10/tcc.o -o libc_min.o -c ../tcc_10/libc_min.c

# make sure ..new.o files are empty

../x86/bin/catm tcc_new.o
../x86/bin/catm libtcc1_new.o

# run tcc_27
time ../x86/bin/elf_loader_cc_x86.exe -l libtcc1.o -l libc_min.o -l tcc.o -c ./lib/libtcc1.c -o libtcc1_new.o
time ../x86/bin/elf_loader_cc_x86.exe -l libtcc1.o -l libc_min.o -l tcc.o -I ../woody/usr/include/ -I include -c tcc.c -DONE_SOURCE -o tcc_new.o

sha256sum *o
diff -u -s libtcc1_new.o libtcc1.o
diff -u -s tcc_new.o tcc.o
