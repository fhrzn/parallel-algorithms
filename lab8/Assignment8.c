#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    int rank, size;
    MPI_Status status;
    double T, start;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (long i = 1; i <= 1000000; i*=10)
    {        

        char* message = (char*)malloc(i * sizeof(char));        

        if (rank == 0)
        {
            start = MPI_Wtime();
        }

        for (int j = 0; j < 10; j++)
        {
            if (rank == 0)
            {
                MPI_Send(message, i, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(message, i, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);
            }
            else if (rank == 1)
            {
                MPI_Recv(message, i, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
                MPI_Send(message, i, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }            
            
        }

        if (rank == 0)
        {
            T = MPI_Wtime() - start;
            
            // Bandwidth calculation
            int L = i * sizeof(char);
            double R = 2 * 10 * L / T;

            printf("Message length: %d\n", L);
            printf("Bandwidth (bytes/s): %f\n", R);
            printf("Throughput (MB/s): %f\n\n", (R/(1024*1024)));
        }                
        
    }

    int new_msg = 0;
    double ltc_T, ltc_start;
    if (rank == 0)
    {
        ltc_start = MPI_Wtime();
    }

    for (long i = 0; i < 1000000; i++)
    {
        if (rank == 0)
        {
            MPI_Send(&new_msg, 0, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&new_msg, 0, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        }
        else
        {
            MPI_Recv(&new_msg, 0, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&new_msg, 0, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }        
    }

    if (rank == 0)
    {
        ltc_T = MPI_Wtime() - ltc_start;
        double latency = ltc_T / 2 * 1000000;
        printf("Latency: %f\n", latency);
    }
    
    
    MPI_Finalize();
    return 0;
    
}