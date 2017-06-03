default_action: 
	gcc -Wall -Werror -Wno-unused-function -O -o main.o main.c BigReal.c testBigReal.c
	./main.o