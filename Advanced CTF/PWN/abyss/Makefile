all: main

main: abyss.asm
	nasm -f elf64 -o abyss.o abyss.asm
	ld -o main abyss.o
	rm -f abyss.o

clean:
	rm -f main abyss.o

.PHONY: all clean
