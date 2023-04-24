#include <mpi.h>
#include <stdio.h>

#define MAX_ARR_SIZE 100

int compute_arr_sum(int *arr, int size)
{
    int sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += arr[i];
    }
    return sum;
}
int main(int argc, char *argv[])
{   
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
    {
        int n;
        printf("Enter size of the array");
        scanf("%d:", &n);
        int arr[n];
        printf("\nEnter the elements: \n");
        for (int i = 0; i < n; i++)
        {

            scanf("%d", &arr[i]);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        int global_sum = 0, local_sum = 0, recv_sum;
        // if array size is divsible by size of processors

        int elements_per_process = n / size;
        int sub_arr[elements_per_process];
        for (int i = 1; i < size; i++)
        {
            if (n % size != 0 && i == size - 1)
            {
                int size_last = n - (size - 1) * elements_per_process;
                for (int j = 0; j < size_last; j++)
                {
                    sub_arr[j] = arr[i * elements_per_process + j];
                }
                MPI_Send(sub_arr, size_last, MPI_INT, i, 1, MPI_COMM_WORLD);
                MPI_Send(&size_last, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            }
            else
            {
                for (int j = 0; j < elements_per_process; j++)
                {
                    sub_arr[j] = arr[i * elements_per_process + j];
                }
                MPI_Send(sub_arr, elements_per_process, MPI_INT, i, 1, MPI_COMM_WORLD);
                MPI_Send(&elements_per_process, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            }
        }
        local_sum = compute_arr_sum(arr,elements_per_process);
        printf("Rank %d: local sum: %d\n", rank, local_sum);
        global_sum += local_sum;

        // Get Global Sum
        for (int i = 1; i < size; i++)
        {
            MPI_Recv(&recv_sum, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_sum += recv_sum;
        }
        printf("Sum of the array is %d\n", global_sum);
    }
    else
    {
        MPI_Barrier(MPI_COMM_WORLD);
        int recv_array_size, local_sum = 0;
        int recv_sub_arr[MAX_ARR_SIZE];

        MPI_Recv(recv_sub_arr, recv_array_size, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&recv_array_size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        local_sum = compute_arr_sum(recv_sub_arr,recv_array_size);
        printf("Rank %d: local sum: %d\n", rank, local_sum);
        MPI_Send(&local_sum, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}