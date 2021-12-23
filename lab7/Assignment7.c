#include <stdio.h>
#include <mpi.h>
using namespace std;

double* initVector(int size) {
    double* vector = new double[size];
    for (long i = 0; i < size; i++)
    {
        vector[i] = 1;
    }

    return vector;
    
}

int main(int argc, char* argv[])
{
    int rank, size;
    double* vecA, * vecB;
    MPI_Status status;

    int n = 1000000;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        vecA = initVector(n);
        vecB = initVector(n);
    }

    // initialize  local vector for each thread
    double* localVecA = new double[n/size];
    double* localVecB = new double[n/size];
    
    // spreading the vector from process-0
    MPI_Scatter(vecA, n/size, MPI_DOUBLE, localVecA, n/size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(vecB, n/size, MPI_DOUBLE, localVecB, n/size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // dot product
    double prod = 0;
    for (long i = 0; i < n/size; i++)
    {
        prod += localVecA[i] * localVecB[i];
    }

    // init vector for gathering prod value
    double* gatherVec;
    if (rank == 0) {
        gatherVec = new double[size];
    }
    
    // gather back to process-0
    MPI_Gather(&prod, 1, MPI_DOUBLE, gatherVec, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double finalProd = 0;
        for (int i = 0; i < size; i++) {
            finalProd += gatherVec[i];
        }

        printf("Dot product = %f\n", finalProd);
        
    }
    
    MPI_Finalize();

}