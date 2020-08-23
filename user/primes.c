#include "kernel/types.h"
#include "user/user.h"

#define MAXPRIME 1000

int primes(int factor, int *leftpipe);

/* State machine: every process gets a factor to try
 *
 * State 1: I am the last in the sieve
 * If I get a number through my read end I can't divide, 
 *      - make a new pipe
 *      - fork a new process with that number as the factor
 *      - print out that number to screen
 *      
 * State 2: I have found a number I can't divide...no longer last
 *      - pass it on through my next pipe
 * 
 * Lessons learned: always print BEFORE you recurse
*/

int
main()
{
    int fds[2], pid, i;

    if(pipe(fds) < 0){
        printf("pipe\n");
        exit(1);
    }

    pid = fork();
    if(pid < 0){
        printf("fork\n");
        exit(1);
    }else if(pid > 0){
        close(fds[0]);
        for(i = 2; i < MAXPRIME; i++){
            if(write(fds[1], &i, 1) != 1){
                printf("write from driver\n");
                exit(1);
            }
        }

        close(fds[1]);
    }else{
        primes(2, fds);
    }

    exit(0);
}

int
primes(int factor, int *leftpipe)
{
    int i, didfork, rightpipe[2], mypid;
    close(leftpipe[1]);
    didfork = 0;

    printf("prime %d\n", factor);
    while(read(leftpipe[0], &i, 1) == 1){
        if(i % factor == 0) continue;

        if(didfork == 1){
            if(write(rightpipe[1], &i, 1) != 1){
                printf("child write failed\n");
                exit(1);
            }
        }else{
            didfork = 1;
            if(pipe(rightpipe) < 0){
                printf("child pipe\n");
                exit(1);
            }

            mypid = fork();

            if(mypid < 0){
                printf("fork\n");
                exit(1);
            }else if(mypid != 0){
                close(rightpipe[0]);
            }else{
                primes(i, rightpipe);
            }
        }
        
    }

    close(leftpipe[0]);
    if(didfork == 1){
        close(rightpipe[1]);
    }
    return 0;
    
}