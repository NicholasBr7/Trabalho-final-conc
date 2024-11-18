#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TXT

int main(int argc, char *argv[]) {
    float *matriz; //matriz que será gerada
    int linhas, colunas; //dimensoes da matriz
    long long int tam; //qtde de elementos na matriz
    FILE * descritorArquivo; //descritor do arquivo de saida
    size_t ret; //retorno da funcao de escrita no arquivo de saida

    if (argc < 3) {
        printf("Parâmetros inválidos. Padrão: <nomeArquivo> <nLinhas> <nColunas> <arquivoSaida> \n");
        return 1;
    }
    linhas = atoi(argv[1]);
    colunas = atoi(argv[2]) + 1;
    tam = linhas * colunas;

    //aloca memoria para a matriz
    matriz = (float*) malloc(sizeof(float) * tam);
    if (!matriz) {
        printf("Erro de alocação de memória para a matriz");
        return 1;
    }
    srand(time(NULL));
    for (long int i = 0; i < tam; i++) {
        *(matriz + i) = (rand() % 201) - 100; // Gera números inteiros entre -100 e 100
    }

    //imprimir na saida padrao a matriz gerada
    #ifdef TXT
    for(int i=0; i<linhas; i++) {
        for(int j=0; j<colunas; j++)
        fprintf(stdout, "%.6f ", matriz[i*colunas+j]);
      fprintf(stdout, "\n");
   }
    #endif

    //escreve a matriz no arquivo
    //abre o arquivo para escrita binaria
    descritorArquivo = fopen(argv[3], "wb");
    if(!descritorArquivo) {
      printf("Erro de abertura do arquivo");
      return 3;
    }
    //escreve numero de linhas e de colunas
    ret = fwrite(&linhas, sizeof(int), 1, descritorArquivo);
    ret = fwrite(&colunas, sizeof(int), 1, descritorArquivo);
    //escreve os elementos da matriz
    ret = fwrite(matriz, sizeof(float), tam, descritorArquivo);
    if(ret < tam) {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }

    //finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(matriz);
    return 0;
}