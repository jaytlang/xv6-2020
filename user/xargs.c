#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFSIZE 512

int
main(int argc, char *argv[])
{
    char inputbuf[BUFSIZE], *nargv[MAXARG], *off, *trailing;
    int pid, i;
    
    off = inputbuf;
    while(read(0, off, 1) == 1){
        if(*off != '\n'){
            if(++off > inputbuf + BUFSIZE - 1){
                printf("err: buffer overflow");
                exit(1);
            }
            continue;
        }

        // have a line of input in inputbuf.
        *off = '\0';
        printf("%s from program left of pipe\n", inputbuf);
        pid = fork();     
        if(pid < 0){
            printf("fork\n");
            exit(1);
        }
        if(pid == 0){
            for(i = 0; i < argc - 1; i++){
                nargv[i] = argv[i + 1];
            }

            trailing = inputbuf;
            for(off = inputbuf; *off != '\0'; off++){
                if(*off == ' '){
                    *off = '\0';
                    nargv[i++] = trailing;
                    if(i > MAXARG) break;
                    trailing = off + 1;
                }
            }

            nargv[i] = trailing;
            nargv[i + 1] = 0;

            if(exec(nargv[0], nargv) < 0){
                printf("exec\n");
                exit(1);
            };

            for(; i >= 0; i--){
                nargv[i] = 0;
            }
        }else{
            wait(&i);
        }

        off = inputbuf;

    }

    exit(0);
}