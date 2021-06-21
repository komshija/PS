#include <stdio.h>
#include <mpi/mpi.h>
#define BUFSIZE 10

int main(int argc, char *argv[])
{
    int buf[BUFSIZE];
    int rank, size,offset;
    MPI_File fh;
    MPI_Datatype ftype;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int i = 0; i < BUFSIZE; i++)
        buf[i] = rank * BUFSIZE + i;

    MPI_File_open(MPI_COMM_WORLD, "fajl1.dat", MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
    offset = rank * sizeof(int) * BUFSIZE;
    MPI_File_seek(fh,offset,MPI_SEEK_CUR);
    MPI_File_write(fh, &buf[0], BUFSIZE, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);

    MPI_File_open(MPI_COMM_WORLD,"fajl1.dat",MPI_MODE_RDONLY,MPI_INFO_NULL,&fh);
    MPI_File_read_at(fh,offset,buf,10,MPI_INT,MPI_STATUS_IGNORE);
    for(int i = 0 ; i < BUFSIZE;i++)
        printf("%d ", buf[i]);
    printf("\n");
    MPI_File_close(&fh);

    MPI_Type_vector(BUFSIZE/2,2,size*2,MPI_INT,&ftype);
    MPI_Type_commit(&ftype);

    MPI_File_open(MPI_COMM_WORLD,"fajl2.dat",MPI_MODE_CREATE | MPI_MODE_WRONLY,MPI_INFO_NULL,&fh);
    offset = 2*rank*sizeof(int);
    MPI_File_set_view(fh,offset,MPI_INT,ftype,"native",MPI_INFO_NULL);
    MPI_File_write_all(fh,buf,BUFSIZE,MPI_INT,MPI_STATUS_IGNORE);
    MPI_File_close(&fh);


    MPI_Finalize();
}