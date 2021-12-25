#include <stdio.h>
#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    int rank, size;
    MPI_Status status;
    double start;

    int n = 1000000;

    int* arr = new int[n];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        for (int i = 0; i < n; i++)
        {
            arr[i] = 1;
        }        

        start = MPI_Wtime();
    }

    // init local array
    int* localArr = new int[n/size];

    // spread the array
    MPI_Scatter(arr, n/size, MPI_INT, localArr, n/size, MPI_INT, 0, MPI_COMM_WORLD);

    int sum = 0;
    for (int i = 0; i < n/size; i++)
    {
        sum += localArr[i];
    }

    int globalSum = 0;
    MPI_Reduce(&sum, &globalSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);    
    
    if (rank == 0)
    {
        printf("Time elapsed = %f\n", (MPI_Wtime() - start));
        printf("Total sum = %d\n", globalSum);
    }

    MPI_Finalize();
    return 0;
        
}