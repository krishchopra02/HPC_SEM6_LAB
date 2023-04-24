#include <pthread.h>
#include <stdio.h>
#include <time.h>
pthread_t id1, id[10];
int num, i, j, k;
int th, b[3][3], a[3][3];
int res[3][3];
void *mul(void *p)
{
    int from, to;
    num = *((int *)(&p));
    from = (num * 3) / th;
    to = (num + 1) * 3 / th;
    for (i = from; i < to; i++)
    {
        for (j = 0; j < 3; j++)
        {
            res[i][j] = 0;
            for (k = 0; k < 3; k++)
            {
                res[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}
int main()
{
    printf("\nEnter the 3 * 3 matrix A");
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            scanf("%d", &a[i][j]);
        }
    }
    printf("\nEnter the 3 * 3 matrix B");
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            scanf("%d", &b[i][j]);
        }
    }
    printf("\nEnter the no of threads ");
    scanf("%d", &th);
    struct timespec begin, end;
    double elapsed;
    clock_gettime(CLOCK_MONOTONIC, &begin);
    for (i = 0; i < th; i++)
    {
        pthread_create(&id[i], NULL, mul, (void *)i);
    }
    for (i = 0; i < th; i++)
    {
        pthread_join(id[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    printf("total time taken : %f", elapsed);
    printf("\n");
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            printf("%d ", res[i][j]);
        }
        printf("\n");
    }
}