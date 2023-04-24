#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#define MAX_VAL 1000
#define MIN_VAL 1

int *create_random_array(int size)
{
    int *arr = malloc(size * sizeof(int));
    srandom(time(NULL));
#pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        arr[i] = rand() % MAX_VAL + MIN_VAL;
    }
    return arr;
}

int main()
{
    int n;
    printf("Enter size of the array: ");
    scanf("%d", &n);
    int *arr = create_random_array(n);
    // Print array
    printf("\nThe array is: \n");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

    int num_of_threads;
    int sum = 0;
    printf("\nEnter the number of threads: ");
    scanf("%d", &num_of_threads);
    omp_set_num_threads(num_of_threads);
    int local_sums[num_of_threads];
#pragma omp parallel
    {
        int id, nthrds;
        id = omp_get_thread_num();
        local_sums[id] = 0;
        nthrds = omp_get_num_threads();
        for (int i = id; i < n; i = i + nthrds)
        {
            local_sums[id] += arr[i];
        }
        printf("Local sum of thread %d is %d\n", id, local_sums[id]);
        if (id == 0)
        {
            for (int i = 0; i < nthrds; i++)
            {
                sum += local_sums[i];
            }
        }
    }
    printf("Sum is %d\n", sum);
    return 0;
}