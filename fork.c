#include <sys/types.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *argv[]) {

   /* fork a child process */
   pid_t pid = fork();

   if (pid < 0) { /* error occurred */
   	fprintf(stderr, "Fork Failed");
   	return 1;
   }

   else if (pid == 0) { /* child process */
   	printf("I'm the child \n"); /* you can execute some commands here */
   }

   else { /* parent process */
   	/* parent will wait for the child to complete */
   	  wait(NULL);
   	/* When the child is ended, then the parent will continue to execute its code */
   	  printf("Child Complete \n");
   }
}