/*
Sistemas Distribuídos e de Tempo Real - GR16023-00048
Aline Nunes de Souza
Trabalho 1 - GB: Sockets

Aplicação do servidor com comunicação por Protocolo TCP.
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAXBUFSIZE 1000

// gerar um número aleatório dentro de um intervalo
int generateRandomNumber(int min, int max) {
  return rand() % (max - min + 1) + min;
}

int main() {
  int listenfd, connfd, n;
  struct sockaddr_in servaddr, cliaddr;
  socklen_t clilen;
  char recv_buffer[MAXBUFSIZE];
  char send_buffer[MAXBUFSIZE];

  int alarme = 0;               // indica se o alarme está ativo
  time_t alarme_start_time = 0; // armazena o tempo de início do alarme

  srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios

  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(32000);

  bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

  listen(listenfd, 1024);

  printf("\t##### TCP server #####\n");
  printf("\tWaiting for requests...\n");

  for (;;) {
    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

    if (fork() == 0) {
      n = 1;
      while (n > 0) {
        recv_buffer[0] = '\0';
        n = recv(connfd, recv_buffer, MAXBUFSIZE, 0);
        recv_buffer[n] = '\0';

        if (recv_buffer[0] != '\0') {
          int opcao = atoi(recv_buffer);

          switch (opcao) {
          case 111: { // Leitura de gás
            int valor = generateRandomNumber(10, 1000);
            if (valor > 130) {
              alarme = 1;
            }
            sprintf(send_buffer, "111%d", valor);
            break;
          }
          case 112: { // Leitura de poeira
            int valor = generateRandomNumber(0, 50);
            if (valor > 10) {
              alarme = 1;
            }
            sprintf(send_buffer, "112%d", valor);
            break;
          }
          case 113: { // Leitura de temperatura do ambiente
            int valor = generateRandomNumber(-5, 45);
            if (valor < 5 || valor > 35) {
              alarme = 1;
            }
            sprintf(send_buffer, "113%d", valor);
            break;
          }
          case 124: { // Ajustar temperatura do ar-condicionado para 22 graus
            int sucesso = rand() % 2; // 50% de chance de sucesso
            if (sucesso) {
              sprintf(send_buffer, "1241");
            } else {
              sprintf(send_buffer, "1240");
              printf("[Servidor]: Erro ao tentar executar comando\n");
            }
            break;
          }
          case 125: {                 // Abrir janelas
            int sucesso = rand() % 2; // 50% de chance de sucesso
            if (sucesso) {
              sprintf(send_buffer, "1251");
            } else {
              sprintf(send_buffer, "1250");
              printf("[Servidor]: Erro ao tentar executar comando\n");
            }
            break;
          }
          case 126: {                 // Fechar janelas
            int sucesso = rand() % 2; // 50% de chance de sucesso
            if (sucesso) {
              sprintf(send_buffer, "1261");
            } else {
              sprintf(send_buffer, "1260");
              printf("[Servidor]: Erro ao tentar executar comando\n");
            }
            break;
          }
          case 131: { // Status alarme
            if (!alarme) {
              sprintf(send_buffer, "130");
            } else {
              time_t current_time;
              time(&current_time);
              if (current_time - alarme_start_time >
                  300) { // 5 minutos em segundos
                printf("[Servidor]: Comunicado via e-mail para responsáveis "
                       "enviado.\n");
                alarme = 0;
                sprintf(send_buffer, "131");
              } else {
                sprintf(send_buffer, "131");
              }
            }
            break;
          }
          default:
            sprintf(send_buffer, "Comando inválido");
          }

          // Envia resposta ao cliente
          send(connfd, send_buffer, strlen(send_buffer), 0);
        }
      }
    }
  }

  return 0;
}
