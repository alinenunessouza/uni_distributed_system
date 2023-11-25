/*
Sistemas Distribuídos e de Tempo Real - GR16023-00048
Aline Nunes de Souza
Trabalho 1 - GB: Sockets

Aplicação do cliente com comunicação por Protocolo TCP.
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXBUFSIZE 1000

int main() {
  int sockfd, n;
  struct sockaddr_in servaddr;
  char sendline[MAXBUFSIZE];
  char recvline[MAXBUFSIZE];

  // Criação do socket TCP
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Configuração do endereço do servidor
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(32000);

  // Inicia a conexão com o servidor
  connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

  while (1) {
    int opcao;
    printf("\nComandos disponíveis:\n");
    printf("111 - Leitura de gás\n112 - Leitura de poeira\n113 - Leitura de "
           "temperatura do ambiente\n");
    printf("124 - Ajustar temperatura do ar-condicionado para 22 graus\n");
    printf("125 - Abrir janelas\n126 - Fechar janelas\n131 - Status alarme\n");
    printf("\n");

    printf("\nDigite o comando desejado abaixo:\n");

    scanf("%d", &opcao);

    // Consumir o caractere de nova linha deixado pelo Enter
    getchar();

    // printf("[Cliente] Comando computado: %d\n", opcao);

    // Construção da mensagem a ser enviada ao servidor
    sprintf(sendline, "%d", opcao);

    // Envio da mensagem ao servidor
    send(sockfd, sendline, strlen(sendline), 0);

    // Recebimento da resposta do servidor
    recvline[0] = '\0';
    n = recv(sockfd, recvline, MAXBUFSIZE, 0);
    recvline[n] = '\0';

    // Resposta do servidor
    printf("[Cliente] Resposta recebida: %s\n", recvline);
    printf("........................");
    printf("\n");
  }

  close(sockfd);
  return 0;
}
