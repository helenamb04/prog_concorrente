/* Helena Marques Batista - 122071866 */
/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratorio: 1 */
/* Codigo: Divide vetor entre threads e soma 1 em cada elemento do vetor */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
   int *vetor;     // ponteiro para o vetor
   int inicio;     // indice inicial do vetor
   int final;      // indice final do vetor
} t_Args;

// funcao que inicializa o vetor com valores aleatorios
void InicializaVetor(int *vetor, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = rand() % 100; // valores aleatorios entre 0 e 99
    }
}

// Funcao que checa se todos os elementos foram incrementados corretamente
void ChecaVetor(int *vetor, int tamanho) {
    int erro = 0;
    for (int i = 0; i < tamanho; i++) {
        if (vetor[i] != (vetor[i] - 1) + 1) {
            erro = 1;
            printf("--ERRO: Elemento %d do vetor esta incorreto.", i);
        }
    }

    if (!erro) {
        printf("--Todos os elementos foram incrementados corretamente!\n");
    }
}

//funcao executada pelas threads
void *SomaUm (void *arg) {
    t_Args *args = (t_Args *) arg;

    // incrementa cada elemento do vetor no intervalo
    for (int i = args->inicio; i <= args->final; i++) {
        args->vetor[i] += 1;
    }

    free(arg); //libera a alocacao feita na main

    pthread_exit(NULL);
}

//funcao principal do programa
int main(int argc, char* argv[]) {
    t_Args *args; //recebera os argumentos para a thread

    int mthreads, nelementos; //qtde de threads que serao criadas e elementos no vetor (recebidas na linha de comando)

    //verifica se o argumento 'qtde de threads' foi passado e armazena seu valor
    if(argc<3) {
        printf("--ERRO: informe a qtde de threads e elementos do vetor <%s> <mthreads> <nelementos>\n", argv[0]);
        return 1;
    }
    mthreads = atoi(argv[1]);
    nelementos = atoi(argv[2]);

    // Verifica se M <= N
    if(mthreads > nelementos) {
        printf("--ERRO: O numero de threads (M) não pode ser maior que o numero de elementos (N)\n");
        return 1;
    }

    // inicializa o vetor com valores aleatorios
    int *vetor = malloc(nelementos * sizeof(int));
    if (vetor == NULL) {
        printf("--ERRO: malloc() para vetor\n");
        return 1;
    }
    
    InicializaVetor(vetor, nelementos);

    //identificadores das threads no sistema
    pthread_t tid_sistema[mthreads];

    //divide o vetor entre as threads
    int resultado = nelementos / mthreads;
    int resto = nelementos % mthreads;
    int inicio = 0;

    //cria as threads
    for(int i = 0; i < mthreads; i++) {
        printf("--Aloca e preenche argumentos para thread %d\n", i+1);
        args = malloc(sizeof(t_Args));
        if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            return 1;
        }
        args->vetor = vetor;
        args->inicio = inicio;
        args->final = inicio + resultado - 1 + (i < resto ? 1 : 0);
        inicio = args->final + 1;

        printf("--Cria a thread %d\n", i+1);
        if (pthread_create(&tid_sistema[i], NULL, SomaUm, (void*) args)) {
            printf("--ERRO: pthread_create()\n"); 
            return 2;
        }
    }

    //espera todas as threads terminarem
    for (int i = 0; i < mthreads; i++) {
        if (pthread_join(tid_sistema[i], NULL)) {
            printf("--ERRO: pthread_join() da thread %d\n", i+1); 
        } 
    }

    // checa se os valores estao certos
    ChecaVetor(vetor, nelementos);

    // limpa a memoria alocada para o vetor
    free(vetor);

    //log da funcao principal
    printf("--Thread principal terminou\n");

    return 0;
}
