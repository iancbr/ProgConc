// Ian de Andrade Camargo
// DRE_118089205
// Laboratório 6 

// Programa concorrente que cria e faz operacoes sobre uma lista de inteiros com prioridade para escrita

#include "list_int.h"
#include "timer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define QTDE_OPS 1000 // quantidade de operacoes sobre a lista (insercao, remocao, consulta)
#define QTDE_INI 100  // quantidade de insercoes iniciais na lista
#define MAX_VALUE 100 // valor maximo a ser inserido

// Lista compartilhada iniciada
struct list_node_s *head_p = NULL;

// Quantidade de threads no programa
int nthreads;

// Variáveis de controle para leitura e escrita
int escr = 0, leit = 0, querEscrever = 0;
pthread_mutex_t mutex;
pthread_cond_t cond_escr, cond_leit;

// Funções de controle para leitura e escrita, priorizando os escritores
void EntraLeitura() {
  pthread_mutex_lock(&mutex);
  printf("\nLeitor solicitando acesso!\n");
  while (escr > 0 ||
         querEscrever > 0) { // Espera se houver escritores ativos ou na fila
    printf("Aguardando escritores ativos ou pendentes...\n");
    pthread_cond_wait(&cond_leit, &mutex);
  }
  leit++;
  printf("Leitura concedida. (Leitores ativos: %d, Escritores ativos: %d)\n",
         leit, escr);
  pthread_mutex_unlock(&mutex);
}

void SaiLeitura() {
  pthread_mutex_lock(&mutex);
  leit--;
  printf("Leitor finalizou. (Leitores ativos restantes: %d)\n", leit);
  if (leit == 0) {
    printf("Último leitor saiu. Sinalizando possíveis escritores..\n");
    pthread_cond_signal(
        &cond_escr); // Notifica escritores se não houver leitores ativos
  }
  pthread_mutex_unlock(&mutex);
}

void EntraEscrita() {
  pthread_mutex_lock(&mutex);
  querEscrever++;
  printf("\nEscritor solicitando acesso!\n");
  while (leit > 0 ||
         escr > 0) { // Espera enquanto há leitores ou escritores ativos
    printf("Aguardando leitores ou escritores ativos.. (Leitores ativos: %d, "
           "Escritores ativos: %d)\n",
           leit, escr);
    pthread_cond_wait(&cond_escr, &mutex);
  }
  escr++;
  querEscrever--;
  printf("Escrita concedida. (Escritores ativos: %d)\n", escr);
  pthread_mutex_unlock(&mutex);
}

void SaiEscrita() {
  pthread_mutex_lock(&mutex);
  escr--;
  printf("Escritor finalizou. (Escritores na fila: %d)\n", querEscrever);
  if (querEscrever > 0) {
    printf("Sinalizando próximo escritor na fila..\n");
    pthread_cond_signal(
        &cond_escr); // Dá prioridade a outros escritores na fila
  } else {
    printf("Nenhum escritor pendente. Sinalizando leitores..\n");
    pthread_cond_broadcast(
        &cond_leit); // Notifica leitores se não houver mais escritores
  }
  pthread_mutex_unlock(&mutex);
}

// Tarefa das threads
void *tarefa(void *arg) {
  long int id = (long int)arg;
  int op, in = 0, out = 0, read = 0;

  // Realiza operacoes de consulta (80%), insercao (10%) e remocao (10%)
  for (long int i = id; i < QTDE_OPS; i += nthreads) {
    op = rand() % 100;
    if (op < 80) { // Operação de leitura
      EntraLeitura();
      Member(i % MAX_VALUE, head_p); /* Ignore return value */
      SaiLeitura();
      read++;
    } else if (80 <= op && op < 90) { // Operação de inserção (escrita)
      EntraEscrita();
      Insert(i % MAX_VALUE, &head_p); /* Ignore return value */
      SaiEscrita();
      in++;
    } else if (op >= 90) { // Operação de remoção (escrita)
      EntraEscrita();
      Delete(i % MAX_VALUE, &head_p); /* Ignore return value */
      SaiEscrita();
      out++;
    }
  }
  // Registra a qtde de operacoes realizadas por tipo
  printf("\nThread %ld: in=%d out=%d read=%d\n", id, in, out, read);
  pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
  pthread_t *tid;
  double ini, fim, delta;

  // Verifica se o numero de threads foi passado na linha de comando
  if (argc < 2) {
    printf("Digite: %s <numero de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);

  // Insere os primeiros elementos na lista
  for (int i = 0; i < QTDE_INI; i++) {
    Insert(i % MAX_VALUE, &head_p); /* Ignore return value */
  }

  // Aloca espaco de memoria para o vetor de identificadores de threads no
  // sistema
  tid = malloc(sizeof(pthread_t) * nthreads);
  if (tid == NULL) {
    printf("--ERRO: malloc()\n");
    return 2;
  }

  // Inicializa mutex e variáveis condicionais
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_escr, NULL);
  pthread_cond_init(&cond_leit, NULL);

  // Tomada de tempo inicial
  GET_TIME(ini);

  // Cria as threads
  for (long int i = 0; i < nthreads; i++) {
    if (pthread_create(tid + i, NULL, tarefa, (void *)i)) {
      printf("--ERRO: pthread_create()\n");
      return 3;
    }
  }

  // Aguarda as threads terminarem
  for (int i = 0; i < nthreads; i++) {
    if (pthread_join(*(tid + i), NULL)) {
      printf("--ERRO: pthread_join()\n");
      return 4;
    }
  }

  // Tomada de tempo final
  GET_TIME(fim);
  delta = fim - ini;
  printf("Tempo: %lf\n", delta);

  // Libera recursos
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_escr);
  pthread_cond_destroy(&cond_leit);

  // Libera o espaco de memoria do vetor de threads
  free(tid);

  // Libera o espaco de memoria da lista
  Free_list(&head_p);

  return 0;
}
