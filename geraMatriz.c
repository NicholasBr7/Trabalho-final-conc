/*
    Código baseado no gera_vet_rand.c do laboratório 2;
    Esse arquivo serve gerar uma matriz com valores aleatórios inteiros 
    entre -100 e 100. Passa-se o número de linhas/número de variáveis 
    os quais são iguais, já que a matriz será quadrada. O número de 
    colunas será sempre linhas + 1. 
    Seu resultado será salvo em um arquivo txt chamado "matriz.txt"

    Entrada: ./nomeArquivo nLinhas 
    Saída: arquivos txt "matriz.txt"
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Macro que serve para imprimir ou não a matriz gerada no console
//#define IMPRIME 

int main(int argc, char *argv[]) {
    float *matriz; 
    int linhas, colunas; 
    long long int tam; 
    FILE *descritorTexto;  

    if (argc < 2) {
        printf("Parâmetros inválidos. Padrão: <nomeArquivo> <nLinhas> \n");
        return 1;
    }
    linhas = atoi(argv[1]);
    colunas = atoi(argv[1]) + 1;
    tam = linhas * colunas;

    // aloca memória para a matriz
    matriz = (float*) malloc(sizeof(float) * tam);
    if (!matriz) {
        printf("Erro de alocação de memória para a matriz");
        return 1;
    }

    // Gera números inteiros entre -100 e 100
    srand(time(NULL));
    for (long int i = 0; i < tam; i++) {
        *(matriz + i) = (rand() % 201) - 100; 
    }

    // Imprimir na saída padrão e no arquivo de texto a matriz gerada
    descritorTexto = fopen("matriz.txt", "w");
    if (!descritorTexto) {
        printf("Erro de abertura do arquivo de texto");
        return 1;
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

