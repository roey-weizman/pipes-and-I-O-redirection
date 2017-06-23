
#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int
main(int argc, char *argv[])
{
    int pipeFileDescriptors[2];//[0]-read part, [1]-write part
    pid_t childpid;
    char buffer;//buufer for reading


   // assert(argc == 2);


    if (pipe(pipeFileDescriptors) == -1) {//pipe opening fails
     perror("pipe"); exit(EXIT_FAILURE); 
     }


    childpid = fork();//opening a child process
    if (childpid == -1) { 
        perror("fork"); exit(EXIT_FAILURE); 
    }


    if (childpid == 0) {    
      
        close(pipeFileDescriptors[0]);       //not reading in child thus closing//
        write(pipeFileDescriptors[1], "hello", 5);// write to the writing side of pipe//general  argv[1], strlen(argv[1])
        close(pipeFileDescriptors[1]);         //finish thus closing
        wait(NULL);             /* Wait for child */
        exit(EXIT_SUC   CESS);

    } 
    else {  
        close(pipeFileDescriptors[1]);        //just reading
        

        while (read(pipeFileDescriptors[0], &buffer, 1) > 0)//reading char by char to stdout from the side of the chils was writen to monitor
        write(STDOUT_FILENO, &buffer, 1);


        write(STDOUT_FILENO, "\n", 1);
        close(pipeFileDescriptors[0]);
        _exit(EXIT_SUCCESS);

    }
}
