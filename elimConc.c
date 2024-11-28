#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

int nThreads;
pthread_mutex_t mutex;
pthread_cond_t cond;
int bloqueadas = 0;

typedef struct {
    int id;           // Identificador da thread
    int dim;          // Dimensão da matriz
    double** matriz;  // Matriz aumentada
} tArgs;

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
        barreira(nThreads);

        // Eliminação paralela
        for (int i = k + 1 + args->id; i < n; i += nThreads) {
            double fator = matriz[i][k] / matriz[k][k];
            for (int j = k; j <= n; j++) {
                matriz[i][j] -= fator * matriz[k][j];
            }
        }

        // Sincroniza após a eliminação
        barreira(nThreads);
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
            if(fabs(matriz[i][j]) < 1e-3){
                fprintf(arquivo, "%.3lf", 0.000);
            }
            else{
                fprintf(arquivo, "%.3lf", matriz[i][j]);
            }
            if (j < colunas - 1) {
                fprintf(arquivo, " ");
            }
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
    printf("Tempo total inicializacao: %.6lf segundos\n", tempo_totalInit);

    inicioProc = clock();

    for (int i = 0; i < nThreads; i++) {
        args[i].id = i;
        args[i].dim = linhas;
        args[i].matriz = matriz;
        if (pthread_create(&tid[i], NULL, eliminacao_gaussiana, (void*)&args[i])) {
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
    printf("Tempo total processamento: %.6lf segundos\n", tempo_totalProc);


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
    printf("Tempo total finalizacao: %.6lf segundos\n", tempo_totalLib);

    return 0;
}

