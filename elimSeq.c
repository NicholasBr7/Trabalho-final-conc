#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Função para imprimir uma matriz aumentada
void printMatrix(double **matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%8.3lf ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Função para realizar a eliminação gaussiana
void gaussianElimination(double **matrix, int n) {
    for (int k = 0; k < n - 1; k++) {
        // Pivotamento parcial
        for (int i = k + 1; i < n; i++) {
            if (fabs(matrix[k][k]) < 1e-9) {
                printf("Erro: Pivot nulo. A matriz não pode ser resolvida.\n");
                return;
            }
            double factor = matrix[i][k] / matrix[k][k];
            for (int j = k; j <= n; j++) {
                matrix[i][j] -= factor * matrix[k][j];
            }
        }
    }
}

// Função para realizar a substituição regressiva e encontrar o vetor de soluções
void backSubstitution(double **matrix, int n, double *solution) {
    for (int i = n - 1; i >= 0; i--) {
        solution[i] = matrix[i][n];
        for (int j = i + 1; j < n; j++) {
            solution[i] -= matrix[i][j] * solution[j];
        }
        solution[i] /= matrix[i][i];
    }
}

int main() {
    int n;
    printf("Entre com o tamanho da matriz: ");
    scanf("%d", &n);

    // Alocar matriz aumentada
    double **matrix = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double *)malloc((n + 1) * sizeof(double));
    }

    // Ler a matriz aumentada
    printf("Entre com os elementos da matriz aumentada (%d x %d):\n", n, n + 1);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= n; j++) {
            scanf("%lf", &matrix[i][j]);
        }
    }

    printf("\nMatriz aumentada inicial:\n");
    printMatrix(matrix, n, n + 1);

    // Eliminação Gaussiana
    gaussianElimination(matrix, n);

    printf("Matriz triangular superior:\n");
    printMatrix(matrix, n, n + 1);

    // Substituição regressiva
    double *solution = (double *)malloc(n * sizeof(double));
    backSubstitution(matrix, n, solution);

    // Imprimir soluções
    printf("Soluções:\n");
    for (int i = 0; i < n; i++) {
        printf("x[%d] = %.3lf\n", i, solution[i]);
    }

    // Liberar memória
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(solution);

    return 0;
}
