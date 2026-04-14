main:main.c
	gcc -Wl,-z,relro,-z,now  -o main main.c -fstack-protector

test:test.c
	gcc -Wl,-z,relro,-z,now -no-pie -o test test.c -fstack-protector

clean:
	rm main