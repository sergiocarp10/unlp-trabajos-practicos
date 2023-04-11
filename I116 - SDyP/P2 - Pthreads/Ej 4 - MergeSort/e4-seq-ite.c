// PARALELIZAR LA ORDENACION POR MEZCLA DE UN VECTOR DE N ELEMENTOS

// Cabeceras
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include "tiempo.h"
#include <time.h>       // random seed

// Constantes
// #define DEBUG

// Prototipos de funcion
void extraerParams(int argc, char* argv[]);
void inicializar(double*);
void ordenarIterativo(double*);
void combinar(double* V, int left, int medio, int right);

// Variables compartidas
int N;

// Programa principal
int main(int argc, char* argv[]){
    int i;
    
    extraerParams(argc, argv);

    // alocar memoria
    double* V = (double*) malloc(N * sizeof(double));

    // inicializar vector con numeros random
    inicializar(V);

    // mergesort iterativo (para evitar overhead de recursión)
    double t0 = dwalltime();

    ordenarIterativo(V);

    double t1 = dwalltime();
    printf("Para N=%d, mide %f segundos\n", N, t1 - t0);

    // Verificar ordenacion (menor a mayor)
    double ant = V[0];
    double act;

    for (i=1; i<N; i++){
        act = V[i];
        if (act < ant) printf("Error: V[%d] = %.2f es menor que V[%d] = %.2f \n", i, act, i-1, ant);
        ant = act;
    }

    // liberar memoria
    free(V);

    return 0;
}

// Implemento funciones
static inline int min(int n1, int n2){
    return (n1 < n2) ? n1 : n2;
}

void ordenarIterativo(double* V){
    int len, left, prevLen = 1;
    int medio, right;

    // len <= 2*N es un fix a los vectores con N no potencia de 2
    for (len=2; len<=2*N; len *= 2){
        for (left=0; left < N-prevLen; left += len){
            right = min(left + len - 1, N-1);
            medio = left + prevLen - 1;

            #ifdef DEBUG
                printf("len = %d, left = %d \n", len, left);
            #endif

            combinar(V, left, medio, right);
        }

        prevLen = len;
    }
}

void combinar(double* V, int left, int medio, int right){
    int len1 = (medio - left) + 1;
    int len2 = (right - medio);
    int i = 0, j = 0, k;

    // crear arreglos temporales
    double* L = (double*) malloc(len1 * sizeof(double));
    double* R = (double*) malloc(len2 * sizeof(double));

    // copiar datos
    for (k=0; k<len1; k++) L[k] = V[left+k];
    for (k=0; k<len2; k++) R[k] = V[medio+k+1];

    // realizar ordenacion por cada L[i] y R[j]
    for (k=left; k<=right; k++){
        if (i >= len1) V[k] = R[j++];       // si se acabaron los de L, copiar los de R
        else if (j >= len2) V[k] = L[i++];  // si se acabaron los de R, copiar los de L
        else if (L[i] < R[j]) V[k] = L[i++];    // copiar el menor entre L y R
        else V[k] = R[j++];

        #ifdef DEBUG
            printf("V[%d] = %.2f \n", k, V[k]);
        #endif
    }

    // liberar memoria temporal
    free(L);
    free(R);
}

void inicializar(double* V){
    int i;

    srand(time(NULL));

    for (i=0; i<N; i++){
        V[i] = rand() % 10000;

        #ifdef DEBUG
            printf("V[%d] = %.2f \n", i, V[i]);
        #endif
    }
}

void extraerParams(int argc, char* argv[]){
    if (argc < 2) {
        printf("Especificar N\n");
        exit(1);
    }

    N = atoi(argv[1]);

    if (N <= 0) {
        printf("N debe ser positivo\n");
        exit(2);
    }
}