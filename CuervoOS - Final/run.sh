#assemble boot.s file
as --32 boot.s -o boot.o

#compile kernel.c file
gcc -m32 -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O1 -Wall -Wextra

gcc -m32 -c utils.c -o utils.o -std=gnu99 -ffreestanding -O1 -Wall -Wextra

gcc -m32 -c char.c -o char.o -std=gnu99 -ffreestanding -O1 -Wall -Wextra

#linking the kernel with kernel.o and boot.o files
ld -m elf_i386 -T linker.ld kernel.o utils.o char.o boot.o -o CuervoOS.bin -nostdlib

#check CuervoOS.bin file is x86 multiboot file or not
grub-file --is-x86-multiboot CuervoOS.bin

#building the iso file
mkdir -p isodir/boot/grub
cp CuervoOS.bin isodir/boot/CuervoOS.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o CuervoOS.iso isodir --xorriso="$HOME"/xorriso/xorriso-1.4.6/xorriso/xorriso

#run it in qemu
qemu-system-x86_64 -cdrom CuervoOS.iso
