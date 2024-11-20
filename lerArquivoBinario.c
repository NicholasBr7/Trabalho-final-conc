/*
    Esse arquivo serve unicamente para ler um arquivo binário
    para caso seja necessário conferir o conteúdo.

    Entrada: ./nomePrograma arquivoBinario
    Saída: imprime no console o conteúdo do arquivo
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    float *matriz; // matriz que será lida
    int linhas, colunas; // dimensões da matriz
    long long int tam; // quantidade de elementos na matriz
    FILE *descritorArquivo; // descritor do arquivo binário
    size_t ret; // retorno da função de leitura do arquivo binário

    if (argc < 2) {
        printf("Parâmetros inválidos. Padrão: <arquivoBinario>\n");
        return 1;
    }

    // abre o arquivo binário para leitura
    descritorArquivo = fopen(argv[1], "rb");
    if (!descritorArquivo) {
        printf("Erro de abertura do arquivo binário\n");
        return 2;
    }

    // lê número de linhas e colunas
    fread(&linhas, sizeof(int), 1, descritorArquivo);
    fread(&colunas, sizeof(int), 1, descritorArquivo);
    tam = linhas * colunas;

    // aloca memória para a matriz
    matriz = (float *)malloc(sizeof(float) * tam);
    if (!matriz) {
        printf("Erro de alocação de memória para a matriz\n");
        fclose(descritorArquivo);
        return 3;
    }

    // lê os elementos da matriz
    ret = fread(matriz, sizeof(float), tam, descritorArquivo);
    if (ret < tam) {
        fprintf(stderr, "Erro de leitura do arquivo binário\n");
        free(matriz);
        fclose(descritorArquivo);
        return 4;
    }

    // imprime a matriz lida
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%.6f ", matriz[i * colunas + j]);
        }
        printf("\n");
    }

    // finaliza o uso das variáveis
    free(matriz);
    fclose(descritorArquivo);
    return 0;
}
