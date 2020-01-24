all: tp0

tp0: main.c
	gcc -Wno-builtin-declaration-mismatch -o tp0 main.c

tests: tp0
	./tp0

clean:
	-rm -f tp0
