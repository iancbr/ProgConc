// Ian de Andrade Camargo
// DRE_118089205
// Laboratório 3 - Concorrente

#include "timer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

float *matrizA;        // matriz de entrada A
float *matrizB;        // matriz de entrada B
float *saida;          // matriz de saída
int nthreads;          // número de threads
int linhasA, colunasA; // dimensões da matriz A
int linhasB, colunasB; // dimensões da matriz B

typedef struct {
  int id;  // identificador da thread
  int in;  // linha inicial que a thread processará
  int fim; // linha final que a thread processará
} tArgs;

// Função que será executada pelas threads para multiplicar as matrizes
void *mult(void *arg) {
  tArgs *args = (tArgs *)arg;

  // Multiplicação da matriz A pela matriz B
  for (int i = args->in; i < args->fim; i++) {
    for (int j = 0; j < colunasB; j++) {
      for (int k = 0; k < colunasA; k++) {
        saida[i * colunasB + j] +=
            matrizA[i * colunasA + k] * matrizB[k * colunasB + j];
      }
    }
  }

  pthread_exit(NULL);
}

// Função principal
int main(int argc, char *argv[]) {
  long long int tamA, tamB; // quantidade de elementos nas matrizes A e B
  FILE *arqA, *arqB, *arqS; // descritores de arquivos de entrada e saída
  size_t retA, retB, ret; // retorno da função de leitura e escrita de arquivos
  pthread_t *tid;         // identificadores das threads
  tArgs *args;            // argumentos para as threads
  double inicio, fim, delta; // variáveis para medir o tempo de execução

  // Verifica se os parâmetros de entrada estão corretos
  if (argc < 5) {
    printf("Digite: %s <arquivos de entrada A e B> <arquivo de saída> <numero "
           "de threads>\n",
           argv[0]);
    return 1;
  }

  // Lê o número de threads a serem criadas
  nthreads = atoi(argv[4]);

  GET_TIME(inicio); // Tempo de início da inicialização

  // Abre o arquivo para leitura binária da matriz A
  arqA = fopen(argv[1], "rb");
  if (!arqA) {
    fprintf(stderr, "Erro de abertura do arquivo A\n");
    return 2;
  }

  // Abre o arquivo para leitura binária da matriz B
  arqB = fopen(argv[2], "rb");
  if (!arqB) {
    fprintf(stderr, "Erro de abertura do arquivo B\n");
    return 2;
  }

  // Lê as dimensões da matriz A
  retA = fread(&linhasA, sizeof(int), 1, arqA);
  if (retA != 1) {
    fprintf(stderr, "Erro de leitura das dimensões da matriz A\n");
    return 3;
  }
  retA = fread(&colunasA, sizeof(int), 1, arqA);
  if (retA != 1) {
    fprintf(stderr, "Erro de leitura das dimensões da matriz A\n");
    return 3;
  }
  tamA = linhasA * colunasA;

  // Aloca memória para a matriz A
  matrizA = (float *)malloc(sizeof(float) * tamA);
  if (matrizA == NULL) {
    fprintf(stderr, "Erro de alocação de memória para a matriz A\n");
    return 4;
  }
  retA = fread(matrizA, sizeof(float), tamA, arqA);
  if (retA != tamA) {
    fprintf(stderr, "Erro de leitura dos elementos da matriz A\n");
    return 3;
  }

  // Lê as dimensões da matriz B
  retB = fread(&linhasB, sizeof(int), 1, arqB);
  if (retB != 1) {
    fprintf(stderr, "Erro de leitura das dimensões da matriz B\n");
    return 3;
  }
  retB = fread(&colunasB, sizeof(int), 1, arqB);
  if (retB != 1) {
    fprintf(stderr, "Erro de leitura das dimensões da matriz B\n");
    return 3;
  }
  tamB = linhasB * colunasB;

  // Aloca memória para a matriz B
  matrizB = (float *)malloc(sizeof(float) * tamB);
  if (matrizB == NULL) {
    fprintf(stderr, "Erro de alocação de memória para a matriz B\n");
    return 4;
  }
  retB = fread(matrizB, sizeof(float), tamB, arqB);
  if (retB != tamB) {
    fprintf(stderr, "Erro de leitura dos elementos da matriz B\n");
    return 3;
  }

  // Aloca memória para a matriz de saída
  int tamS = linhasA * colunasB;
  saida = (float *)malloc(sizeof(float) * tamS);
  if (saida == NULL) {
    fprintf(stderr, "Erro de alocação de memória para a matriz de saída\n");
    return 4;
  }

  GET_TIME(fim); // Tempo de fim da inicialização
  delta = fim - inicio;
  printf("Tempo inicialização: %lf\n", delta);

  GET_TIME(inicio); // Tempo de início da multiplicação

  // Criação das threads
  tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
  args = (tArgs *)malloc(sizeof(tArgs) * nthreads);
  int pedaco = linhasA / nthreads;

  for (int i = 0; i < nthreads; i++) {
    (args + i)->id = i;
    (args + i)->in = i * pedaco;
    (args + i)->fim = (i + 1) * pedaco;
    if (i == nthreads - 1) {
      (args + i)->fim = linhasA;
    }

    pthread_create(tid + i, NULL, mult, (void *)(args + i));
  }

  // Espera o término das threads
  for (int i = 0; i < nthreads; i++) {
    pthread_join(*(tid + i), NULL);
  }

  GET_TIME(fim); // Tempo de fim da multiplicação
  delta = fim - inicio;
  printf("Tempo multiplicação (nthreads %d): %lf\n", nthreads, delta);

  GET_TIME(inicio); // Tempo de início da finalização

  // Abre o arquivo para escrita binária do resultado
  arqS = fopen(argv[3], "wb");
  if (!arqS) {
    fprintf(stderr, "Erro de abertura do arquivo de saída\n");
    return 5;
  }
  fwrite(&linhasA, sizeof(int), 1, arqS);
  fwrite(&colunasB, sizeof(int), 1, arqS);
  fwrite(saida, sizeof(float), tamS, arqS);

  // Libera a memória alocada
  free(matrizA);
  free(matrizB);
  free(saida);
  free(args);
  free(tid);

  GET_TIME(fim); // Tempo de fim da finalização
  delta = fim - inicio;
  printf("Tempo finalização: %lf\n", delta);

  return 0;
}

