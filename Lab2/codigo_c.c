/* Helena Marques Batista - 122071866 */
/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratorio: 2 */
/* Codigo: Divide vetor entre threads, calcula produto interno e variacao relativa */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
    float *vetor1, *vetor2;  // ponteiros para os vetores
    int inicio;              // índice inicial do vetor
    int final;               // índice final do vetor
} t_Args;

//cria a estrutura de dados de retorno da thread
typedef struct {
    float soma_parcial;
} t_Ret;

// Funcao que calcula a variacao relativa do resultado
void VariacaoRelativa(double pi_thread, double pi_arquivo) {
    float variacao_relativa = (float) ((pi_thread - pi_arquivo) / pi_arquivo);
    printf("Variacao relativa: %10f\n", variacao_relativa);
}

//funcao executada pelas threads
void *ProdutoInterno (void *arg) {
    t_Args *args = (t_Args *) arg;
    t_Ret *ret = malloc(sizeof(t_Ret));
    if (ret == NULL) {
        printf("--ERRO: malloc() na thread\n");
        pthread_exit(NULL);
    }

    ret->soma_parcial = 0;
    for (int i = args->inicio; i <= args->final; i++) {
        ret->soma_parcial += args->vetor1[i] * args->vetor2[i];
    }

    free(arg); //libera a alocacao feita na main
    pthread_exit((void*) ret);
}

//funcao principal do programa
int main(int argc, char* argv[]) {
    t_Args *args;    //recebera os argumentos para a thread
    t_Ret *retorno;  //recebera o retorno das threads

    int mthreads; //qtde de threads que serao criadas (recebida na linha de comando)
    int nelementos; //qtde de elementos dos vetores (lida no arquivo)
    double pi_arquivo; //produto interno lido do arquivo
    
    FILE *arq; //arquivo de entrada
    size_t ret; //retorno da funcao de leitura no arquivo de entrada

    //verifica se o argumento 'qtde de threads' e o arquivo foram passados e os armazena
    if(argc < 3) {
        printf("--ERRO: informe a qtde de threads e o nome do arquivo <%s> <mthreads> <arquivo>\n", argv[0]);
        return 1;
    }
    mthreads = atoi(argv[1]);

    arq = fopen(argv[2], "rb");
    if(arq == NULL) {
        printf("--ERRO: fopen()\n"); 
        return 1;
    }

    //le o tamanho dos vetores (primeira linha do arquivo)
    ret = fread(&nelementos, sizeof(long int), 1, arq);
    if(!ret) {
        fprintf(stderr, "Erro de leitura das dimensoes dos vetores no arquivo \n");
        return 3;
    }

    // Aloca memoria para os vetores
    float *vetor1 = malloc(nelementos * sizeof(float));
    float *vetor2 = malloc(nelementos * sizeof(float));
    if (vetor1 == NULL || vetor2 == NULL) {
        printf("--ERRO: malloc() para vetores\n");
        return 1;
    }

    // Le os vetores do arquivo
    ret = fread(vetor1, sizeof(float), nelementos, arq);
    if(ret < nelementos) {
        fprintf(stderr, "Erro de leitura dos elementos do vetor1 no arquivo\n");
        return 3;
    }
    
    ret = fread(vetor2, sizeof(float), nelementos, arq);
    if(ret < nelementos) {
        fprintf(stderr, "Erro de leitura dos elementos do vetor2 no arquivo\n");
        return 3;
    }

    //le o produto interno calculado pelo programa sequencial (segunda linha do arquivo)
    ret = fread(&pi_arquivo, sizeof(double), 1, arq);
    if(!ret) {
        fprintf(stderr, "Erro de leitura do produto interno no arquivo \n");
        return 3;
    }
    
    fclose(arq);

    // Verifica se M <= N
    if(mthreads > nelementos) {
        printf("--ERRO: O numero de threads (M) não pode ser maior que o numero de elementos (N)\n");
        return 1;
    }

    //identificadores das threads no sistema
    pthread_t tid_sistema[mthreads];

    //divide o vetor entre as threads
    int elementos_por_thread = nelementos / mthreads;
    int resto = nelementos % mthreads;
    int inicio = 0;
    double pi_thread = 0.0;

    //cria as threads
    for(int i = 0; i < mthreads; i++) {
        printf("--Aloca e preenche argumentos para thread %d\n", i+1);
        args = malloc(sizeof(t_Args));
        if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            return 1;
        }
        args->vetor1 = vetor1;
        args->vetor2 = vetor2;
        args->inicio = inicio;
        args->final = inicio + elementos_por_thread - 1 + (i < resto ? 1 : 0);
        inicio = args->final + 1;

        printf("--Cria a thread %d\n", i+1);
        if (pthread_create(&tid_sistema[i], NULL, ProdutoInterno, (void*) args)) {
            printf("--ERRO: pthread_create()\n"); 
            return 2;
        }
    }

    //espera todas as threads terminarem e soma os resultados
    for (int i = 0; i < mthreads; i++) {
        if (pthread_join(tid_sistema[i], (void**) &retorno)) {
            printf("--ERRO: pthread_join() da thread %d\n", i+1); 
        }
        pi_thread += retorno->soma_parcial;
        free(retorno); //libera a memória alocada na thread
    }

    // calcula a variacao relativa
    VariacaoRelativa(pi_thread, pi_arquivo);

    // limpa a memoria alocada para os vetores
    free(vetor1);
    free(vetor2);

    //log da funcao principal
    printf("--Thread principal terminou\n");

    return 0;
}
