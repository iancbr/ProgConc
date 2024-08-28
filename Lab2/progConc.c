// Ian de Andrade Camargo
// DRE_118089205
// Laboratório 2 - Atividade 3 - Concorrente

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

// Variáveis globais
long int n;      // Tamanho do vetor
float *vet;      // Vetor de elementos A
float *vetb;     // Vetor de elementos B
int nthreads;    // Número de threads

// Função executada pelas threads
// Estratégia de divisão de tarefas: blocos de n/nthreads elementos
void *MultVetor(void *tid) {
    long int id = (long int) tid; // Identificador da thread
    int ini, fim, bloco;          // Auxiliares para divisão do vetor em blocos
    float prod_local = 0.0, *ret; // Produto local

    bloco = n / nthreads; // Tamanho do bloco de dados de cada thread
    ini = id * bloco;     // Posição inicial do vetor
    fim = ini + bloco;    // Posição final do vetor
    if (id == (nthreads - 1)) fim = n; // A última thread trata os elementos restantes no caso de divisão não exata

    // Produto vetorial
    for (int i = ini; i < fim; i++) {
        prod_local += vet[i] * vetb[i];
    }

    // Retorna o resultado do produto vetorial
    ret = malloc(sizeof(float));
    if (ret != NULL) *ret = prod_local;
    else printf("--ERRO: malloc() thread\n");
    pthread_exit((void*) ret);
}

// Calcula a variação relativa entre o programa concorrente e sequencial
double varRel(double prod_seq, double prod_conc) {
    return sqrt(pow((prod_seq - prod_conc) / prod_seq, 2));
}

// Função principal do programa
int main(int argc, char *argv[]) {
    FILE *arq;           // Arquivo de entrada
    size_t ret;          // Retorno da função de leitura no arquivo de entrada
    double prod_ori;     // Produto interno registrado no arquivo
    float prod_conc = 0.0; // Resultado do produto interno calculado pelas threads
    float *prod_retorno_threads; // Auxiliar para retorno das threads

    pthread_t *tid_sistema; // Vetor de identificadores das threads no sistema

    // Valida e recebe os valores de entrada
    if (argc < 3) {
        printf("Use: %s <arquivo de entrada> <numero de threads>\n", argv[0]);
        exit(-1);
    }

    // Abre o arquivo de entrada com os valores para serem somados
    arq = fopen(argv[1], "rb");
    if (arq == NULL) {
        printf("--ERRO: fopen()\n");
        exit(-1);
    }

    // Lê o tamanho do vetor (primeira linha do arquivo)
    ret = fread(&n, sizeof(long int), 1, arq);
    if (!ret) {
        fprintf(stderr, "Erro de leitura das dimensões do vetor no arquivo \n");
        return 3;
    }

    // Aloca espaço de memória e carrega os vetores de entrada
    vet = malloc(sizeof(float) * n);
    if (vet == NULL) {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }
    ret = fread(vet, sizeof(float), n, arq);
    if (ret < n) {
        fprintf(stderr, "Erro de leitura dos elementos do vetor\n");
        return 4;
    }

    vetb = malloc(sizeof(float) * n);
    if (vetb == NULL) {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }
    ret = fread(vetb, sizeof(float), n, arq);
    if (ret < n) {
        fprintf(stderr, "Erro de leitura dos elementos do vetor B\n");
        return 4;
    }

    // Lê o número de threads da entrada do usuário
    nthreads = atoi(argv[2]);
    // Limita o número de threads ao tamanho do vetor
    if (nthreads > n) nthreads = n;

    // Aloca espaço para o vetor de identificadores das threads no sistema
    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if (tid_sistema == NULL) {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }

    // Cria as threads
    for (long int i = 0; i < nthreads; i++) {
        if (pthread_create(&tid_sistema[i], NULL, MultVetor, (void*) i)) {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    // Espera todas as threads terminarem e calcula o produto vetorial total
    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(tid_sistema[i], (void *) &prod_retorno_threads)) {
            printf("--ERRO: pthread_join()\n");
            exit(-1);
        }
        prod_conc += *prod_retorno_threads;
        free(prod_retorno_threads);
    }

    // Lê o produto interno registrado no arquivo
    ret = fread(&prod_ori, sizeof(double), 1, arq);

    // Imprime os resultados
    printf("\nProduto interno (concorrente) = %.26f\n", prod_conc);
    printf("Produto interno (sequencial)  = %.26lf\n", prod_ori);
    printf("Variação relativa             = %.26lf\n", varRel(prod_ori, prod_conc));

    // Desaloca os espaços de memória
    free(vet);
    free(vetb);
    free(tid_sistema);

    // Fecha o arquivo
    fclose(arq);
    return 0;
}

