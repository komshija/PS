#include <stdio.h>
#include <mpi/mpi.h>
#include <stdlib.h>

#define N 5

int main(int argc,char* argv[])
{
    int A[N][N],vec[4];
    int size,rank;
    MPI_Datatype tempdt, dt;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    MPI_Type_vector(2,1,2,MPI_INT,&tempdt);
    MPI_Type_create_resized(tempdt,0,N*sizeof(int),&tempdt);
    MPI_Type_commit(&tempdt);

    MPI_Type_vector(2,1,1,tempdt,&dt);
    MPI_Type_commit(&dt);

    if(rank == 0) {
        int k =0;
        for (int i = 0; i < N ; i++)
        {
             for (int j = 0; j < N; j++)
            {
                A[i][j] = k++;
                printf("%d ", A[i][j]);
            }   
            printf("\n");
        }   
        MPI_Send(&A[2][2],1,dt,1,0,MPI_COMM_WORLD);

    }
    else {
        MPI_Recv(vec,4,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        for (int i = 0; i < 4 ; i++)
            printf("vec[%d] = %d\n",i, vec[i]);
    }


    MPI_Finalize();

}