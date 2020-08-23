#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFSIZE 1024

/*
 * struct stat {
 * int dev;     // File system's disk device
 * uint ino;    // Inode number
 * short type;  // Type of file -- are you a directory?
 * short nlink; // Number of links to file
 * uint64 size; // Size of file in bytes
 * };
 * 
 * struct dirent {
 * ushort inum;
 * char name[DIRSIZ];   
 * };
 */

int find(char *path, char *name);

int
main(int argc, char *argv[])
{
    if(argc != 3){
        printf("usage: find dirname keyword\n");
        exit(1);
    }

    if(find(argv[1], argv[2]) != 0){
        printf("something went wrong\n");
        exit(1);
    }

    exit(0);
}

int
find(char *path, char *name)
{
    char nextpath[512], *manip;
    int fd, i;
    struct stat st;
    struct dirent dr;

    if((fd = open(path, O_RDONLY)) < 0){
        printf("file does not do the exist\n");
        return 1;
    }

    if(fstat(fd, &st) < 0){
        printf("could not stat ur boyo :(\n");
        goto error;
    }

    switch(st.type){
    case T_FILE:
        manip = path;
        for(i = 0; i < strlen(path); i++){
            if(path[i] == '/') manip = path + i + 1;
        }
        if(strcmp(manip, name) == 0) printf("%s\n", path);
        break;
    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > BUFSIZE){
            printf("Path is too long for my sanity\n");
            goto error;
        }
        strcpy(nextpath, path);
        manip = nextpath + strlen(nextpath);
        *manip++ = '/';
        while(read(fd, &dr, sizeof(dr)) == sizeof(dr)){
            if(dr.inum == 0) continue;

            memmove(manip, dr.name, DIRSIZ);
            manip[DIRSIZ] = '\0';
            if(stat(nextpath, &st) == 0 && *manip != '.'){
                find(nextpath, name);
            }
        }
        break;
    }

    close(fd);
    return 0;

error:
    close(fd);
    return 1;
}