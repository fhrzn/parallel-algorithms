#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>

using namespace std;

// The function of allocating memory for a two-dimensional array
double** malloc_array(long int n)
{
	double** matrix = new double*[n];
	for (int i = 0; i < n; i++)
		matrix[i] = new double[n];
	return matrix;
}

// Memory free function
void free_array(double** matrix,long int n)
{
	for (int i = 0; i < n; i++)
		delete[] matrix[i];
	delete[] matrix;
}

// The function of initializing a matrix with random numbers from [0,1]
void rand_init_matrix(double** matrix,long int n)
{
	srand(time(NULL));
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			matrix[i][j] = rand() / RAND_MAX;
}

// Matrix zeroing function
void zero_init_matrix(double** matrix, long int n)
{
	srand(time(NULL));
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			matrix[i][j] = 0.0 ;
}

double* non_parallel(double** A, double** B, double** C, int N) {

    clock_t t;

    // Matrix multiplication with cycle order ijk
	double time = clock();
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			for (int k = 0; k < N; k++)
				C[i][j] += A[i][k] * B[k][j];
	double ijk = (clock() - time) / CLOCKS_PER_SEC;	

    // Matrix multiplication with cycle order jki
	zero_init_matrix(C, N);
	time = clock();
	for (int j = 0; j < N; j++)
		for (int k = 0; k < N; k++)
			for (int i = 0; i < N; i++)
				C[i][j] += A[i][k] * B[k][j];
	double jki = (clock() - time) / CLOCKS_PER_SEC;

    // Matrix multiplication with cycle order ikj
	zero_init_matrix(C, N);
	time = clock();
	for (int i = 0; i < N; i++)
		for (int k = 0; k < N; k++)
			for (int j = 0; j < N; j++)
				C[i][j] += A[i][k] * B[k][j];
	double ikj = (clock() - time) / CLOCKS_PER_SEC;

    static double times[] = {ijk, jki, ikj};
    return times;
}

double parallel(double** A, double** B, double** C, int N, int numThread, string mode) {
    
    double time, t;
    
    if (mode == "ijk")
    {
        // Matrix multiplication with cycle order ijk
        time = omp_get_wtime();
        #pragma omp parallel num_threads(numThread)
        {
            #pragma omp for
            {            
                for (int i = 0; i < N; i++)
                    for (int j = 0; j < N; j++)
                        for (int k = 0; k < N; k++)
                            C[i][j] += A[i][k] * B[k][j];
            }
            #pragma omp single
            {
                printf("Total thread(s) ijk: %d\n", omp_get_num_threads());
            }
        }
        t = omp_get_wtime() - time;
    }
    else if (mode == "jki")
    {
        // Matrix multiplication with cycle order jki
        time = omp_get_wtime();              
        #pragma omp parallel num_threads(numThread)
        {
            #pragma omp for
            {            
                for (int j = 0; j < N; j++)
                    for (int k = 0; k < N; k++)
                        for (int i = 0; i < N; i++)
                            C[i][j] += A[i][k] * B[k][j];
            }
            #pragma omp single
            {
                printf("Total thread(s) jki: %d\n", omp_get_num_threads());
            }        
        }
        t = omp_get_wtime() - time;
    }
    else if (mode == "ikj")
    {
        // Matrix multiplication with cycle order ikj
        time = omp_get_wtime();
        zero_init_matrix(C, N);            
        #pragma omp parallel num_threads(numThread)
        {
            #pragma omp for
            {            
                for (int i = 0; i < N; i++)
                    for (int k = 0; k < N; k++)
                        for (int j = 0; j < N; j++)
                            C[i][j] += A[i][k] * B[k][j];
            }
            #pragma omp single
            {
                printf("Total thread(s): %d\n", omp_get_num_threads());
            }        
        }
        t = omp_get_wtime() - time;        
    }
    
    return t;   
}

int main(int argc, char* argv[]) {

    const long int N = atoi(argv[1]);
    double** A, **B, **C;

    // Memory allocation for matrices A, B, C
	A = malloc_array(N);
	B = malloc_array(N);
	C = malloc_array(N);

    // Initialization of matrices
	rand_init_matrix(A, N);
	rand_init_matrix(B, N);
	zero_init_matrix(C, N);
	clock_t t;

    // without OMP
    double* non_omp = non_parallel(A, B, C, N);
    printf("Time execution single thread: [%f, %f, %f]\n", non_omp[0], non_omp[1], non_omp[2]);
    zero_init_matrix(C, N);    

    // ijk with OMP
    for (int i = 1; i <= 10; i++)
    {
        zero_init_matrix(C, N);
        double omp = parallel(A, B, C, N, i, "ijk");
        double efficiency = non_omp[0] / omp;
        
        printf("Time execution: %f\n", omp);
        printf("Efficiency: %f\n", efficiency);
    }

    zero_init_matrix(C, N);
    printf("--------------\n");

    // jki with OMP
    for (int i = 1; i <= 10; i++)
    {
        zero_init_matrix(C, N);
        double omp = parallel(A, B, C, N, i, "jki");
        double efficiency = non_omp[1] / omp;
        
        printf("Time execution: %f\n", omp);
        printf("Efficiency: %f\n", efficiency);
    }

    zero_init_matrix(C, N);
    printf("--------------\n");

    // ikj with OMP
    for (int i = 1; i <= 10; i++)
    {
        double omp = parallel(A, B, C, N, i, "ikj");
        double efficiency = non_omp[2] / omp;
        
        printf("Time execution: %f\n", omp);
        printf("Efficiency: %f\n", efficiency);
    }
    
    // Freeing memory occupied by matrices A, B, C
	free_array(A, N);
	free_array(B, N);
	free_array(C, N);

    return 0;

}