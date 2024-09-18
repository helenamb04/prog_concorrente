#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

/*variaveis globais*/
pthread_mutex_t mutex;
pthread_cond_t cond;
long int soma = 0;
int impresso = 0;
int terminou = 0;

//função executada pela thread principal
void *ExecutaTarefa (void *arg) {
    long int id = (long int) arg;
    printf("Thread principal está executando...\n");

    while (soma <= 19*10) {
        pthread_mutex_lock(&mutex);

        // pausa se o valor atual for múltiplo de 10 e não foi impresso ainda
        while (soma % 10 == 0 && soma > 0 && !impresso) {
            pthread_cond_wait(&cond, &mutex); // espera o sinal da thread extra
        }

        soma++; // incrementa soma
        impresso = 0; // reseta o flag para a próxima vez que for múltiplo de 10

        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    terminou = 1; // thread  principal terminou
    pthread_cond_signal(&cond); // manda o sinal para a thread extra
    pthread_mutex_unlock(&mutex);

    printf("Thread principal terminou!\n");
    pthread_exit(NULL);
}

//função executada pela thread extra
void *extra (void *args) {
    printf("Extra: está executando...\n");
    printf("soma = 0\n");

    while (!terminou) { // executa enquanto a soma não atingir o limite
        pthread_mutex_lock(&mutex);

        if (soma % 10 == 0 && !impresso && soma > 0) {
            printf("soma = %ld\n", soma);
            impresso = 1; // flag indicando que imprimiu
            pthread_cond_signal(&cond); // manda o sinal para a thread principal
        }

        pthread_mutex_unlock(&mutex);
    }

    printf("Thread extra terminou!\n");
    pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
    pthread_t tid[2]; //identificadores das duas threads

    // inicializa o mutex e a variável de condição
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // cria a thread principal
    pthread_create(&tid[0], NULL, ExecutaTarefa, (void *)0);

    // cria a thread extra
    pthread_create(&tid[1], NULL, extra, NULL);

    // espera ambas as threads terminarem
    for (int t = 0; t < 2; t++) {
        pthread_join(tid[t], NULL);
    }

    // finaliza o mutex e a variável de condição
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
