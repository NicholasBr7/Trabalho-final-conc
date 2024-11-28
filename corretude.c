#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#define IMPRIME

double* carregar_solucao(const char *arquivo, int* num_linhas) {
    int capacidade = 10; // Capacidade inicial do vetor
    double* solucao = (double*)malloc(capacidade * sizeof(double));
    if (!solucao) {
        perror("Erro ao alocar memória para solução");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(arquivo, "r");
    if (!fp) {
        perror("Erro ao abrir o arquivo");
        free(solucao);
        exit(EXIT_FAILURE);
    }

    char linha[50000];
    int encontrou_solucao = 0;
    *num_linhas = 0;

    while (fgets(linha, sizeof(linha), fp)) {
        if (strstr(linha, "Solucao")) {
            encontrou_solucao = 1;
            continue;
        }
        if (encontrou_solucao) {
            double valor;
            if (sscanf(linha, "%lf", &valor) == 1) {
                if (*num_linhas >= capacidade) {
                    capacidade *= 2;
                    solucao = (double*)realloc(solucao, capacidade * sizeof(double));
                    if (!solucao) {
                        perror("Erro ao realocar memória para solução");
                        fclose(fp);
                        exit(EXIT_FAILURE);
                    }
                }
                solucao[(*num_linhas)++] = valor;
            }
        }
    }

    fclose(fp);

    if (!encontrou_solucao) {
        fprintf(stderr, "Seção 'Solucao' não encontrada no arquivo.\n");
        free(solucao);
        exit(EXIT_FAILURE);
    }

    return solucao;
}

double** ler_matriz_a_e_vetor_b(const char* nome_arquivo, int* linhas, int* colunas, double** vetor_b) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    char linha[50000];
    *linhas = 0;
    *colunas = 0;

    // Determinar o número de linhas e colunas
    while (fgets(linha, sizeof(linha), arquivo)) {
        int count_colunas = 0;
        double temp;
        char* ptr = linha;
        while (sscanf(ptr, "%lf%n", &temp, &count_colunas) == 1) {
            ptr += count_colunas;
            (*colunas)++;
        }
        (*linhas)++;
    }

    if (*linhas == 0 || *colunas % *linhas != 0) {
        fprintf(stderr, "Erro: arquivo inválido ou formato inconsistente.\n");
        fclose(arquivo);
        exit(EXIT_FAILURE);
    }

    *colunas /= *linhas;

    rewind(arquivo);

    // Alocar memória para matriz A e vetor b
    double** matriz_a = (double**)malloc(*linhas * sizeof(double*));
    *vetor_b = (double*)malloc(*linhas * sizeof(double));

    if (!matriz_a || !*vetor_b) {
        perror("Erro ao alocar memória");
        fclose(arquivo);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *linhas; i++) {
        matriz_a[i] = (double*)malloc((*colunas - 1) * sizeof(double));
        if (!matriz_a[i]) {
            perror("Erro ao alocar memória");
            fclose(arquivo);
            exit(EXIT_FAILURE);
        }
    }

    // Ler dados e preencher matriz A e vetor b
    for (int i = 0; i < *linhas; i++) {
        for (int j = 0; j < *colunas; j++) {
            double valor;
            if (fscanf(arquivo, "%lf", &valor) != 1) {
                fprintf(stderr, "Erro ao ler valor no arquivo.\n");
                fclose(arquivo);
                exit(EXIT_FAILURE);
            }

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

void imprimir_matriz(double** matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%.6f ", matriz[i][j]);
        }
        printf("\n");
    }
}

void imprimir_vetor(double* vetor, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%.6f\n", vetor[i]);
    }
}

double* multiplicar_matriz_vetor(double** matriz, double* vetor, int linhas, int colunas) {
    double* resultado = (double*)malloc(linhas * sizeof(double));
    if (!resultado) {
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

int comparar_vetores(double* vetor1, double* vetor2, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (fabs(vetor1[i] - vetor2[i]) > 0.5) { // Tolerância para comparações de ponto flutuante
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

    if (argc != 3) {
        fprintf(stderr, "Use: %s <matriz_original.txt> <matriz_triangular_solucao.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    matriz_a = ler_matriz_a_e_vetor_b(argv[1], &linhas, &colunas, &vetor_b);
    vetor_x = carregar_solucao(argv[2], &linhas);
    resultado = multiplicar_matriz_vetor(matriz_a, vetor_x, linhas, colunas - 1);

#ifdef IMPRIME
    printf("Matriz A:\n");
    imprimir_matriz(matriz_a, linhas, colunas - 1);

    printf("\nVetor b:\n");
    imprimir_vetor(vetor_b, linhas);

    printf("\nVetor x:\n");
    imprimir_vetor(vetor_x, linhas);

    printf("\nResultado:\n");
    imprimir_vetor(resultado, linhas);
#endif

    if (!comparar_vetores(resultado, vetor_b, linhas)) {
        printf("Erro: os vetores não coincidem.\n");
    } else {
        printf("Sucesso: os vetores coincidem.\n");
    }

    // Liberar memória
    free(vetor_b);
    free(vetor_x);
    free(resultado);
    for (int i = 0; i < linhas; i++) {
        free(matriz_a[i]);
    }
    free(matriz_a);

    return 0;
}
