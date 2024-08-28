//Ian de Andrade Camargo
//DRE_118089205
//Laboratório 2 - Atividade 3 - Sequencial

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Definições
#define MAX 1000  // valor máximo de um elemento do vetor

// Calcula o produto interno de dois vetores
double prodVet(float *vet1, float *vet2, long int N) {
    double soma = 0.0;
    for (long int i = 0; i < N; i++) {
        soma += vet1[i] * vet2[i];
    }
    return soma;
}

// Cria e preenche um vetor com valores aleatórios
float* criaVetor(long int n) {
    float elem;
    int fator = 1;
    float* vetor = (float*)malloc(n * sizeof(float));
    
    if (vetor == NULL) {
        printf("Erro ao alocar memória para o vetor.\n");
        exit(1);
    }
    
    // Preenche o vetor com valores float aleatórios
    for (long int i = 0; i < n; i++) {
        elem = (rand() % MAX) / 3.0 * fator;
        vetor[i] = elem;
        fator *= -1;
    }
    return vetor;
}

int main(int argc, char* argv[]) {
    long int n;  // quantidade de elementos do vetor
    FILE *descritorArquivo;  // descritor do arquivo de saída
    size_t ret;  // retorno da função de escrita no arquivo

    // Recebe argumentos de entrada
    if (argc < 3) {
        printf("Uso: %s <dimensao> <arquivo saida>\n", argv[0]);
        return 1;
    }

    n = atol(argv[1]);
    float *vetor = criaVetor(n);  // criando o vetor A
    float *vetorb = criaVetor(n); // criando o vetor B

    // Abre o arquivo p/ escrita binária
    descritorArquivo = fopen(argv[2], "wb");
    if (descritorArquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        free(vetor);
        free(vetorb);
        return 3;
    }

    // Escreve a dimensão N no arquivo binário
    ret = fwrite(&n, sizeof(long int), 1, descritorArquivo);
    if (ret != 1) {
        printf("Erro ao escrever a dimensão no arquivo.\n");
        fclose(descritorArquivo);
        free(vetor);
        free(vetorb);
        return 4;
    }

    // Escreve os elementos do vetor A no arquivo binário
    ret = fwrite(vetor, sizeof(float), n, descritorArquivo);
    if (ret != n) {
        printf("Erro ao escrever os elementos do vetor A no arquivo.\n");
        fclose(descritorArquivo);
        free(vetor);
        free(vetorb);
        return 5;
    }

    // Escreve os elementos do vetor B no arquivo binário
    ret = fwrite(vetorb, sizeof(float), n, descritorArquivo);
    if (ret != n) {
        printf("Erro ao escrever os elementos do vetor B no arquivo.\n");
        fclose(descritorArquivo);
        free(vetor);
        free(vetorb);
        return 6;
    }

    // Calcula o produto interno dos vetores
    double prod = prodVet(vetor, vetorb, n);
    printf("Produto interno: %.4lf\n", prod);

    // Escreve o resultado do produto interno no arquivo binário
    ret = fwrite(&prod, sizeof(double), 1, descritorArquivo);
    if (ret != 1) {
        printf("Erro ao escrever o produto interno no arquivo.\n");
        fclose(descritorArquivo);
        free(vetor);
        free(vetorb);
        return 7;
    }

    // Fecha o arquivo e libera a memória
    fclose(descritorArquivo);
    free(vetor);
    free(vetorb);
    return 0;
}

