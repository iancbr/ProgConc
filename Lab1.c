//Ian de Andrade Camargo
//DRE_118089205
//Laboratório 1 - Atividade 5

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Estrutura para passar múltiplos argumentos para a função da thread
typedef struct {
    int *vector;
    int start;
    int end;
} ThreadData;

// Função executada pelas threads
void *increment_elements(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    for (int i = data->start; i < data->end; i++) {
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
    int correct = 1;
    for (int i = 0; i < N; i++) {
        if (vector[i] != i + 1) {
            correct = 0;
            break;
        }
    }
    if (correct) {
        printf("Resultado correto!\n");
    } else {
        printf("Erro no resultado.\n");
    }
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

    // Dividir a carga de trabalho entre as threads
    int elements_per_thread = N / M;
    int remainder = N % M;

    int start = 0;
    for (int i = 0; i < M; i++) {
        int end = start + elements_per_thread + (i < remainder ? 1 : 0);

        thread_data[i].vector = vector;
        thread_data[i].start = start;
        thread_data[i].end = end;

        pthread_create(&threads[i], NULL, increment_elements, (void *)&thread_data[i]);

        start = end;
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

