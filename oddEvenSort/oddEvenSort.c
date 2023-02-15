#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#define CHILD_SEM "children"
#define EVEN_SEM "even"
#define ODD_SEM "odd"
#define MAX_SIZE 100
void swap_array_in_shared_memory(key_t key, int buffer_size, int offset)
{

    // find the segment
    int shmid;
    int *shm;
    int val;
    if ((shmid = shmget(key, buffer_size, 0666)) < 0)
    {
        perror("Could not find shared memory buffer!");
        exit(1);
    }

    // Attach the shared memory to our processes space
    if ((shm = shmat(shmid, NULL, 0)) == (int *)-1)
    {
        perror("Could not attatch to shared memory");
        exit(1);
    }

    if (offset < buffer_size - 1 && *(shm + offset) > *(shm + offset + 1))
    {
        int temp = *(shm + offset);
        *(shm + offset) = *(shm + offset + 1);
        *(shm + offset + 1) = temp;
    }
    // array after this phase
    // for (int i = 0; i < buffer_size; i++)
    // {
    //     printf("%d ", *(shm + i));
    // }
    // printf("\n");

    shmdt(shm);
}

void even_phase_swaps(int key, int n, int pid)
{

    sem_t *even_phase = sem_open(EVEN_SEM, 0);
    sem_t *odd_phase = sem_open(ODD_SEM, 0);
    sem_wait(even_phase);
    for (int i = 0; i < n / 2; i++)
    {
        pid_t child_pid;
        if ((child_pid = fork()) < 0)
        {
            perror("Fork failed!");
            exit(1);
        }
        else if (child_pid != 0)

        {
            sem_t *sem = sem_open(CHILD_SEM, 0);
            // it is a child process and should not spawn new children
            sem_wait(sem);

            swap_array_in_shared_memory(key, n, i * 2);
            sem_post(sem);
            exit(0);
        }
    }
    sem_post(odd_phase);
    sem_close(even_phase);
    sem_close(odd_phase);
}

void odd_phase_swaps(int key, int n, int pid)
{

    sem_t *even_phase = sem_open(EVEN_SEM, 0);
    sem_t *odd_phase = sem_open(ODD_SEM, 0);
    sem_wait(odd_phase);
    for (int i = 0; i < n / 2; i++)
    {
        pid_t child_pid;
        if ((child_pid = fork()) < 0)
        {
            perror("Fork failed!");
            exit(1);
        }
        else if (child_pid != 0)

        {
            sem_t *sem = sem_open(CHILD_SEM, 0);
            // it is a child process and should not spawn new children
            sem_wait(sem);
            swap_array_in_shared_memory(key, n, i * 2 + 1);
            sem_post(sem);
            exit(0);
        }
    }
    sem_post(even_phase);
    sem_close(even_phase);
    sem_close(odd_phase);
}
int main()
{
    int n;
    printf("Number of elements in array: ");
    scanf("%d", &n);

    int array[MAX_SIZE];
    printf("Enter the elements of the array: \n");
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &array[i]);
    }
    pid_t pid1 = getpid();
    key_t key = ftok("hpc.txt", 1);
    int shmid;
    printf("Adding array to shared memory!!\n");
    // Creating the shared memory segment for array
    if ((shmid = shmget(key, n, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget failed!");
        exit(1);
    }
    int *shm, *temp;
    if ((shm = shmat(shmid, NULL, 0)) == (int *)-1)
    {
        perror("shmat failed!");
        exit(1);
    }

    temp = shm;
    for (int i = 0; i < n; i++)
    {
        *temp++ = array[i];
    }
    shmdt(shm);

    // Creating semaphores
    sem_t *children = sem_open(CHILD_SEM, O_CREAT | O_EXCL, 0644, n / 2);
    sem_t *even_phase = sem_open(EVEN_SEM, O_CREAT | O_EXCL, 0644, 1);
    sem_t *odd_phase = sem_open(ODD_SEM, O_CREAT | O_EXCL, 0644, 0);
    for (int i = 0; i < n / 2 + 1; i++)
    {
        pid1 = fork();
        if (pid1 == 0)
        {
            even_phase_swaps(key, n, getpid());
            odd_phase_swaps(key, n, getpid());
            exit(0);
        }
    }

    if (pid1 != 0)
    {
        // Wait for children to exit
        while (pid1 = waitpid(-1, NULL, 0))
        {
            if (errno == ECHILD)
                break;
        }
        sem_unlink(CHILD_SEM);
        sem_unlink(EVEN_SEM);
        sem_unlink(ODD_SEM);
        sem_close(children);
        sem_close(even_phase);
        sem_close(odd_phase);
        if ((shm = shmat(shmid, NULL, 0)) == (int *)-1)
        {
            perror("shmat failed!");
            exit(1);
        }
        printf("Array after sorting: \n");
        for (int i = 0; i < n; i++)
        {

            printf("%d ", *(shm + i));
        }
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
    }
}
