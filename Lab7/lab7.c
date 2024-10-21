#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define BUFFER_SIZE 10 // Tamanho do buffer1 e buffer2
#define MAX_LINES 11   // Número de linhas especiais com inserção de \n a cada 2n+1 caracteres

// Buffers compartilhados
char buffer1[BUFFER_SIZE];
char buffer2[BUFFER_SIZE];

// Variáveis de controle
int id1 = 0; // Índice para controle de leitura em buffer1
int id2 = 0; // Índice para controle de escrita em buffer2
int eof;     // Sinaliza final do arquivo

// Semáforos para sincronização entre as threads
sem_t sem_empty1, sem_full1; // Para controle do buffer1
sem_t sem_empty2, sem_full2; // Para controle do buffer2

// Função da thread 1: Lê caracteres do arquivo e coloca no buffer1
void *t1(void *arg) {
    FILE *input = fopen("entrada.txt", "r");
    if (input == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        pthread_exit(NULL);
    }

    while (1) {
        sem_wait(&sem_empty1); // Espera o buffer1 esvaziar
        
        id1 = 0; // Reseta o índice do buffer1 para a nova leitura

        // Lê um caractere de cada vez até ler 10 caracteres
        for (int i = 0; i < BUFFER_SIZE; i++) {
            int ch = fgetc(input);

            // Verifica se o arquivo chegou ao fim
            if (ch == EOF) {
                eof = 1;
                sem_post(&sem_full1);  // Sinaliza que o buffer1 está cheio
                fclose(input);
                pthread_exit(NULL);    // Finaliza a thread1
            }

            // Insere o caractere lido no buffer1
            buffer1[id1++] = ch;
        }

        sem_post(&sem_full1); // Sinaliza que o buffer1 está cheio
    }
}

// Função da thread 2: Processa o buffer1 e insere '\n' quando necessário
void *t2(void *arg) {
    int char_count = 0; // Contador de caracteres processados
    int line_count = 0; // Contador de linhas impressas

    while (1) {
        sem_wait(&sem_full1);  // Espera o buffer1 ficar cheio
        sem_wait(&sem_empty2); // Espera o buffer2 esvaziar

        for (int i = 0; i < id1; i++) {
            // Copia os caracteres de buffer1 para buffer2
            buffer2[id2++] = buffer1[i];
            char_count++;

            // Após 11 linhas, insere uma nova linha a cada 10 caracteres
            if (line_count >= MAX_LINES && char_count == BUFFER_SIZE) {
                buffer2[id2++] = '\n';
                char_count = 0;
            }

            // Regra: a cada 2n+1 caracteres, insere um '\n'
            else if (char_count == 2 * line_count + 1) {
                buffer2[id2++] = '\n';
                line_count++; // Incrementa o contador de linha global
                char_count = 0; // Reseta o contador de caracteres
            }
        }

        // Sinaliza que o buffer2 está cheio e buffer1 pode ser reutilizado
        sem_post(&sem_full2);
        sem_post(&sem_empty1);

        // Verifica se o arquivo chegou ao fim
        if (eof) {
            pthread_exit(NULL);
        }
    }
}

// Função da thread 3: Imprime o conteúdo do buffer2
void *t3(void *arg) {
    while (1) {
        sem_wait(&sem_full2); // Espera o buffer2 ficar cheio

        // Imprime os caracteres do buffer2
        for (int i = 0; i < id2; i++) {
            putchar(buffer2[i]);
        }

        id2 = 0; // Reseta o índice do buffer2 para a próxima iteração
        sem_post(&sem_empty2); // Sinaliza que o buffer2 foi esvaziado

        // Verifica se o arquivo chegou ao fim
        if (eof) {
            pthread_exit(NULL);
        }
    }
}

int main() {
    pthread_t tid[3];

    // Inicializa os semáforos
    sem_init(&sem_empty1, 0, 1); // Buffer1 começa vazio
    sem_init(&sem_full1, 0, 0);
    sem_init(&sem_empty2, 0, 1); // Buffer2 começa vazio
    sem_init(&sem_full2, 0, 0);

    // Cria as threads
    pthread_create(&tid[0], NULL, t1, NULL);
    pthread_create(&tid[1], NULL, t2, NULL);
    pthread_create(&tid[2], NULL, t3, NULL);

    // Aguarda as threads finalizarem
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);

    // Destrói os semáforos
    sem_destroy(&sem_empty1);
    sem_destroy(&sem_full1);
    sem_destroy(&sem_empty2);
    sem_destroy(&sem_full2);

    return 0;
}
