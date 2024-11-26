#include<stdio.h>
#include<stdlib.h>
#include<math.h>

double** ler_matriz_aumentada(const char* nome_arquivo, int* linhas, int* colunas);

void liberar_matriz(double** matriz, int linhas);

void substituicao_regressiva(double** matriz, int n, double* solucao);

void escreve_matriz_arquivo(int linhas, int colunas, double **matriz, double* solucao, const char *nomeArquivo);

double* eliminacao_gaussiana(double** matriz, int n);
