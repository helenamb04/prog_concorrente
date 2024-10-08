//Programa concorrente que cria e faz operacoes sobre uma lista de inteiros

#include <stdio.h>
#include <stdlib.h>
#include "list_int.h"
#include <pthread.h>
#include "timer.h"

#define QTDE_OPS 3 //quantidade de operacoes sobre a lista por thread
#define QTDE_INI 100 //quantidade de insercoes iniciais na lista
#define MAX_VALUE 100 //valor maximo a ser inserido

//lista compartilhada iniciada 
struct list_node_s* head_p = NULL; 
//qtde de threads no programa
int nthreads;
//lock de exclusao mutua e variavel de condicao
pthread_mutex_t mutex;
pthread_cond_t cond;
//variaveis para complementar a logica do programa
int leitores_ativos = 0;
int escritores_esperando = 0;
int escrevendo = 0;

//funcoes de entrada e saida de leitores e escritores com mensagens de
//log para demonstrar que a prioridade de escrita esta sendo atendida

void EntraLeitura(int id) {
    pthread_mutex_lock(&mutex);
    while (escritores_esperando > 0 || escrevendo) {
        printf("Thread %d: Quero ler mas tem %d escritor(es) na frente\n", id, escritores_esperando);
        pthread_cond_wait(&cond, &mutex);
    }
    leitores_ativos++;
    pthread_mutex_unlock(&mutex);
}

void SaiLeitura(int id) {
    pthread_mutex_lock(&mutex);
    printf("Thread %d: Terminei de ler\n", id);
    leitores_ativos--;
    if (leitores_ativos == 0 && escritores_esperando > 0) {
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex);
}

void EntraEscrita(int id) {
    pthread_mutex_lock(&mutex);
    escritores_esperando++;
    while (leitores_ativos > 0 || escrevendo) {
        if (escrevendo){printf("Thread %d: Quero escrever mas já estão escrevendo\n", id);}
        else {printf("Thread %d: Quero escrever mas preciso esperar %d leitor(es)\n", id, leitores_ativos);}
        pthread_cond_wait(&cond, &mutex);
    }
    escrevendo = 1;
    pthread_mutex_unlock(&mutex);
}

void SaiEscrita(int id) {
    pthread_mutex_lock(&mutex);
    printf("Thread %d: Terminei de escrever\n", id);
    escrevendo = 0;
    escritores_esperando--;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}

//funcoes das threads
void *leitor(void *arg) {
    long int id = (long int) arg;
    int read = 0;

    for(int i=0; i<QTDE_OPS; i++) {
        EntraLeitura(id);
        Member(id+1 % MAX_VALUE, head_p); // Ignore return value
        SaiLeitura(id);
        read++;
    }

    pthread_exit(NULL);
}

void *escritor(void *arg) {
    long int id = (long int) arg;
    int in = 0, out = 0;

    for(int i=0; i<QTDE_OPS; i++) {
        EntraEscrita(id);
        if (rand() % 2) {
            Insert(id % MAX_VALUE, &head_p); // Ignore return value
            in++;
        } else {
            Delete(id % MAX_VALUE, &head_p); // Ignore return value
            out++;
        }
        SaiEscrita(id);
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    pthread_t *tid;
    double ini, fim, delta;
    int op, write, read; 
    write=read = 0; 
   
    //verifica se o numero de threads foi passado na linha de comando
    if(argc<2) {
        printf("Digite: %s <numero de threads>\n", argv[0]); return 1;
    }
    nthreads = atoi(argv[1]);

    //insere os primeiros elementos na lista
    for(int i=0; i<QTDE_INI; i++)
        Insert(i%MAX_VALUE, &head_p);  /* Ignore return value */
    
    //aloca espaco de memoria para o vetor de identificadores de threads no sistema
    tid = malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {  
        printf("--ERRO: malloc()\n"); return 2;
    }

    //inicializa o mutex e a condição
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    //tomada de tempo inicial
    GET_TIME(ini);

    //realiza operacoes de leitura (92%) e escrita (8%)
    for(long int i=0; i<nthreads; i++) {
        op = rand() % 100;
        if(op<92) {
            pthread_create(tid+i, NULL, leitor, (void*) i);
	        read++;
        } 
        else {
            pthread_create(tid+i, NULL, escritor, (void*) i);
            write++;
        }
    }
   
    //aguarda as threads terminarem
    for(int i=0; i<nthreads; i++) {
        if(pthread_join(*(tid+i), NULL)) {
            printf("--ERRO: pthread_join()\n"); return 4;
        }
    }

    //registra a qtde de operacoes realizadas por tipo
    printf("write=%d read=%d\n", write*QTDE_OPS, read*QTDE_OPS);

    //tomada de tempo final
    GET_TIME(fim);
    delta = fim-ini;
    printf("Tempo: %lf\n", delta);

    //libera o mutex e a condição
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
    //libera a memória
    free(tid);
    Free_list(&head_p);

    return 0;
}