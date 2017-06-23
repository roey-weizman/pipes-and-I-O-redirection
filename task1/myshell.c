#include "LineParser.h"
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h> 
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int  main(int argc,char** argv){
     char  myLine[2048];       int debugMode=0;  
     char  currDir[PATH_MAX];            
     cmdLine* myCmd; 
     char slashBuf[1]={'/'};
     char errBuf[28];
     
     for(int i=0;i<argc;i++){
          if(strcmp(argv[i], "-d") == 0)
               debugMode=1;
     }
     while (1) { 
          getcwd(currDir, sizeof(currDir));                  /* repeat until done ....         */
          do{ 
          printf("#");
          printf(currDir);     /*   display a prompt             */
          printf("# ");
          gets(myLine);  
          printf("\n");}
          while(strcmp(myLine,"") == 0);

          myCmd=parseCmdLines(myLine);            /*   read in the command line     */
          if (strcmp(myCmd->arguments[0], "quit") == 0){  /* is it an "exit"?     */
               if(debugMode==1){
                int pid= getpid();
                debugPrint(pid,"quit");
                  }
                exit(0); 
               } 
               if (strcmp(myCmd->arguments[0], "cd") == 0){  /* is it an "exit"?     */
                    char dest[sizeof(currDir)+1+sizeof(myCmd->arguments[1])];
                    sprintf(dest, "%s%s", currDir, slashBuf);

                    sprintf(dest, "%s%s", dest, myCmd->arguments[1]);
                    if(chdir(dest)<0){
                      sprintf(errBuf, "Error executing cd command\n\n");
                     write(2, errBuf, strlen(errBuf));
                     }
               if(debugMode==1){
                int pid= getpid();
                debugPrint(pid,"cd");
                  }
                  continue;
               }
                     /*   exit if it is                */
                int blocking=(int)(myCmd->blocking);
          execute(myCmd,debugMode,blocking); 
          freeCmdLines(myCmd);          /* otherwise, execute the command */
     }
     return 0;
}

void  execute(cmdLine *pCmdLine,int debugMode,int blocking){
    pid_t  pidNum;
    int    status;
    int died=0;
    int pipeFileDescriptors[2];//[0]-read part, [1]-write part
    int file1;
    int file2;
    int stdin_copy = dup(0);
    int stdout_copy = dup(1);
    
    if (pipe(pipeFileDescriptors) == -1) {//pipe opening fails
     perror("pipe");
      exit(EXIT_FAILURE); 
     }

     if ((pidNum = fork()) < 0) {     /* fork a child process           */
          perror("**fork() method failed\n");
          exit(1);
     }

     else if (pidNum == 0) {          /* for the child process:         */
          if(pCmdLine->inputRedirect && !pCmdLine->outputRedirect)
            reDirectInputAlone(file1,pCmdLine,stdin_copy);
          else if(!pCmdLine->inputRedirect &&pCmdLine->outputRedirect)
            reDirectOutputAlone(file2,pCmdLine,stdout_copy);
          else if(pCmdLine->inputRedirect && pCmdLine->outputRedirect)
            reDirectOutputAndInputTogether(file1,file2,pCmdLine,stdin_copy,stdout_copy);
          else {
              if (execvp (pCmdLine->arguments[0], pCmdLine->arguments) < 0) {     /* execute the command  */
                   perror("** execv failed\n");
                   exit(-1);
              }
              if(debugMode==1)
                   debugPrint(pidNum,pCmdLine->arguments[0]);
            }
      }
     else {    
             if(debugMode==1)
               debugPrint(pidNum,pCmdLine->arguments[0]); 
             
            if(blocking==1)
                while (wait(&status) != pidNum);
             else {

              kill(pidNum, SIGTERM);

              
              for (int loop=0 ;!died && loop < 5 ; ++loop){
                        sleep(1);
                  if (waitpid(pidNum, &status, WNOHANG) == pidNum) 
                    died = 1;
              }

              if (!died) 
                kill(pidNum, SIGKILL);



             }

        }
}

void signalHandler( int gotSignal){
    const char *signal_name;

    // Find out which signal we're handling
    switch (gotSignal) {
        case SIGQUIT:
            signal_name = "SIGQUIT";
            break;
        case SIGTSTP:
            signal_name = "SIGTSTP";
            break;
        case SIGCHLD:
          signal_name = "SIGCHLD";
            break;
        default:
            fprintf(stderr, "Caught wrong signal: %d\n", signal);
            
      printf("ignoring  %s signal \n", signal_name);
     }
}

void debugPrint(int pid,char * exeCmd){
      char   buf[100];
     sprintf(buf, "Procces ID: %d\nExecuting Command: %s\n\n", pid, exeCmd);
     write(2, buf, strlen(buf));
   }

   
void reDirectInputAlone(int file,cmdLine *pCmdLine,int fileCopy){
      close(0);
      
      if(file=open(pCmdLine->inputRedirect,O_RDONLY)<0){
          perror("opening file failed\n");
          dup2(fileCopy, 0);
          close(fileCopy);
          exit(EXIT_FAILURE); 
        }
      if(execvp (pCmdLine->arguments[0], pCmdLine->arguments) < 0){     /* execute the command  */
         perror("** execv failed\n");
         close(file);
         dup2(fileCopy, 0);
         close(fileCopy);
         exit(EXIT_FAILURE); 
      }
      close(file);
      dup2(fileCopy, 0);
      close(fileCopy);

  
}
void reDirectOutputAlone(int file,cmdLine *pCmdLine,int fileCopy){
 
      close(1);
      
      if(file=open(pCmdLine->outputRedirect,O_WRONLY )<0){
          perror("opening file failed\n");
          dup2(fileCopy, 1);
          close(fileCopy);
          exit(EXIT_FAILURE); 
        }
      if(execvp (pCmdLine->arguments[0], pCmdLine->arguments) < 0){     /* execute the command  */
         perror("** execv failed\n");
         close(file);
         dup2(fileCopy, 1);
         close(fileCopy);
         exit(EXIT_FAILURE); 
      }
      close(file);
      dup2(fileCopy, 1);
      close(fileCopy);
  
}
void reDirectOutputAndInputTogether(int file1,int file2,cmdLine *pCmdLine,int fileCopy1,int fileCopy2){
        close(0);
        close(1);
      if(file1=open(pCmdLine->inputRedirect,O_RDONLY)<0){
        perror("opening file failed\n");
        dup2(fileCopy1, 0);
        close(fileCopy1);
        dup2(fileCopy2, 1);
        close(fileCopy2);
        exit(EXIT_FAILURE); 
      }
      if(file2=open(pCmdLine->outputRedirect,O_WRONLY )<0){
        perror("opening file failed\n");
        dup2(fileCopy1, 0);
        close(fileCopy1);
        dup2(fileCopy2, 1);
        close(fileCopy2);
        exit(EXIT_FAILURE); 
       }
       if(execvp (pCmdLine->arguments[0], pCmdLine->arguments) < 0){     /* execute the command  */
        perror("** execv failed\n");
        close(file1);
        close(file2);
        dup2(fileCopy1, 0);
        close(fileCopy1);
        dup2(fileCopy2, 1);
        close(fileCopy2);
        exit(EXIT_FAILURE); 
      }
        close(file1);
        close(file2);
        dup2(fileCopy1, 0);
        close(fileCopy1);
        dup2(fileCopy2, 1);
        close(fileCopy2);
        exit(EXIT_FAILURE); 

    
}
