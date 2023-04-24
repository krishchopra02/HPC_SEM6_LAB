#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>

#define MAX_VAL 100
#define MIN_VAL 1
int A[10][10], B[10][10];
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
int main(int argc, char *argv[])
{

    int id, processes;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);

    if (id == 0)
    {
        int size;
        printf("Enter the size of the matrix: ");
        scanf("%d", &size);
        // Create matrices
        int **K = create_rand_matrix(size);
        int **M = create_rand_matrix(size);
        printf("Matrix A is: \n");
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                A[i][j] = K[i][j];
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        printf("\nMatrix B is: \n");
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                B[i][j] = M[i][j];
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

        MPI_Barrier(MPI_COMM_WORLD);

        int row_product[size];
        int num_rows = size / processes;

        if (size % processes == 0)
        {
            for (int i = 1; i < processes; i++)
            {
                // Start index
                MPI_Send(&size, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                int start = i * num_rows;
                MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&A, size * size, MPI_INT, i, 2, MPI_COMM_WORLD);
                MPI_Send(&B, size * size, MPI_INT, i, 3, MPI_COMM_WORLD);
            }
        }
        else
        {
            int offset = size % processes;
            for (int i = 1; i < processes; i++)
            {
                MPI_Send(&size, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                int start = offset + i * num_rows;
                MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&A, size * size, MPI_INT, i, 2, MPI_COMM_WORLD);
                MPI_Send(&B, size * size, MPI_INT, i, 3, MPI_COMM_WORLD);
            }
        }

        // Performing row multiplications
        for (int i = 0; i < num_rows + size % processes; i++)
        {
            for (int j = 0; j < size; j++)
            {
                for (int k = 0; k < size; k++)
                {
                    C[i][j] = C[i][j] + A[i][k] * B[k][j];
                }
            }
        }

        // Recieve products from other processes

        for (int i = 1; i < processes; i++)
        {
            for (int j = 0; j < num_rows; j++)
            {
                MPI_Recv(&row_product, size, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int k = 0; k < size; k++)
                {
                    C[i * num_rows + size % processes + j][k] = row_product[k];
                }
            }
        }

        printf("\n\nFinal matrix is: \n");
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }
    else
    {
        MPI_Barrier(MPI_COMM_WORLD);

        int size;
        MPI_Recv(&size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int start_index;
        MPI_Recv(&start_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int A[size][size];
        int B[size][size];

        MPI_Recv(&A, size * size, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&B, size * size, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int num_rows = size / processes;
        int C[num_rows][size];
        double start_time = MPI_Wtime();
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < size; j++)
            {
                C[i][j] = 0;
                for (int k = 0; k < size; k++)
                {

                    C[i][j] = C[i][j] + A[start_index + i][k] * B[k][j];
                }
            }
        }
        double end_time = MPI_Wtime();
        // Local products
        printf("\n\n The Local matrix product by process %d is\n", id);
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < size; j++)
            {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }

        printf("\nTime elapsed by process %d is %.3f", id, end_time - start_time);
        for (int i = 0; i < num_rows; i++)
        {
            MPI_Send(&C[i], size, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
}