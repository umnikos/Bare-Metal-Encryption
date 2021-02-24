home=../..
a.out: $(home)/encryption/*.c $(home)/encryption/*.h Makefile *.c
	gcc -Wall -g -O2 -DBN_NO_ASSERT $(home)/encryption/*.c $(home)/tiny-bignum-c/bn.c *.c

clean:
	rm a.out
