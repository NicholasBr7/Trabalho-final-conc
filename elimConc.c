/*
    Esse arquivo realiza a eliminação gaussiana de maneira concorrente;
    Passa-se um arquivo txt com uma matriz aumentada (caso contrário teremos erro),
    um número de threads necessárias, calcula-se a matriz escalonada reduzida, 
    realiza-se a substituição reversa, guarda-se o matriz simplificada 
    e a solução em um arquivo de saída.

    Entrada: ./nomeArquivo matriz.txt saidaConc.txt nThreads
    Saída: arquivo txt saidaConc.txt
    Console: tempos de inicialização, processamento e finalização
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include "functions.h"
#include "functions.c"

int nThreads;
int bloqueadas = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

typedef struct {
    int id;           
    int dim;          
    double** matriz;  
} tArgs;

//Função de barreira fornecida pela professora
void barreira(int nthreads) {
    pthread_mutex_lock(&mutex); // Início da seção crítica
    if (bloqueadas == (nthreads - 1)) { 
        // Última thread a chegar na barreira
        pthread_cond_broadcast(&cond);
        bloqueadas = 0;
    } else {
        bloqueadas++;
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex); // Fim da seção crítica
}

void *eliminacao_gaussiana_conc(void *arg) {
    tArgs *args = (tArgs*) arg;
    int n = args->dim;
    double** matriz = args->matriz;

    for (int k = 0; k < n; k++) {
        if (args->id == 0 && fabs(matriz[k][k]) < 1e-9) {
            fprintf(stderr, "Erro: pivô zero encontrado na linha %d.\n", k);
            exit(EXIT_FAILURE);
        }

        for (int i = k + 1 + args->id; i < n; i += nThreads) {
            double fator = matriz[i][k] / matriz[k][k];
            for (int j = k; j <= n; j++) {
                matriz[i][j] -= fator * matriz[k][j];
            }
        }

        barreira(nThreads);
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    clock_t inicioInit, fimInit, inicioProc, fimProc, inicioLib, fimLib;
    inicioInit = clock();

    if (argc != 4) {
        fprintf(stderr, "Use: %s <arquivo_entrada.txt> <arquivo_saida.txt> <nThreads>\n", argv[0]);
        return 1;
    }

    const char* nome_arquivo = argv[1];
    const char* nome_saida = argv[2];
    nThreads = atoi(argv[3]);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&cond, NULL);

    int linhas, colunas;
    double** matriz = ler_matriz_aumentada(nome_arquivo, &linhas, &colunas);

    if (colunas != linhas + 1) {
        fprintf(stderr, "Erro: a matriz no arquivo não é uma matriz aumentada válida (%dx%d).\n", linhas, colunas);
        liberar_matriz(matriz, linhas);
        return 1;
    }

    pthread_t *tid = (pthread_t*)malloc(nThreads * sizeof(pthread_t));
    tArgs *args = (tArgs*)malloc(nThreads * sizeof(tArgs));

    fimInit = clock();
    double tempo_totalInit = (double)(fimInit - inicioInit) / CLOCKS_PER_SEC;
    printf("Tempo total inicializacao: %.3lf segundos\n", tempo_totalInit);

    inicioProc = clock();

    for (int i = 0; i < nThreads; i++) {
        args[i].id = i;
        args[i].dim = linhas;
        args[i].matriz = matriz;
        if (pthread_create(&tid[i], NULL, eliminacao_gaussiana_conc, (void*)&args[i])) {
            fprintf(stderr, "Erro ao criar thread.\n");
            return 1;
        }
    }

    for (int i = 0; i < nThreads; i++) {
        pthread_join(tid[i], NULL);
    }

    double* solucao = (double*)malloc(linhas * sizeof(double));
    substituicao_regressiva(matriz, linhas, solucao);

    fimProc = clock();
    double tempo_totalProc = (double)(fimProc - inicioProc) / CLOCKS_PER_SEC;
    printf("Tempo total processamento: %.3lf segundos\n", tempo_totalProc);


    inicioLib = clock();

    escreve_matriz_arquivo(linhas, colunas, matriz, solucao, nome_saida);

    liberar_matriz(matriz, linhas);
    free(solucao);
    free(tid);
    free(args);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    fimLib = clock();
    double tempo_totalLib = (double)(fimLib - inicioLib) / CLOCKS_PER_SEC;
    printf("Tempo total finalizacao: %.3lf segundos\n", tempo_totalLib);

    return 1;
}
