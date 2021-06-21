#include <stdio.h>
#include <mpi/mpi.h>
#include <stdlib.h>

#define N 5

int main(int argc, char *argv[])
{
    int A[N][N], B[N][N], temp[N][N], C[N][N];
    int kolona[N], red[N];
    int size, rank, com_rank;
    int excl[1] = {0};
    MPI_Datatype col;
    MPI_Group gr;
    MPI_Comm com;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Type_vector(N, 1, N, MPI_INT, &col);
    MPI_Type_commit(&col);

    MPI_Comm_group(MPI_COMM_WORLD, &gr);
    MPI_Group_excl(gr, 1, excl, &gr);
    MPI_Comm_create(MPI_COMM_WORLD, gr, &com);
    MPI_Group_rank(gr, &com_rank);

    if (rank == 0)
    {
        srand(0);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                A[i][j] = rand() % 10;
                B[i][j] = rand() % 10;
            }
        }

printf("::A::\n");
        for(int i = 0 ; i < N;i++) {
            for(int j = 0 ; j < N ;j++) {
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
printf("::B::\n");
        for(int i = 0 ; i < N;i++) {
            for(int j = 0 ; j < N ;j++) {
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }


        for (int i = 0; i < N; i++)
        {
            MPI_Send(&A[0][i], 1, col, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(&B[i][0], N, MPI_INT, i + 1, 0, MPI_COMM_WORLD);      
        }

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                C[i][j] =0;
                for(int k = 0 ; k < N;k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        printf("::KLASICNO MNOZENJE::\n");
        for(int i = 0 ; i < N;i++) {
            for(int j = 0 ; j < N ;j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }
    else
    {
        MPI_Recv(&kolona[0],N,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(&red[0],N,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        for(int i = 0 ; i < N ;i++) {
            for(int j = 0 ; j < N;j++) {
                temp[i][j] = kolona[i] * red[j];
            }
        }
        MPI_Reduce(&temp,&C,N*N,MPI_INT,MPI_SUM,0,com);


        if(com_rank == 0) {
            printf("::GRUPNO MNOZENJE::\n");
            for(int i = 0 ; i < N;i++) {
                for(int j = 0 ; j < N ;j++) {
                    printf("%d ", C[i][j]);
                }
                printf("\n");
            }
        }

    }

    MPI_Finalize();
}
