Nessa atividade temos como objetivo implementação do rwlock com prioridade nas operação de escrita. Para isso utilizamos o padrão leitor e escritor e modificamos para que toda vez que uma escrita for solicitada, novas operações de leitura só poderão ser inicializadas assim que esta seja finalizada. 

Temos como características dessa implementação:

- Leitura simultanea da regiões de memória compartilhada, ou seja, vários leitores podem acessar essa região sem interferência;
- Apenas um "serviço" de escrita por vez;
- Caso haja um escritor em "serviço", ninguém pode acessar a região de memória compartilhada, sejam outros escritores/leitores;
- Caso haja algum escritor na fila para executar sua operação, ele receberá prioridade, ou seja, os pedidos de leitura que forem realizados após esse pedido, só poderão ser executados assim que a escrita for inicializada e concluida.

Para compilar o código, basta usar o seguinte comando 'gcc -o main list_int.c atividade3.c'.

Prints do log:

![log1](https://github.com/user-attachments/assets/a862bda5-fb3d-4a05-8840-edda7af1c281)
![log2](https://github.com/user-attachments/assets/849df2b0-2d59-4e41-9688-d154f28a398d)

