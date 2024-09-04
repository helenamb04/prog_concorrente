/* 
 * Multiplicação de Matrizes Concorrente
 * Helena Marques Batista - 122071866
 * Disciplina: Programação Concorrente (ICP-361) 2024-2
 * Prof. Silvana Rossetto
 * Laboratório 3
 * Código concorrente
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

// Estrutura para os argumentos das threads
typedef struct {
    int id;         // Identificador da thread
    int nthreads;   // Número total de threads
    int n;          // Número de linhas da matriz A
    int m;          // Número de colunas da matriz A e linhas da matriz B
    int p;          // Número de colunas da matriz B
    float *A;       // Matriz A
    float *B;       // Matriz B
    float *C;       // Matriz C (resultado)
} tArgs;

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

// Função que cada thread executará
void *multiplica(void *arg) {
    tArgs *args = (tArgs *) arg;
    int id = args->id;
    int nthreads = args->nthreads;
    int n = args->n;
    int m = args->m;
    int p = args->p;
    float *A = args->A;
    float *B = args->B;
    float *C = args->C;

    // Distribuição das linhas da matriz C entre as threads
    int linhas_por_thread = n / nthreads;
    int resto = n % nthreads;
    int inicio = id * linhas_por_thread + (id < resto ? id : resto);
    int fim = inicio + linhas_por_thread - 1 + (id < resto ? 1 : 0);

    for(int i = inicio; i <= fim; i++) {
        for(int j = 0; j < p; j++) {
            C[i*p + j] = 0.0f;
            for(int k = 0; k < m; k++) {
                C[i*p + j] += A[i*m + k] * B[k*p + j];
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int nthreads;        // Número de threads
    int nA, mA;          // Dimensões da matriz A
    int mB, pB;          // Dimensões da matriz B
    float *A, *B, *C;    // Matrizes A, B e C
    pthread_t *tid;
    tArgs *args;
    double inicio, fim, inicio_t, delta;

    // Início da temporização da inicialização
    GET_TIME(inicio);
    inicio_t = inicio;

    // Verificação dos argumentos de entrada
    if(argc < 5) {
        fprintf(stderr, "Uso: %s <arquivo_A> <arquivo_B> <arquivo_C> <nthreads>\n", argv[0]);
        return 1;
    }

    // Leitura do número de threads
    nthreads = atoi(argv[4]);
    if(nthreads < 1) nthreads = 1;

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

    // Alocação das threads e argumentos
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(!tid) {
        fprintf(stderr, "Erro ao alocar memória para as threads\n");
        return 7;
    }

    args = (tArgs *) malloc(sizeof(tArgs) * nthreads);
    if(!args) {
        fprintf(stderr, "Erro ao alocar memória para os argumentos das threads\n");
        return 8;
    }

    // Criação das threads
    for(int i = 0; i < nthreads; i++) {
        args[i].id = i;
        args[i].nthreads = nthreads;
        args[i].n = nA;
        args[i].m = mA;
        args[i].p = pB;
        args[i].A = A;
        args[i].B = B;
        args[i].C = C;

        if(pthread_create(&tid[i], NULL, multiplica, (void *) &args[i])) {
            fprintf(stderr, "Erro ao criar a thread %d\n", i);
            return 9;
        }
    }

    // Espera pelo término das threads
    for(int i = 0; i < nthreads; i++) {
        pthread_join(tid[i], NULL);
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
    free(tid);
    free(args);

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de finalização: %lf segundos\n", delta);

    delta = fim - inicio_t;
    printf("Tempo de execucao: %lf segundos\n", delta);
    
    printf("-- Programa concluído com sucesso --\n");
    return 0;
}
