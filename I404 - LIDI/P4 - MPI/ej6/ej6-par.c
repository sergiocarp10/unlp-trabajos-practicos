// IMPLEMENTACIÓN PARALELA

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COORDINATOR 0

// ------------------------------------------------

// N se recibe por parámetro
int main(int argc, char* argv[]){
    int i, j, k, numProcs, rank, n, stripSize, check=1;
	double *v;
	MPI_Status status;
	double commTimes[4], maxCommTimes[4], minCommTimes[4], commTime, totalTime;

	/* Leer argumentos */
	if ((argc != 2) || ((n = atoi(argv[1])) <= 0) ) {
	    printf("\nUsar: %s size \n  size: Dimension de la matriz y el vector\n", argv[0]);
		exit(1);
	}

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    // chequear que N sea multiplo de numProcs
    if (n % numProcs != 0) {
		printf("N debe ser multiplo del numero de procesos.\n");
		exit(1);
	}

    // calcular porcion de cada worker
	stripSize = n / numProcs;

    if (rank == COORDINATOR){
        v = (double*) malloc(sizeof(double)*n);
        for (i=0; i<n; i++) v[i] = i;
    } else {
        v = (double*) malloc(sizeof(double)*stripSize);
    }

    // REPARTIR (de master a todos)
    commTimes[0] = MPI_Wtime();
    MPI_Scatter(v, stripSize, MPI_DOUBLE, v, stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    commTimes[1] = MPI_Wtime();

    // buscar valores
    double min = 9999.9, max = -9999.9, acum = 0.0;

    for (i=0; i<stripSize; i++){
        if (v[i] < min) min = v[i];
        if (v[i] > max) max = v[i];
        acum += v[i];
    }

    // RECOLECTAR
    double minGeneral, maxGeneral, acumGeneral, prom;

    commTimes[2] = MPI_Wtime();
    MPI_Reduce(&min, &minGeneral, 1, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&max, &maxGeneral, 1, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&acum, &acumGeneral, 1, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);
    commTimes[3] = MPI_Wtime();

    // esto es para medidas de tiempo
	MPI_Reduce(commTimes, minCommTimes, 4, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 4, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

    MPI_Finalize();

    // muestra de resultados
    if (rank == COORDINATOR){
        if (minGeneral == 0) printf("min correcto \n");
        if (maxGeneral == n-1) printf("max correcto \n");

        // total time
        totalTime = maxCommTimes[3] - minCommTimes[0];
        commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]);

        printf("Multiplicacion (N=%d)\tTiempo total=%lf\tTiempo comm=%lf\n",n,totalTime,commTime);
    }

    return 0;
}