# run tcc_27
../x86/bin/elf_loader_cc_x86.exe -l libtcc1.o -l ../tcc_26/libc_boot3.o -l tcc.o -c ./lib/libtcc1.c -o libtcc1_new.o
../x86/bin/elf_loader_cc_x86.exe -l libtcc1.o -l ../tcc_26/libc_boot3.o -l tcc.o -I ../woody/usr/include/ -I include -c tcc.c -DONE_SOURCE -o tcc_new.o

../x86/bin/catm tcc.o tcc_new.o
../x86/bin/catm libtcc1.o libtcc1_new.o
