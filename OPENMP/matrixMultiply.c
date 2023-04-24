#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define MAX_VAL 100
#define MIN_VAL 1
int **create_rand_matrix(int size)
{
    int **matrix = malloc(size * sizeof(int *));

    for (int i = 0; i < size; i++)
    {
        matrix[i] = malloc(size * sizeof(int));
    }
    srandom(time(0));
#pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = rand() % MAX_VAL + MIN_VAL;
        }
    }
    return matrix;
}

int main()
{
    int size;
    printf("Enter the size of the matrix: ");
    scanf("%d", &size);
    // Create matrices
    int **A = create_rand_matrix(size);
    int **B = create_rand_matrix(size);
    printf("Matrix A is: \n");
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    printf("\nMatrix B is: \n");
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }

    // Creating array C to store result
    int C[size][size];
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            C[i][j] = 0;
        }
    }
    time_t start = time(NULL);
    // Parrallel multiplication
#pragma omp parallel for
    for (int i = 0; i < size; i++)
    {   
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < size; k++)
            {
                printf("ID %d is working on i=%d j=%d k=%d\n", omp_get_thread_num(), i, j, k);
                C[i][j] = C[i][j] + A[i][k] * B[k][j];
            }
        }
    }
    double elapse = (double)(time(NULL)-start);
    // Print result
    printf("Result of matrix multiplication is :\n");
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
    printf("Time taken %.2f\n",elapse);
    return 0;
}