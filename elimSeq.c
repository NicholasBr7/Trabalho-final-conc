#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "functions.c"
#include "functions.h"
//#include "timer.h"
//#include <time.h> 

int main(int argc, char* argv[]) {
    //clock_t t; 
    //t = clock();

    if(argc != 3) {
        fprintf(stderr, "Use: %s <arquivo_entrada.txt> <arquivo_saida.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* nome_arquivo = argv[1];
    int linhas, colunas;

    double** matriz = ler_matriz_aumentada(nome_arquivo, &linhas, &colunas);

    if(colunas != linhas + 1) {
        fprintf(stderr, "Erro: a matriz no arquivo nao e uma matriz aumentada valida (%dx%d).\n", linhas, colunas);
        liberar_matriz(matriz, linhas);
        return EXIT_FAILURE;
    }

    double* solucao = eliminacao_gaussiana(matriz, linhas);

    escreve_matriz_arquivo(linhas, colunas, matriz, solucao, argv[2]);

    liberar_matriz(matriz, linhas);

    //t = clock() - t; 
    //double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 

    //printf("Tempo total sequencial: %lf\n", time_taken);

    return 0;
}
