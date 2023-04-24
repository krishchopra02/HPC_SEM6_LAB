#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>

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
    int size;
    printf("Enter the size of the array: ");
    scanf("%d", &size);
    int *arr = create_random_array(size);

    // Print array
    printf("\nThe array before sorting is: \n");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
    // Odd phase or even phase
    time_t start = time(NULL);
    int phase, i, temp = 0;
#pragma omp parallel num_threads(omp_get_thread_num()) \ 
default(none) shared(arr, size) private(i, phase, temp)
    for (phase = 0; phase < size; phase++)
    {
        if (phase % 2 == 0)
        { // Even phase
#pragma omp for
            for (i = 1; i < size; i += 2)
            {
                if (arr[i - 1] > arr[i])
                {
                    temp = arr[i];
                    arr[i] = arr[i - 1];
                    arr[i - 1] = temp;
                }
            }
        }
        else
        {
// Odd phase
#pragma omp for
            for (i = 1; i < size - 1; i += 2)
            {
                if (arr[i + 1] < arr[i])
                {
                    temp = arr[i];
                    arr[i] = arr[i + 1];
                    arr[i + 1] = temp;
                }
            }
        }
        if (omp_get_thread_num() == 0)
        {
            printf("\nArray at end of phase %d\n", phase);
            for (int i = 0; i < size; i++)
            {
                printf("%d ", arr[i]);
            }
        }
    }

    double elapse = (double)(time(NULL) - start);
    printf("\nTime taken to sort %.2f\n", elapse);

    printf("Sorted array is: \n");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }
    return 0;
}