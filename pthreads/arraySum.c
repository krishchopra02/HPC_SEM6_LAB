#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
pthread_mutex_t sum_mutex[10];
typedef struct data
{
    int *arr;
    int thread_num;
    int th;
} data;
int sz = 10;
void *halfSum(void *p)
{
    data *ptr = (data *)p;
    int n = ptr->thread_num;
    int th = ptr->th;
    int *thread_sum = (int *)calloc(1, sizeof(int));
    if (n == th - 1)
    {
        for (int i = n * (sz) / th; i < sz; i++)
        {
            thread_sum[0] = thread_sum[0] + ptr->arr[i];
        }
    }
    else
    {
        for (int i = n * (sz) / th; i < (n + 1) * sz / th && i < sz; i++)
        {
            thread_sum[0] = thread_sum[0] + ptr->arr[i];
        }
    }
    pthread_exit(thread_sum);
}
int main(void)
{
    int *int_arr = (int *)calloc(sz, sizeof(int));
    for (int i = 0; i < sz; i++)
    {
        int_arr[i] = i + 1;
        printf("%d ", int_arr[i]);
    }
    printf("\n");
    // Declare arguments for both threads:
    int n;
    printf("enter thread number ");
    scanf("%d", &n);
    printf("\n");
    data thread_data[n];
    for (int i = 0; i < n; i++)
    {
        thread_data[i].thread_num = i;
        thread_data[i].arr = int_arr;
        thread_data[i].th = n;
    }
    pthread_t tid[n];
    for (int i = 0; i < n; i++)
    {
        pthread_create(&tid[i], NULL, halfSum, &thread_data[i]);
    }
    int *sm;
    int sum = 0;
    for (int i = 0; i < n; i++)
    {
        pthread_join(tid[i], (void **)&sm);
        sum += *sm;
    }
    printf("Sum of whole array = %d\n", sum);
    return 0;
}