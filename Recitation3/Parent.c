#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int n = argc-1;
    int child_id;
    pid_t forkrank;
    for (int i = 0; i < n; i++){
        forkrank = fork();
        //child process
        if (forkrank==0){
            child_id = i+1;
            execl("./child", argv[i+1], NULL);
        }
        //parent process
        else if (forkrank > 0){
            printf("Parent procedure is in wait mode \n");
            wait(NULL);
        }
    }
    printf("All the child process are terminated, parent process exiting. \n");
    return 0; 
}