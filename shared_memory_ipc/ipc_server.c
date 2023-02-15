#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdio.h>
#include<stdlib.h> 

#define SHMSZ 27
int main() {
    char c;
    int shmid;
    key_t key;
    char *shm, *s;
    key = 1234;
    // Create segment
    if((shmid = shmget(key,SHMSZ,IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    // Now we attach the segment to our data space
    if((shm = shmat(shmid,NULL,0))== (char*)-1) {
        perror("shmat");
        exit(1);
    }
    // Put something into memory
    s = shm;
    for (c='a';c<='z';c++) {
        *s++ = c;
    }
    *s = NULL;
    while(*shm !='X') sleep(1);
    exit(0);
}