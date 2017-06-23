int main (int argc, const char * argv[]) {
    int fd[2];
    pipe(fd); 

    // Create one child process for more
    int pid = fork();
    if (pid == 0) {
        close(fd[1]);
        int ret = dup2(fd[0],0);
        if (ret < 0) perror("dup2");
        char *argv[3];
        argv[0] = "tail";  argv[1] = "-n";
        argv[2]="2";
        execvp("tail", argv);
    } 
    // Create another child process for ls
    int pid2 = fork();
    if (pid2 == 0) {
        int ret = dup2(fd[1],1);
        if (ret < 0) perror("dup2");
        char *argv[2];
        argv[0] = "ls";    argv[1] = "-l";   
        
        execvp("ls", argv);
    }
    close(fd[0]);
    close(fd[1]);
    // wait for the more process to finish
    int status;
    waitpid(pid, &status, 0);

    printf("Done!\n");
    return 0;
}