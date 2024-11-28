#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

pthread_barrier_t barreira;
int nThreads;

typedef struct {
    int id;           // Identificador da thread
    int dim;          // Dimensão da matriz
    double** matriz;  // Matriz aumentada
} tArgs;

double** ler_matriz_aumentada(const char* nome_arquivo, int* linhas, int* colunas) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    *linhas = 0;
    *colunas = 0;
    double temp;

    while (!feof(arquivo)) {
        int count_colunas = 0;
        char linha[50000];
        if (fgets(linha, sizeof(linha), arquivo)) {
            char* ptr = linha;
            while (sscanf(ptr, "%lf%n", &temp, &count_colunas) == 1) {
                ptr += count_colunas;
                (*colunas)++;
            }
            (*linhas)++;
        }
    }
    *colunas /= *linhas;

    rewind(arquivo);

    double** matriz = (double**)malloc(*linhas * sizeof(double*));
    for (int i = 0; i < *linhas; i++) {
        matriz[i] = (double*)malloc(*colunas * sizeof(double));
    }

    for (int i = 0; i < *linhas; i++) {
        for (int j = 0; j < *colunas; j++) {
            fscanf(arquivo, "%lf", &matriz[i][j]);
        }
    }

    fclose(arquivo);

    return matriz;
}

void liberar_matriz(double** matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

void substituicao_regressiva(double** matriz, int n, double* solucao) {
    for (int i = n - 1; i >= 0; i--) {
        solucao[i] = matriz[i][n];
        for (int j = i + 1; j < n; j++) {
            solucao[i] -= matriz[i][j] * solucao[j];
        }
        solucao[i] /= matriz[i][i];
    }
}

void *eliminacao_gaussiana(void *arg) {
    tArgs *args = (tArgs*) arg;
    int n = args->dim;
    double** matriz = args->matriz;

    for (int k = 0; k < n; k++) {
        // Apenas a thread mestre verifica o pivô
        if (args->id == 0 && fabs(matriz[k][k]) < 1e-9) {
            fprintf(stderr, "Erro: pivô zero encontrado na linha %d.\n", k);
            exit(EXIT_FAILURE);
        }

        // Sincroniza as threads antes da etapa
        pthread_barrier_wait(&barreira);

        // Eliminação paralela
        for (int i = k + 1 + args->id; i < n; i +=  nThreads) {
            double fator = matriz[i][k] / matriz[k][k];
            for (int j = k; j <= n; j++) {
                matriz[i][j] -= fator * matriz[k][j];
            }
        }

        // Sincroniza após a eliminação
        pthread_barrier_wait(&barreira);
    }

    pthread_exit(NULL);
}

void escreve_matriz_arquivo(int linhas, int colunas, double **matriz, double* solucao, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    
    fprintf(arquivo, "Matriz\n");
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            fprintf(arquivo, "%.3lf ", matriz[i][j]);
        }
        fprintf(arquivo, "\n");
    }

    fprintf(arquivo, "\nSolucao\n");
    for (int i = 0; i < linhas; i++) {
        fprintf(arquivo, "%.3lf\n", solucao[i]);
    }
    
    fclose(arquivo);
}

int main(int argc, char* argv[]) {
    clock_t inicio, fim;
    inicio = clock();

    if (argc != 4) {
        fprintf(stderr, "Use: %s <arquivo_entrada.txt> <arquivo_saida.txt> <nThreads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* nome_arquivo = argv[1];
    const char* nome_saida = argv[2];
    nThreads = atoi(argv[3]);

    int linhas, colunas;
    double** matriz = ler_matriz_aumentada(nome_arquivo, &linhas, &colunas);

    if (colunas != linhas + 1) {
        fprintf(stderr, "Erro: a matriz no arquivo não é uma matriz aumentada válida (%dx%d).\n", linhas, colunas);
        liberar_matriz(matriz, linhas);
        return EXIT_FAILURE;
    }

    pthread_t *tid = (pthread_t*)malloc(nThreads * sizeof(pthread_t));
    tArgs *args = (tArgs*)malloc(nThreads * sizeof(tArgs));

    if (pthread_barrier_init(&barreira, NULL, nThreads)) {
        fprintf(stderr, "Erro ao inicializar barreira.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < nThreads; i++) {
        args[i].id = i;
        args[i].dim = linhas;
        args[i].matriz = matriz;
        if (pthread_create(&tid[i], NULL, eliminacao_gaussiana, (void*)&args[i])) {
            fprintf(stderr, "Erro ao criar thread.\n");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nThreads; i++) {
        pthread_join(tid[i], NULL);
    }

    double* solucao = (double*)malloc(linhas * sizeof(double));
    substituicao_regressiva(matriz, linhas, solucao);



    escreve_matriz_arquivo(linhas, colunas, matriz, solucao, nome_saida);

    liberar_matriz(matriz, linhas);
    free(solucao);
    free(tid);
    free(args);
    pthread_barrier_destroy(&barreira);

    fim = clock();
    double tempo_total = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo total: %.6lf segundos\n", tempo_total);

    return EXIT_SUCCESS;
}
