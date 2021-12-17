#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <stdio.h>
using namespace std;

int main(int argc, char* argv[]) {

    int nsize = atoi(argv[1]);    // vector size
    double* arr = new double[nsize];

    for (long i = 0; i < nsize; i++)
    {
        arr[i] = (double)rand() / RAND_MAX;
    }
    
    double maxVal = arr[0];

    for (int j = 1; j <= 10; j++)
    {        
        double time = omp_get_wtime();
        #pragma omp parallel num_threads(j)
        {
            #pragma omp parallel for reduction(max: maxVal)
            {
                for (int i = 0; i < nsize; i++)
                {        
                    if (arr[i] > maxVal)
                    {
                        maxVal = arr[i];
                    }
                    
                }  
            }
        }
        printf("Total thread(s): %d\n", omp_get_num_threads());
        printf("Max number: %f\n", maxVal);
        printf("Time execution: %f\n", (omp_get_wtime() - time) / 100);
    }
    delete[] arr;
    return 0;
    
}