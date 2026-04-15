all: main

main: main.c
	gcc -no-pie -z execstack main.c -o main

clean:
	rm -f main

.PHONY: all clean
