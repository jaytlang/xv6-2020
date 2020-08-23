#include "kernel/types.h"
#include "user/user.h"


int
main()
{
    int fd[2], pid;
    int sendbit = 1;

    if(pipe(fd) < 0){
        printf("we did a bad\n");
        exit(1);
    } 

    pid = fork();
    if(pid > 0){
        // we are the parent
        if(write(fd[1], &sendbit, 1) != 1){
            printf("parent did a die\n");
            exit(1);
        }

        if(read(fd[0], &sendbit, 1) != 1){
            printf("i am illiterate\n");
            exit(1);
        }

        printf("%d: received pong\n", getpid());
    }else{
        // we are the child
        if(read(fd[0], &sendbit, 1) != 1){
            printf("i am illiterate\n");
            exit(1);
        }

        printf("%d: received ping\n", getpid());
        sleep(1);

        if(write(fd[1], &sendbit, 1) != 1){
            printf("parent did a die\n");
            exit(1);
        }

    }

    exit(0);

}