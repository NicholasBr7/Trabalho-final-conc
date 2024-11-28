#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "functions.h"
#include "functions.c"
#include <time.h>

int main(int argc, char* argv[]) {
    clock_t inicioInit, fimInit, inicioProc, fimProc, inicioLib, fimLib;
    inicioInit = clock();

    if (argc != 3) {
        fprintf(stderr, "Use: %s <arquivo_entrada.txt> <arquivo_saida.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* nome_arquivo = argv[1];
    int linhas, colunas;

    double** matriz = ler_matriz_aumentada(nome_arquivo, &linhas, &colunas);

    if (colunas != linhas + 1) {
        fprintf(stderr, "Erro: a matriz no arquivo não é uma matriz aumentada válida (%dx%d).\n", linhas, colunas);
        liberar_matriz(matriz, linhas);
        return EXIT_FAILURE;
    }

    fimInit = clock();
    double tempo_totalInit = (double)(fimInit - inicioInit) / CLOCKS_PER_SEC;
    printf("Tempo total inicializacao: %.6lf segundos\n", tempo_totalInit);

    inicioProc = clock();

    double* solucao = eliminacao_gaussiana(matriz, linhas);

    fimProc = clock();
    double tempo_totalProc = (double)(fimProc - inicioProc) / CLOCKS_PER_SEC;
    printf("Tempo total processamento: %.6lf segundos\n", tempo_totalProc);

    inicioLib = clock();

    escreve_matriz_arquivo(linhas, colunas, matriz, solucao, argv[2]);

    liberar_matriz(matriz, linhas);

    fimLib = clock();
    double tempo_totalLib = (double)(fimLib - inicioLib) / CLOCKS_PER_SEC;
    printf("Tempo total finalizacao: %.6lf segundos\n", tempo_totalLib);

    return 0;
}

