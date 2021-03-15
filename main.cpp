#include <iostream>
#include <mpi.h>

using namespace std;

int main()
{
    int rank, sum = 0, N = 3;
    int a[N], B[N][N], D[N][N], c[N]; 

    MPI_Comm comm;
    comm  = MPI_COMM_WORLD;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(comm, &rank);

     // Step 2: The root process (here rank = 0) generates the matrix B
    if(rank==0) {
        for(int i=0; i<N; i++) {
            for(int j=0; j<N; j++) {
                B[i][j] = (j + i + 2)*(N - j);
            }
        }
    }
    // The matrix B is broadcast to all other processes
    MPI_Bcast(B, N*N, MPI_INT, 0, comm);

    // Step 3: each process creates their corresponding row of matrix A
    for(int col=0; col<N; col++) {
        a[col] = (N - col + rank + 1) * (rank+1);
    } 

    // Step 4: each process multiplies their row of A by the entire matrix B 
    // (inner product) to get a row of matrix D corresponding to row = rank
    for (int i = 0; i < N; i++)  {
            for (int col = 0; col < N; col++) {
                sum = sum + a[col] * B[col][i];                
            }
        c[i] = sum;
        sum = 0;
    }
    
    // Step 5: the root process (0) gathers the 'c' vectors of all processes and
    // puts them into matrix D in the order of the processes' ranks
    MPI_Gather(c, N, MPI_INT, D, N, MPI_INT, 0, comm);

    // Print matrix D for the user
   if(rank==0) {
       cout << "Matrix D: " << endl;
       for(int i=0; i<N; i++) {
           for(int j=0; j<N; j++) {
               cout << D[i][j] << "   ";
           }
       cout << endl;
       }
   }

    MPI_Finalize();

}                      
