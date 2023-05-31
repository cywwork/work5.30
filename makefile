main:main.o add.o
	gcc -o main main.o add.o

%.o:%.c
	gcc -c -o $@ $<

add.c : add.h

clean:
	rm *.o test -f
