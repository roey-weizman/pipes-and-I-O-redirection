all: mypipeline
 
mypipeline: mypipeline.o 
	gcc -Wall  -g  -o mypipeline mypipeline.o 
mypipeline.o : mypipeline.c
	gcc -Wall  -g -c mypipeline.c
clean:
	rm -f *.o  mypipeline
