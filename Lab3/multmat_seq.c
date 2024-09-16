// Ian de Andrade Camargo
// DRE_118089205
// Laboratório 3 - Sequencial

#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

// Função que calcula a multiplicação de 2 matrizes
float *multMat(int linhasA, int colunasA, float *matrizA, int linhasB,
               int colunasB, float *matrizB) {
  // Verifica se a multiplicação é possível
  if (colunasA != linhasB) {
    return NULL; // Dimensões incompatíveis
  }

  // Aloca memória para a matriz de saída e inicializa com zeros
  float *res = (float *)calloc(linhasA * colunasB, sizeof(float));
  if (!res) {
    return NULL; // Erro de alocação
  }

  // Calcula a multiplicação das matrizes
  for (int i = 0; i < linhasA; i++) {
    for (int j = 0; j < colunasB; j++) {
      for (int k = 0; k < colunasA; k++) {
        res[i * colunasB + j] +=
            matrizA[i * colunasA + k] * matrizB[k * colunasB + j];
      }
    }
  }

  return res;
}

// Função principal
int main(int argc, char *argv[]) {
  float *matrizA, *matrizB;                 // Matrizes A e B
  int linhasA, colunasA, linhasB, colunasB; // Dimensões das matrizes
  long long int tamA, tamB;  // Tamanho de elementos nas matrizes A e B
  FILE *arqA, *arqB, *arqS;  // Arquivos para leitura e escrita
  size_t retA, retB, ret;    // Retorno das funções de leitura/escrita
  double inicio, fim, delta; // Monitoramento de tempo

  // Verifica os argumentos de entrada
  if (argc < 4) {
    fprintf(stderr, "Uso: %s <arquivo A> <arquivo B> <arquivo de saída>\n",
            argv[0]);
    return 1;
  }

  // Inicia a contagem de tempo da inicialização
  GET_TIME(inicio);

  // Abre o arquivo da matriz A
  arqA = fopen(argv[1], "rb");
  if (!arqA) {
    fprintf(stderr, "Erro ao abrir o arquivo A\n");
    return 2;
  }

  // Lê as dimensões da matriz A
  retA = fread(&linhasA, sizeof(int), 1, arqA);
  retA = fread(&colunasA, sizeof(int), 1, arqA);
  if (retA < 1) {
    fprintf(stderr, "Erro ao ler dimensões da matriz A\n");
    fclose(arqA);
    return 3;
  }

  tamA = linhasA * colunasA; // Calcula o tamanho da matriz A
  matrizA =
      (float *)malloc(tamA * sizeof(float)); // Aloca memória para a matriz A
  if (!matrizA) {
    fprintf(stderr, "Erro de alocação para a matriz A\n");
    fclose(arqA);
    return 4;
  }

  // Carrega os elementos da matriz A
  retA = fread(matrizA, sizeof(float), tamA, arqA);
  if (retA < tamA) {
    fprintf(stderr, "Erro ao ler elementos da matriz A\n");
    fclose(arqA);
    free(matrizA);
    return 5;
  }
  fclose(arqA);

  // Abre o arquivo da matriz B
  arqB = fopen(argv[2], "rb");
  if (!arqB) {
    fprintf(stderr, "Erro ao abrir o arquivo B\n");
    free(matrizA);
    return 6;
  }

  // Lê as dimensões da matriz B
  retB = fread(&linhasB, sizeof(int), 1, arqB);
  retB = fread(&colunasB, sizeof(int), 1, arqB);
  if (retB < 1) {
    fprintf(stderr, "Erro ao ler dimensões da matriz B\n");
    fclose(arqB);
    free(matrizA);
    return 7;
  }

  tamB = linhasB * colunasB; // Calcula o tamanho da matriz B
  matrizB =
      (float *)malloc(tamB * sizeof(float)); // Aloca memória para a matriz B
  if (!matrizB) {
    fprintf(stderr, "Erro de alocação para a matriz B\n");
    fclose(arqB);
    free(matrizA);
    return 8;
  }

  // Carrega os elementos da matriz B
  retB = fread(matrizB, sizeof(float), tamB, arqB);
  if (retB < tamB) {
    fprintf(stderr, "Erro ao ler elementos da matriz B\n");
    fclose(arqB);
    free(matrizA);
    free(matrizB);
    return 9;
  }
  fclose(arqB);

  // Tempo final da inicialização
  GET_TIME(fim);
  delta = fim - inicio;
  printf("Tempo inicialização: %lf\n", delta);

  // Tempo da multiplicação
  GET_TIME(inicio);
  float *M = multMat(linhasA, colunasA, matrizA, linhasB, colunasB, matrizB);
  if (!M) {
    fprintf(stderr, "Erro: Dimensões incompatíveis para multiplicação\n");
    free(matrizA);
    free(matrizB);
    return 10;
  }
  GET_TIME(fim);
  delta = fim - inicio;
  printf("Tempo multiplicação: %lf\n", delta);

  // Tempo de início da escrita
  GET_TIME(inicio);
  arqS = fopen(argv[3], "wb");
  if (!arqS) {
    fprintf(stderr, "Erro ao abrir o arquivo de saída\n");
    free(matrizA);
    free(matrizB);
    free(M);
    return 11;
  }

  // Escreve as dimensões e os elementos da matriz resultado
  ret = fwrite(&linhasA, sizeof(int), 1, arqS);
  ret = fwrite(&colunasB, sizeof(int), 1, arqS);
  ret = fwrite(M, sizeof(float), linhasA * colunasB, arqS);
  fclose(arqS);

  // Tempo final da escrita
  GET_TIME(fim);
  delta = fim - inicio;
  printf("Tempo finalização: %lf\n", delta);

  // Libera a memória alocada
  free(matrizA);
  free(matrizB);
  free(M);

  return 0;
}

