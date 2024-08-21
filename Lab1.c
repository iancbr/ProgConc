//Ian de Andrade Camargo
//DRE_118089205
//Laboratório 1 - Atividade 5

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Estrutura que será usada para passar o vetor e o id da thread
typedef struct {
    int *vector;
    int id;
    int N;
    int M;
} ThreadData;

// Função executada pelas threads
void *increment_elements(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int id = data->id;
    int N = data->N;
    int M = data->M;
    int elements_per_thread = N / M;
    int remainder = N % M;

    // Calcular a faixa de trabalho da thread
    int start = id * elements_per_thread + (id < remainder ? id : remainder);
    int end = start + elements_per_thread + (id < remainder ? 1 : 0);

    for (int i = start; i < end; i++) {
        data->vector[i] += 1;
    }
    pthread_exit(NULL);
}

// Função para inicializar o vetor
void initialize_vector(int *vector, int N) {
    for (int i = 0; i < N; i++) {
        vector[i] = i; // Inicializa o vetor com valores de 0 a N-1
    }
}

// Função para verificar se o resultado está correto
void check_result(int *vector, int N) {
    for (int i = 0; i < N; i++) {
        if (vector[i] != i + 1) {
            printf("Erro na posição %d: esperado %d, mas encontrou %d\n", i, i + 1, vector[i]);
            return;
        }
    }
    printf("Resultado correto!\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <numero de threads> <tamanho do vetor>\n", argv[0]);
        return -1;
    }

    int M = atoi(argv[1]); // Número de threads
    int N = atoi(argv[2]); // Tamanho do vetor

    int *vector = (int *)malloc(N * sizeof(int));
    pthread_t *threads = (pthread_t *)malloc(M * sizeof(pthread_t));
    ThreadData *thread_data = (ThreadData *)malloc(M * sizeof(ThreadData));

    // Inicializar o vetor
    initialize_vector(vector, N);

    // Criar e executar as threads
    for (int i = 0; i < M; i++) {
        thread_data[i].vector = vector;
        thread_data[i].id = i;
        thread_data[i].N = N;
        thread_data[i].M = M;
        pthread_create(&threads[i], NULL, increment_elements, (void *)&thread_data[i]);
    }

    // Esperar todas as threads terminarem
    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }

    // Verificar se o resultado está correto
    check_result(vector, N);

    // Liberar a memória alocada
    free(vector);
    free(threads);
    free(thread_data);

    return 0;
}
