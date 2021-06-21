#include <stdio.h>
#include <mpi/mpi.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int rank;

    struct zaposleni
    {
        int matbr;
        char ime[50];
        char prezime[50];
        float plata;
    } zaposlen;
    int array_of_lenghts[4] = {
        sizeof(int),
        sizeof(char) * 50,
        sizeof(char) * 50,
        sizeof(float)};
    MPI_Datatype array_of_types[4] = {
        MPI_INT, MPI_CHAR, MPI_CHAR, MPI_FLOAT};
    MPI_Aint array_of_displacements[4];
    MPI_Datatype zaposlenType;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Address(&zaposlen.matbr, &array_of_displacements[0]);
    MPI_Address(&zaposlen.ime, &array_of_displacements[1]);
    MPI_Address(&zaposlen.prezime, &array_of_displacements[2]);
    MPI_Address(&zaposlen.plata, &array_of_displacements[3]);

    array_of_displacements[3] = array_of_displacements[3] - array_of_displacements[0];
    array_of_displacements[2] = array_of_displacements[2] - array_of_displacements[0];
    array_of_displacements[1] = array_of_displacements[1] - array_of_displacements[0];
    array_of_displacements[0] = 0;

    MPI_Type_struct(4, array_of_lenghts, array_of_displacements, array_of_types, &zaposlenType);
    MPI_Type_commit(&zaposlenType);
    
    if (rank == 0)
    {
        printf("Unesi matbr:");
        scanf("%d", &zaposlen.matbr);
        printf("Unesi ime:");
        scanf("%s", zaposlen.ime);
        printf("Unesi prezime:");
        scanf("%s", zaposlen.prezime);
        printf("Unesi platu:");
        scanf("%f", &zaposlen.plata);
    }
    MPI_Bcast(&zaposlen,1,zaposlenType,0,MPI_COMM_WORLD);

    if(rank == 1)
    {
        printf("matbr: %d\n",zaposlen.matbr);
        printf("ime: %s\n",zaposlen.ime);
        printf("prezime: %s\n",zaposlen.prezime);
        printf("plata: %f\n",zaposlen.plata);
    }

    MPI_Finalize();
}