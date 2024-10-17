// Ian de Andrade Camargo
// DRE_118089205
// Laboratório 7 

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 500            
#define BUFFER_SIZE 1000 

// Buffers
char buffer1[N];
char buffer2[BUFFER_SIZE];
int count1 = 0;           // Contador para buffer1
int count2 = 0;           // Contador para buffer2
int finished_reading = 0; // Flag para indicar se a leitura do arquivo terminou

// Semáforos
sem_t sem_buffer1, sem_buffer2;

// Função executada pela Thread 1 (leitura do arquivo)
void *t1(void *arg) {
  char *arquivo = (char *)arg;      // Nome do arquivo passado como argumento
  FILE *file = fopen(arquivo, "r"); // Abre o arquivo
  if (!file) {
    perror("Erro ao abrir o arquivo");
    pthread_exit(NULL);
  }

  while (1) {
    // Lê N caracteres do arquivo
    count1 = fread(buffer1, sizeof(char), N, file);
    if (count1 <= 0) {
      finished_reading = 1;   // Indica que a leitura foi concluída
      sem_post(&sem_buffer1); // Sinaliza o fim para a Thread 2
      break;                  // Fim do arquivo ou erro
    }

    // Posta no semáforo para sinalizar que o buffer1 está pronto
    sem_post(&sem_buffer1);
    sem_wait(&sem_buffer2); // Aguarda a Thread 2 processar
  }

  fclose(file);
  pthread_exit(NULL);
}

// Função executada pela Thread 2 (processamento do buffer1 para buffer2)
void *t2(void *arg) {
  int n = 0, cont = 0;

  while (1) {
    // Espera que buffer1 tenha dados
    sem_wait(&sem_buffer1);

    // Se a leitura terminou e não há mais dados, encerra a Thread 2
    if (finished_reading && count1 == 0) {
      sem_post(&sem_buffer2); // Libera a Thread 1 antes de sair
      break;
    }

    // Processa os caracteres no buffer1
    for (int i = 0; i < count1; i++) {
      buffer2[count2++] = buffer1[i];
      cont++;

      // Adiciona uma nova linha a cada 2n + 1 caracteres se n < 10
      if (cont == (2 * n + 1) && n < 10) {
        buffer2[count2++] = '\n';
        n++;
        cont = 0;
      }

      // Adiciona nova linha a cada 10 caracteres se n >= 10
      if (cont == 10 && n >= 10) {
        buffer2[count2++] = '\n';
        cont = 0;
      }
    }

    // Sinaliza que buffer2 está pronto
    sem_post(&sem_buffer2);
  }

  pthread_exit(NULL);
}

// Função executada pela Thread 3 (impressão do buffer2)
void *t3(void *arg) {
  while (1) {
    // Espera que buffer2 tenha dados
    sem_wait(&sem_buffer2);

    // Se a leitura terminou e buffer2 está vazio, encerra a Thread 3
    if (finished_reading && count2 == 0) {
      break;
    }

    // Imprime os caracteres no buffer2
    for (int i = 0; i < count2; i++) {
      putchar(buffer2[i]);
    }
    count2 = 0; // Reseta o contador de buffer2 para permitir novo processamento

    // Libera a Thread 1 para continuar leitura
    sem_post(&sem_buffer1);
  }

  pthread_exit(NULL);
}

// Função principal
int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Uso: %s <nome do arquivo>\n", argv[0]);
    return 1;
  }

  pthread_t tid[3];

  // Inicializa os semáforos
  sem_init(&sem_buffer1, 0, 0);
  sem_init(&sem_buffer2, 0, 0);

  // Cria as threads
  pthread_create(&tid[0], NULL, t1,
                 (void *)argv[1]); // Passa o nome do arquivo para t1
  pthread_create(&tid[1], NULL, t2, NULL);
  pthread_create(&tid[2], NULL, t3, NULL);

  // Espera todas as threads terminarem
  for (int i = 0; i < 3; i++) {
    pthread_join(tid[i], NULL);
  }

  // Destroi os semáforos
  sem_destroy(&sem_buffer1);
  sem_destroy(&sem_buffer2);

  return 0;
}

