#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

long int soma = 0;     // variável compartilhada entre as threads
pthread_mutex_t mutex; // mutex para exclusão mútua
pthread_cond_t cond;   // variável de condição para sincronização

int cont = 0; // contador de múltiplos de 10 impressos

// função executada pelas threads de tarefa
void *ExecutaTarefa(void *arg) {
  long int id = (long int)arg;
  printf("Thread %ld : está executando...\n", id);

  for (int i = 0; i < 100000; i++) {
    pthread_mutex_lock(&mutex);

    // Se a thread extra já imprimiu os 20 múltiplos de 10, todas as threads de
    // tarefa devem encerrar
    if (cont >= 20) {
      pthread_mutex_unlock(&mutex);
      break;
    }

    soma++; // incrementa a variável compartilhada

    // Verifica se 'soma' é múltiplo de 10 e se é um novo múltiplo
    if (soma % 10 == 0 && soma / 10 > cont) {
      pthread_cond_signal(&cond);       // Sinaliza a thread extra
      pthread_cond_wait(&cond, &mutex); // Espera a thread extra imprimir
    }

    pthread_mutex_unlock(&mutex);
  }
  printf("Thread %ld : terminou!\n", id);
  pthread_exit(NULL);
}

// função executada pela thread de log
void *extra(void *args) {
  printf("Extra : está executando...\n");

  while (cont < 20) {
    pthread_mutex_lock(&mutex);

    // Espera por um múltiplo de 10 que ainda não tenha sido impresso
    while (soma % 10 != 0 || soma / 10 <= cont) {
      pthread_cond_wait(&cond, &mutex);
    }

    // Imprime o valor de 'soma' quando for múltiplo de 10
    printf("%d° multiplo = %ld \n", cont + 1, soma);
    cont++;

    // Sinaliza a continuação para as threads de tarefa
    // Como o intuito pelo que entendi é usar apenas uma thread para executar a
    // tarefa poderia ter usado signal ao invés de broadcast, mas achei melhor
    // deixar mais genérico
    // pthread_cond_signal(&cond);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
  }

  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

// fluxo principal
int main(int argc, char *argv[]) {
  pthread_t *tid; // identificadores das threads no sistema
  int nthreads;   // quantidade de threads (passada na linha de comando)

  // lê e avalia os parâmetros de entrada
  if (argc < 2) {
    printf("Digite: %s <número de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);

  // aloca as estruturas
  tid = (pthread_t *)malloc(sizeof(pthread_t) * (nthreads + 1));
  if (tid == NULL) {
    puts("ERRO--malloc");
    return 2;
  }

  // inicializa o mutex e a variável de condição
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  // cria as threads de tarefa
  for (long int t = 0; t < nthreads; t++) {
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
      printf("--ERRO: pthread_create()\n");
      exit(-1);
    }
  }

  // cria a thread de log
  if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
    printf("--ERRO: pthread_create()\n");
    exit(-1);
  }

  // espera todas as threads terminarem
  for (int t = 0; t < nthreads + 1; t++) {
    if (pthread_join(tid[t], NULL)) {
      printf("--ERRO: pthread_join() \n");
      exit(-1);
    }
  }

  // finaliza o mutex e a variável de condição
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);

  return 0;
}
