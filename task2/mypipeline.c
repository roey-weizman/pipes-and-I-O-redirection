#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int  main(int argc,char** argv){
 	int pipeFileDescriptors[2];
 	int debugMode=0; 

	for(int i=0;i<argc;i++){
          if(strcmp(argv[i], "-d") == 0)
               debugMode=1;
     }

 	if (pipe(pipeFileDescriptors) == -1) {//pipe opening fails
	     perror("pipe");
	     exit(EXIT_FAILURE); 
     }
     if(debugMode==1){
     	fprintf(stderr, "%s", "parent_process>forking…\n");
     }
     int processID1=fork();
     if(debugMode==1){
     	fprintf(stderr, "%s %d\n", "parent_process>created process with id:",processID1);

     }
     
     	if(processID1<0){
     	 	perror("**fork() method failed\n");
         	exit(1);
         }
        else if(processID1==0){
        	close(1);
        	int dupChild1 = dup(pipeFileDescriptors[1]);
        	if(debugMode==1){
     	fprintf(stderr, "%s", "child1>redirecting stdout to the write end of the pipe…\n");
     	}
        	close(pipeFileDescriptors[1]);
        	char* arguments1={"ls","-l",0};
        	if(debugMode==1){
     	fprintf(stderr, "%s %s\n", "child1>going to execute cmd:",arguments1[0]);
     }
        	if (execvp (arguments1[0],arguments1) < 0) {     /* execute the command  */
                   perror("** execv failed\n");
                   exit(-1);
              }
        }
        else{
	        if(debugMode==1){
	     		fprintf(stderr, "%s", "parent_process>closing the write end of the pipe…\n");
	    	 } 
	        	if(debugMode==1){
        fprintf(stderr, "%s", "parent_process>waiting for child processes to terminate…\n");
     }
                //child 2
                int status1;
                waitpid(-1,&status1,0);

                close(pipeFileDescriptors[1]);
             if(debugMode==1){
        fprintf(stderr, "%s", "parent_process>forking…\n");
     }
     int processID2=fork();
     if(debugMode==1){
        fprintf(stderr, "%s %d\n", "parent_process>created process with id:",processID2);

     }

    if(processID2<0){
            perror("**fork() method failed\n");
            exit(1);
         }
        if(processID2==0){
            close(0);
            if(debugMode==1){
        fprintf(stderr, "%s", "child2>redirecting stdin to the read end of the pipe…\n");
     }
            int dupChild2 = dup(pipeFileDescriptors[0]);
            close(pipeFileDescriptors[0]);
            char * arguments2={"tail","-n","2",0};
            if(debugMode==1){
        fprintf(stderr, "%s %s\n", "child2>going to execute cmd:",arguments2[0]);
     }
            if (execvp (arguments2[0],arguments2) < 0) {     /* execute the command  */
                   perror("** execv failed\n");
                   exit(-1);
              }
         }
        else{ 
                if(debugMode==1){
                        fprintf(stderr, "%s", "parent_process>closing the read end of the pipe…\n");
                     } 
            int status2; 
    if(debugMode==1){
        fprintf(stderr, "%s", "parent_process>waiting for child processes to terminate…\n");
     }
        close(pipeFileDescriptors[0]);
            waitpid(-1,&status2,0);
            
     
 

     if(debugMode==1){
     	fprintf(stderr, "%s","parent_process>exiting\n");
     }
    }
}
 }