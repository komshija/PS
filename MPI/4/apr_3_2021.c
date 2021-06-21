#include <stdio.h>
#include <mpi/mpi.h>

#define N 8
#define q 4

int main(int argc, char **argv)
{
    struct
    {
        int max;
        int rank;
    } maxloc, maxglob;
    int size, rank;
    MPI_Datatype vectype, mattype;
    int A[N][N], B[N], C[N];
    int a[q][q], b[q], temp[N];
    int k = N / q;
    int row = rank / q;
    int col = rank % q;
    MPI_Comm rowCom, colCom;
    int maxloc;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Type_vector(k, 1, q, MPI_INT, &mattype);
    MPI_Type_create_resized(mattype, 0, sizeof(int) * N, &mattype);
    MPI_Type_vector(k, 1, q, mattype, &mattype);
    MPI_Type_commit(&mattype);

    MPI_Type_vector(k, 1, q, MPI_INT, &vectype);
    MPI_Type_create_resized(vectype, 0, sizeof(int), &vectype);
    MPI_Type_commit(&vectype);

    MPI_Comm_split(MPI_COMM_WORLD, row, col, &rowCom);
    MPI_Comm_split(MPI_COMM_WORLD, col, row, &colCom);

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                A[i][j] = i * N + j;
            B[i] = i;
        }

        for (int i = 0; i < size; i++)
        {
            if (rank == i)
            {
                for (int i = 0; i < k; i++)
                {

                    for (int j = 0; j < k; j++)
                        a[i][j] = A[i * q][j * q];
                    b[i] = B[i * q];
                }
            }
            else
                MPI_Send(&A[i / q][i % q], 1, mattype, i, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Recv(&a[0][0], k * k, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Scatter(&B[0], 1, vectype, &b[0], k, MPI_INT, 0, colCom);
    MPI_Bcast(&b[0], k, MPI_INT, 0, rowCom);

    max = a[0][0];
    for (int i = 0; i < k; i++)
        for (int j = 0; j < k; j++)
            if (max < a[i][j])
                max = a[i][j];

    maxloc.max = max;
    maxloc.rank = rank;
    MPI_Reduce(&maxloc, &maxglob, 1, MPI_2INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);
    MPI_Bcast(&maxglob, 1, MPI_2INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
        printf("Max glob %d\n", maxglob.rank);

    for (int i = 0; i < N; i++)
        temp[i] = 0;

    for (int i = 0; i < k; i++)
    {
        int index = row + i * q;
        for (int j = 0; j < k; j++)
            temp[index] += a[i][j] * b[j];
    }
    MPI_Reduce(&temp[0], &C[0], N, MPI_INT, MPI_SUM, maxglob.rank, MPI_COMM_WORLD);

    if (rank == maxglob.rank)
    {
        for (int i = 0; i < N; i++)
            printf("%d ", C[i]);
        printf("\n");
    }

    MPI_Finalize();
}