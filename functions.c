#include<stdio.h>
#include<stdlib.h>
#include<math.h>

//Função para ler a matriz estendida
double** ler_matriz_aumentada(const char* nome_arquivo, int* linhas, int* colunas) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo");
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

//Libera o espaço de memória da matriz
void liberar_matriz(double** matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

//Realiza o processo de achar solução via matriz reduzida
void substituicao_regressiva(double** matriz, int n, double* solucao) {
    for (int i = n - 1; i >= 0; i--) {
        solucao[i] = matriz[i][n];
        for (int j = i + 1; j < n; j++) {
            solucao[i] -= matriz[i][j] * solucao[j];
        }
        solucao[i] /= matriz[i][i];
    }
}

//Escreve a solução em um arquivo txt
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
    for(int i = 0; i < linhas; i++){
        fprintf(arquivo, "%.3lf\n", *(solucao+i));
    }
    
    fclose(arquivo);
}

//Eliminação gaussiana sequencial
double* eliminacao_gaussiana(double** matriz, int n) {
    for (int k = 0; k < n; k++) {
        if (fabs(matriz[k][k]) < 1e-9) {
            fprintf(stderr, "Erro: pivô zero encontrado na linha %d.\n", k);
            exit(EXIT_FAILURE);
        }

        for (int i = k + 1; i < n; i++) {
            double fator = matriz[i][k] / matriz[k][k];
            for (int j = k; j <= n; j++) {
                matriz[i][j] -= fator * matriz[k][j];
            }
        }
    }

    double* solucao = (double*)malloc(n * sizeof(double));

    substituicao_regressiva(matriz, n, solucao);

    return solucao;
}