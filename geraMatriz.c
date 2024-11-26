/*
    Esse arquivo serve gerar uma matriz com valores aleatórios inteiros 
    entre -100 e 100. O número de colunas será sempre col + 1. 
    Seu resultado será salvo em um arquivo binário e o outro txt

    Entrada: ./nomeArquivo nLinhas nColunas 
    Saída: arquivos binário e txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define IMPRIME

int main(int argc, char *argv[]) {
    float *matriz; // matriz que será gerada
    int linhas, colunas; // dimensões da matriz
    long long int tam; // qtde de elementos na matriz
    FILE *descritorBinario, *descritorTexto; // descritores dos arquivos de saída
    size_t ret; // retorno da função de escrita no arquivo de saída

    if (argc < 2) {
        printf("Parâmetros inválidos. Padrão: <nLinhas> \n");
        return 1;
    }
    linhas = atoi(argv[1]);
    colunas = linhas + 1;
    tam = linhas * colunas;

    // aloca memória para a matriz
    matriz = (float*) malloc(sizeof(float) * tam);
    if (!matriz) {
        printf("Erro de alocação de memória para a matriz");
        return 1;
    }
    srand(time(NULL));
    for (long int i = 0; i < tam; i++) {
        *(matriz + i) = (rand() % 201) - 100; // Gera números inteiros entre -100 e 100
    }

    // imprimir na saída padrão e no arquivo de texto a matriz gerada
    descritorTexto = fopen("matriz.txt", "w");
    if (!descritorTexto) {
        printf("Erro de abertura do arquivo de texto");
        return 2;
    }

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            float value = matriz[i * colunas + j];
            if (value == (int)value) {
                #ifdef IMPRIME
                fprintf(stdout, "%d ", (int)value);
                #endif
                fprintf(descritorTexto, "%d ", (int)value);
            } else {
                #ifdef IMPRIME
                fprintf(stdout, "%.6f ", value);
                #endif
                fprintf(descritorTexto, "%.6f ", value);
            }
        }
        #ifdef IMPRIME
        fprintf(stdout, "\n");
        #endif
        fprintf(descritorTexto, "\n");
    }

    fclose(descritorTexto);

    // escreve a matriz no arquivo binário
    descritorBinario = fopen("matriz.bin", "wb");
    if (!descritorBinario) {
        printf("Erro de abertura do arquivo binário");
        return 3;
    }
    // escreve número de linhas e de colunas
    fwrite(&linhas, sizeof(int), 1, descritorBinario);
    fwrite(&colunas, sizeof(int), 1, descritorBinario);
    // escreve os elementos da matriz
    ret = fwrite(matriz, sizeof(float), tam, descritorBinario);
    if (ret < tam) {
        fprintf(stderr, "Erro de escrita no arquivo binário\n");
        return 4;
    }

    // finaliza o uso das variáveis
    fclose(descritorBinario);
    free(matriz);
    return 0;
}

