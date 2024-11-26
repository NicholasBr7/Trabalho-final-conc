#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "functions.h"

typedef struct {
    double** matriz;   // Matriz aumentada
    int n;             // Dimensão da matriz (n x n)
    int k;             // Etapa atual (índice do pivô)
} DadosThread;

pthread_barrier_t barreira;

// Função que realiza as operações de eliminação para uma linha específica
void* operacao_eliminacao(void* args) {
    DadosThread* dados = (DadosThread*)args;
    int n = dados->n;
    double** matriz = dados->matriz;

    int linha = dados->k + 1; // Cada thread começa na linha após o pivô
    while (linha < n) {
        pthread_barrier_wait(&barreira); // Sincroniza antes de cada etapa

        // Realiza a eliminação para a linha atual
        double fator = matriz[linha][dados->k] / matriz[dados->k][dados->k];
        for (int j = dados->k; j <= n; j++) {
            matriz[linha][j] -= fator * matriz[dados->k][j];
        }

        pthread_barrier_wait(&barreira); // Aguarda todas as threads concluírem
        linha += n; // Próxima linha atribuída a esta thread
    }

    return NULL;
}

// Função de eliminação gaussiana concorrente
double* eliminacao_gaussiana_concorrente(double** matriz, int n) {
    pthread_t threads[n];
    DadosThread dados[n];

    // Inicializa a barreira
    pthread_barrier_init(&barreira, NULL, n);

    // Cria threads para as linhas
    for (int i = 0; i < n; i++) {
        dados[i].matriz = matriz;
        dados[i].n = n;
        dados[i].k = i;
        if (pthread_create(&threads[i], NULL, operacao_eliminacao, &dados[i]) != 0) {
            perror("Erro ao criar thread");
            exit(EXIT_FAILURE);
        }
    }

    // Realiza as operações de pivô e sincronização principal
    for (int k = 0; k < n; k++) {
        // Checa se o pivô é válido
        if (fabs(matriz[k][k]) < 1e-9) {
            fprintf(stderr, "Erro: pivô zero encontrado na linha %d.\n", k);
            exit(EXIT_FAILURE);
        }

        // Define o pivô (sincronização gerenciada pelas threads)
        pthread_barrier_wait(&barreira); // Sincroniza antes de modificar o pivô
    }

    // Aguarda as threads terminarem
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destrói a barreira
    pthread_barrier_destroy(&barreira);

    // Substituição regressiva
    double* solucao = (double*)malloc(n * sizeof(double));
    substituicao_regressiva(matriz, n, solucao);

    return solucao;
}


int main(int argc, char argv[]){
    if(argc != 3) {
        fprintf(stderr, "Use: %s <arquivo_entrada.txt> <arquivo_saida.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* nome_arquivo = argv[1];
    int linhas, colunas;

    double** matriz = ler_matriz_aumentada(nome_arquivo, &linhas, &colunas);

    if(colunas != linhas + 1) {
        fprintf(stderr, "Erro: a matriz no arquivo não é uma matriz aumentada válida (%dx%d).\n", linhas, colunas);
        liberar_matriz(matriz, linhas);
        return EXIT_FAILURE;
    }

    double* solucao = eliminacao_gaussiana_concorrente(matriz, linhas);

    escreve_matriz_arquivo(linhas, colunas, matriz, solucao, argv[2]);

    liberar_matriz(matriz, linhas);

    return 0;
}
