// matrica A i B
// podeliti ih po procesima iz master procesa k vrste k kolone
// upisati u fajl A.dat matricu A, u fajl B.dat matricu B
// pomnoziti matrice u matricu C
// upisati C u fajl C.dat

#include <stdio.h>
#include <mpi/mpi.h>
#define N 4
#define k 2 // racunamo da je k * k = size

int main(int argc, char *argv[])
{
    int A[N][N], B[N][N];
    int a[N / k][N], b[N][N / k], c[N / k][N / k];

    int rank, size;
    MPI_Datatype vrste, kolone,colFileview, fileview;
    MPI_File fh;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Type_vector(N / k, N, N, MPI_INT, &vrste);
    MPI_Type_create_resized(vrste, 0, sizeof(int) * N * N / k, &vrste);
    MPI_Type_commit(&vrste);

    MPI_Type_vector(N, k, N, MPI_INT, &kolone);
    MPI_Type_create_resized(kolone, 0, sizeof(int) * k, &kolone);
    MPI_Type_commit(&kolone);

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                A[i][j] = i * j;
                B[i][j] = i + j;
            }

        printf("A\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                printf("%d ", A[i][j]);
            printf("\n");
        }
        printf("\n");
        printf("B\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
                printf("%d ", B[i][j]);
            printf("\n");
        }
        printf("\n");

        for (int i = 0; i < N / k; i++)
            for (int j = 0; j < N; j++)
            {
                a[i][j] = A[i][j];
                b[j][i] = B[j][i];
            }

        for (int i = 1; i < size; i++)
        {
            MPI_Send(&A[i / k * k][0], 1, vrste, i, 0, MPI_COMM_WORLD);
            MPI_Send(&B[0][i % k * k], 1, kolone, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&a[0][0], N * N / k, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&b[0][0], N * N / k, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_File_open(MPI_COMM_WORLD, "A.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    MPI_File_write_at_all(fh,rank*sizeof(int)*N, &a[rank % k][0], N, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);

    int ndims[2] = {N, N};
    int distr[2] = {MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_BLOCK};
    int dargs[2] = {MPI_DISTRIBUTE_DFLT_DARG, MPI_DISTRIBUTE_DFLT_DARG};
    int psizes[2] = {k, k};

    MPI_Type_create_darray(size, rank, 2, ndims, distr, dargs, psizes, MPI_ORDER_C, MPI_INT, &fileview);
    MPI_Type_commit(&fileview);         
    
    MPI_File_open(MPI_COMM_WORLD, "B.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    MPI_File_set_view(fh,0,MPI_INT,fileview,"native",MPI_INFO_NULL);

    MPI_File_write_all(fh, &b[rank / k * k][0],N ,MPI_INT,MPI_STATUS_IGNORE);
    MPI_File_close(&fh);

    for (int i = 0; i < N / k; i++)
        for (int j = 0; j < N / k; j++)
        {
            c[i][j] = 0;
            for (int p = 0; p < N; p++)
                c[i][j] += a[i][p] * b[p][j];
        }

    MPI_File_open(MPI_COMM_WORLD, "C.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    MPI_File_set_view(fh, 0, MPI_INT, fileview, "native", MPI_INFO_NULL);
    MPI_File_write_all(fh, &c[0][0], N / k * N / k, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);

    MPI_Finalize();
}