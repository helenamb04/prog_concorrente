/* 
 * Multiplicação de Matrizes Concorrente
 * Helena Marques Batista - 122071866
 * Disciplina: Programação Concorrente (ICP-361) 2024-2
 * Prof. Silvana Rossetto
 * Laboratório 3
 * Código sequencial
 */

#include <stdio.h>
#include <stdlib.h>
// mantive a biblioteca pois quando tirei deu problema no timer ??
#include <pthread.h>
#include "timer.h"

// Função para leitura da matriz a partir do arquivo binário
float* ler_matriz(const char* arquivo, int* linhas, int* colunas) {
    FILE *descritorArquivo = fopen(arquivo, "rb");
    if (!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo %s\n", arquivo);
        exit(1);
    }

    fread(linhas, sizeof(int), 1, descritorArquivo);
    fread(colunas, sizeof(int), 1, descritorArquivo);
    long long int tam = (*linhas) * (*colunas);

    float* matriz = (float*) malloc(sizeof(float) * tam);
    if (!matriz) {
        fprintf(stderr, "Erro de alocação de memória para a matriz\n");
        exit(2);
    }

    fread(matriz, sizeof(float), tam, descritorArquivo);
    fclose(descritorArquivo);
    return matriz;
}

// Função para escrever a matriz em um arquivo binário
void escrever_matriz(const char* arquivo, float* matriz, int linhas, int colunas) {
    FILE *descritorArquivo = fopen(arquivo, "wb");
    if (!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo %s\n", arquivo);
        exit(1);
    }

    fwrite(&linhas, sizeof(int), 1, descritorArquivo);
    fwrite(&colunas, sizeof(int), 1, descritorArquivo);
    fwrite(matriz, sizeof(float), linhas * colunas, descritorArquivo);
    fclose(descritorArquivo);
}

int main(int argc, char *argv[]) {
    int nthreads;        // Número de threads
    int nA, mA;          // Dimensões da matriz A
    int mB, pB;          // Dimensões da matriz B
    float *A, *B, *C;    // Matrizes A, B e C
    double inicio, fim, inicio_t, delta;

    // Início da temporização da inicialização
    GET_TIME(inicio);
    inicio_t = inicio;

    // Verificação dos argumentos de entrada
    if(argc < 4) {
        fprintf(stderr, "Uso: %s <arquivo_A> <arquivo_B> <arquivo_C>\n", argv[0]);
        return 1;
    }

    // Leitura das matrizes A e B
    A = ler_matriz(argv[1], &nA, &mA);
    B = ler_matriz(argv[2], &mB, &pB);

    // Verificação de compatibilidade das matrizes
    if(mA != mB) {
        fprintf(stderr, "Erro: Número de colunas da matriz A deve ser igual ao número de linhas da matriz B\n");
        return 6;
    }

    // Alocação da matriz C
    C = (float *) malloc(sizeof(float) * nA * pB);
    if(!C) {
        fprintf(stderr, "Erro ao alocar memória para a matriz C\n");
        return 4;
    }

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de inicialização: %lf segundos\n", delta);

    // Início da multiplicação concorrente
    GET_TIME(inicio);

    // Calcula a matriz de saída
    for (int i = 0; i < nA; i++) {
        for (int j = 0; j < pB; j++) {
            for (int k = 0; k < mA; k++) {
                C[i * pB + j] += A[i * mA + k] * B[k * pB + j];
            }
        }
    }

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de multiplicação (dimensões %d x %d e %d x %d) com %d threads: %lf segundos\n", 
            nA, mA, mB, pB, nthreads, delta);

    // Finalização da temporização
    GET_TIME(inicio);

    // Escrita da matriz C no arquivo de saída
    escrever_matriz(argv[3], C, nA, pB);

    // Liberação da memória
    free(A);
    free(B);
    free(C);

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de finalização: %lf segundos\n", delta);

    delta = fim - inicio_t;
    printf("Tempo de execucao: %lf segundos\n", delta);

    printf("-- Programa concluído com sucesso --\n");
    return 0;
}
