all: mypipe
 
mypipe:  mypipe.o 
	gcc -Wall  -g  -o mypipe mypipe.o 
mypipe.o : mypipe.c
	gcc -Wall  -g -c mypipe.c
clean:
	rm -f *.o  mypipe
