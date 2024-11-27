#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double* carregar_solucao(const char *arquivo, int *linhas) {
    double* solucao = (double*)malloc(*linhas * sizeof(double*));

    FILE *fp = fopen(arquivo, "r");
    if (!fp) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    char linha[1024];
    int encontrou_solucao = 0;
    *linhas = 0;

    while (fgets(linha, sizeof(linha), fp)) {
        if (strstr(linha, "Solucao")) {
            encontrou_solucao = 1;
            continue;
        }
        if (encontrou_solucao) {
            double valor;
            if (sscanf(linha, "%lf", &valor) == 1) {
                solucao[(*linhas)++] = valor;
            }
        }
    }

    fclose(fp);

    if (!encontrou_solucao) {
        fprintf(stderr, "Seção 'Solucao' não encontrada no arquivo.\n");
        exit(EXIT_FAILURE);
    }

    return solucao;
}

double** ler_matriz_a_e_vetor_b(const char* nome_arquivo, int* linhas, int* colunas, double** vetor_b) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    *linhas = 0;
    *colunas = 0;
    double temp;

    // Determinar o número de linhas e colunas
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

    // Alocar memória para matriz A e vetor b
    double** matriz_a = (double**)malloc(*linhas * sizeof(double*));
    *vetor_b = (double*)malloc(*linhas * sizeof(double));

    if (matriz_a == NULL || *vetor_b == NULL) {
        perror("Erro ao alocar memória");
        fclose(arquivo);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *linhas; i++) {
        matriz_a[i] = (double*)malloc((*colunas - 1) * sizeof(double));
        if (matriz_a[i] == NULL) {
            perror("Erro ao alocar memória");
            fclose(arquivo);
            exit(EXIT_FAILURE);
        }
    }

    // Ler dados e preencher matriz A e vetor b
    for (int i = 0; i < *linhas; i++) {
        for (int j = 0; j < *colunas; j++) {
            double valor;
            fscanf(arquivo, "%lf", &valor);

            if (j < *colunas - 1) {
                matriz_a[i][j] = valor;  // Elementos da matriz A
            } else {
                (*vetor_b)[i] = valor;  // Última coluna, vetor b
            }
        }
    }

    fclose(arquivo);

    return matriz_a;
}

// Função para imprimir a matriz A
void imprimir_matriz(double** matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%.3f ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Função para imprimir o vetor b
void imprimir_vetor(double* vetor, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%.3f\n", vetor[i]);
    }
}

// Função para multiplicar matriz A por vetor x
double* multiplicar_matriz_vetor(double** matriz, double* vetor, int linhas, int colunas) {
    double* resultado = (double*)malloc(linhas * sizeof(double));
    if (resultado == NULL) {
        perror("Erro ao alocar memória para o resultado");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < linhas; i++) {
        resultado[i] = 0.0;
        for (int j = 0; j < colunas; j++) {
            resultado[i] += matriz[i][j] * vetor[j];
        }
    }

    return resultado;
}

// Função para verificar se dois vetores são iguais
int comparar_vetores(double* vetor1, double* vetor2, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (fabs(vetor1[i] - vetor2[i]) > 1e-6) { // Tolerância para comparações de ponto flutuante
            return 0;
        }
    }
    return 1;
}


int main(int argc, char* argv[]) {
    double** matriz_a;
    double* vetor_x;
    double* vetor_b;
    double* resultado;
    int linhas, colunas;

    if(argc != 3) {
        fprintf(stderr, "Use: %s <matriz_original.txt> <matriz_triangular_solucao.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    matriz_a = ler_matriz_a_e_vetor_b(argv[1], &linhas, &colunas, &vetor_b);

    printf("Linha: %d\n", linhas);
    printf("Coluna: %d\n", colunas);

    printf("Matriz A:\n");
    imprimir_matriz(matriz_a, linhas, colunas - 1);

    printf("\nVetor b:\n");
    imprimir_vetor(vetor_b, linhas);

    vetor_x = carregar_solucao(argv[2], &linhas);
    printf("\nVetor x:\n");
    imprimir_vetor(vetor_x, linhas);

    printf("Linha: %d\n", linhas);
    printf("Coluna: %d\n", colunas);
    resultado = multiplicar_matriz_vetor(matriz_a, vetor_x, linhas, colunas-1);
    printf("\nResultado:\n");
    imprimir_vetor(resultado, linhas);
    

    return 0;
}
