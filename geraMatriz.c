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
    float *matriz;
    int linhas, colunas;
    long long int tam;
    FILE *descritorTexto;

    if (argc < 3) {
        printf("Parametros invalidos. Padrao: <nome_do_programa> <numero_de_linhas> <nome_do_arquivo>\n");
        return 1;
    }
    linhas = atoi(argv[1]);
    colunas = linhas + 1;
    tam = linhas * colunas;

    matriz = (float*) malloc(sizeof(float) * tam);
    if (!matriz) {
        printf("Erro de alocação de memória para a matriz");
        return 1;
    }
    srand(time(NULL));
    for (long int i = 0; i < tam; i++) {
        *(matriz + i) = (rand() % 201) - 100;
    }

    descritorTexto = fopen(argv[2], "w");
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

    free(matriz);
    return 0;
}
