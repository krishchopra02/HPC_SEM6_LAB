#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdio.h>
#include<stdlib.h>

#define SHMSZ 27
int main() {
    int shmid;
    key_t key;
    char *shm, *s;
    key = 1234;
    // Find the segment
    if ((shmid=shmget(key,SHMSZ,0666))<0) {
        perror("shmget");
        exit(1);
    }
    // Attach segment
    if((shm = shmat(shmid,NULL,0)) == (char *)-1) {
        perror("shmat");
        exit(1);
    }
    for(s=shm; *s!=NULL;s++) {
        putchar(*s);
    }
    putchar('\n');
    *shm = 'X';
    exit(0);
}