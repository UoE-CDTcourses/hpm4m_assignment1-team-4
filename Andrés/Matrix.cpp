//
// Based in the tutorial https://computing.llnl.gov/tutorials/mpi/samples/C/mpi_mm.c
//

#include <iostream>
#include <vector>
#include <mpi.h>
using namespace std;

#define N 10             // Size of matrices
#define Master 0        // Id of base task

void print_results(int a[N][N])
{
    //printf ("\n\n%s\n", prompt);
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < N; j++) {
            printf(" %d", a[i][j]);
        }
        printf ("\n");
    }
    printf ("\n\n");
}

int main(){
    int rank,           // Task identifier
        size,           // Number of tasks
        tag,            // Interaction tag
        assigned_rows,  //
        buffer,         //
        workers;        // Number of worker tasks
    
    int B[N][N];  // Matrix B
    int C[N][N];  // Result C
    
    
    /* Settings for MPI */
    MPI_Comm comm;
    MPI_Status status;
    comm = MPI_COMM_WORLD;
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    
    if (size < 2){
        cout << "There is only one task.\n";
        cout << "We need at least one worker task!";
        MPI_Abort(comm, 1);
        exit(1);
    }
    
    workers = size - 1;
    
    /* Central node work */
    if (rank == Master){
        cout << "Assembling matrix B of size " << N << "." << endl;
        
        for (unsigned int i = 0; i < N; i++){
            for (unsigned int j = 0; j < N; j++){
                B[i][j] = (i+1 + j+1) * (N - j);
            }
        }
        // Printing B if needed:
        printf("\n\nB = \n");
        print_results(B);
        
        
        cout << "MPI started with " << size << " tasks" << endl << endl;
        double start = MPI_Wtime();     // Start measuring time
        /* Send matrix to worker tasts */
        int n_rows  = N/workers;        // Rows sent to workers on average
        int n_extra = N%workers;        // Extra rows
        buffer = 0;
        
        tag = 1;
        for (int destination = 1; destination <= workers; destination++){
            // If there are extra rows, we give homogeneously to the workers
            assigned_rows = (destination <= n_extra) ? n_rows + 1: n_rows;
            
            // Send 'assigned_rows' to task 'destination'
            MPI_Send(&buffer, 1, MPI_INT, destination, tag, comm);
            MPI_Send(&assigned_rows, 1, MPI_INT, destination, tag, comm);
            MPI_Send(&B[0], N*N, MPI_INT, destination, tag, comm);
            
            buffer += assigned_rows;
        }
        
        /* Receive results from workers */
        tag = 2;
        for (int i = 1; i <= workers; i++){
            int source = i;
            
            MPI_Recv(&buffer, 1, MPI_INT, source, tag, comm, &status);
            MPI_Recv(&assigned_rows, 1, MPI_INT, source, tag, comm, &status);
            for (unsigned int l =  buffer; l < assigned_rows + buffer; l++){
                MPI_Recv(&C[l][0], N, MPI_INT, source, tag, comm, &status);
            }
        }
        
        
        // Printing C if needed:
        printf("\n\nC = \n");
        print_results(C);
        
        // Present finish time
        double finish = MPI_Wtime();
        printf("Done in %f seconds.\n", finish - start);
        
    }
    
    /* Workers assignment */
    if (rank > Master){
        tag = 1;            // From master to worker
        MPI_Recv(&buffer, 1, MPI_INT, Master, tag, comm, &status);
        MPI_Recv(&assigned_rows, 1, MPI_INT, Master, tag, comm, &status);
        MPI_Recv(&B, N*N, MPI_INT, Master, tag, comm, &status);
        
        
        // Only assign work to active workers
        if (assigned_rows > 0){
            // Build A
            vector < vector<int> > A(assigned_rows, vector<int> (N) );
            for (unsigned int i = 0; i < assigned_rows; i++){
                unsigned int ii = buffer + i + 1;
                for (unsigned int j = 0; j < N; j++){
                    A[i][j] = (N - (j+1) + ii + 1) * ii;     // correct zero index
                }
            }
            
            // Perform product
            for (unsigned int k = 0; k < N; k++){
                for (unsigned int i = buffer; i < assigned_rows + buffer; i++){
                    int ii = i - buffer;
                    C[i][k] = 0;
                    for (unsigned int j = 0; j < N; j++){
                        //cout << rank << " at (" << i+1 << "," << j+1 << "): " << A[ii][j] << endl;
                        C[i][k] += A[ii][j] * B[j][k];
                    }
                }
            }
            
            // Print each updated component if needed
            /*for (unsigned int k = 0; k < N; k++){
                for (unsigned int i = buffer; i < assigned_rows + buffer; i++){
                    cout << rank << " at (" << i+1 << "," << k+1 << "): " << C[i][k] << endl;
                    
                }
            }*/
        }
        
        
        tag = 2;
        MPI_Send(&buffer, 1, MPI_INT, Master, tag, comm);
        MPI_Send(&assigned_rows, 1, MPI_INT, Master, tag, comm);
        for (unsigned int l =  buffer; l < assigned_rows + buffer; l++){
            MPI_Send(&C[l][0], N, MPI_INT, Master, tag, comm);
        }
        //MPI_Send(&C, assigned_rows*N, MPI_INT, Master, tag, comm);
        
    }
    
    
    // Find the closest multiple of three to your rank below ...

    // Wrap up
    MPI_Finalize();
}


