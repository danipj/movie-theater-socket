/*
** talker.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "functions.h"

#define SERVERPORT "4950" // the port users will be connecting to

void remove_final_linebreaker(char str[])
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
}

int menu(movie *m)
{
    char buffer[4];
    int option;
    printf("+----------------------------------------------------------------------------------------------+\n");
    printf("|                                                                                              |\n");
    printf("| (1) Cadastrar um novo filme recebendo como resposta positiva o seu respectivo identificador  |\n");
    printf("| (2) Remover um filme a partir do seu identificador                                           |\n");
    printf("| (3) Listar o título e salas de exibição de todos os filmes                                   |\n");
    printf("| (4) Listar todos os títulos de filmes de um determinado gênero                               |\n");
    printf("| (5) Dado o identificador de um filme, retornar o título do filme                             |\n");
    printf("| (6) Dado o identificador de um filme, retornar todas as informações deste filme              |\n");
    printf("| (7) Listar todas as informações de todos os filmes                                           |\n");
    printf("| (8) Sair                                                                                     |\n");
    printf("|                                                                                              |\n");
    printf("+----------------------------------------------------------------------------------------------+\n");
    printf("Digite uma opção > ");

    fgets(buffer, 4, stdin);
    option = atoi(buffer);

    switch (option)
    {
    case 1:
        printf("Solicitação por cadastro do filme\n");

        printf("Insira o ID: ");
        fgets(buffer, 4, stdin);
        m->id = atoi(buffer);

        printf("Insira o título: ");
        fgets(m->title, 50, stdin);
        remove_final_linebreaker(m->title);

        printf("Insira o genero: ");
        fgets(m->genre, 50, stdin);
        remove_final_linebreaker(m->genre);

        printf("Insira a sala: ");
        fgets(buffer, 4, stdin);
        m->room = atoi(buffer);

        printf("Insira a sinopse: ");
        fgets(m->synopsis, 200, stdin);
        remove_final_linebreaker(m->synopsis);

        break;
    case 2:
        printf("Solicitação de remoção de um filme\n");

        printf("Insira o ID: ");
        fgets(buffer, 4, stdin);
        m->id = atoi(buffer);
        break;
    case 3:
        printf("Solicitação de listagem dos títulos e salas de todos os filmes\n");
        break;
    case 4:
        printf("Solicitação de listagem de todos os filmes de um gênero\n");

        printf("Informe o gênero: ");
        fgets(m->genre, 50, stdin);
        remove_final_linebreaker(m->genre);
        break;
    case 5:
        printf("Solicitação de busca pelo título de um filme\n");

        printf("Insira o ID: ");
        fgets(buffer, 4, stdin);
        m->id = atoi(buffer);
        break;
    case 6:
        printf("Solicitação de busca por todas as informações de um filme\n");

        printf("Insira o ID: ");
        fgets(buffer, 4, stdin);
        m->id = atoi(buffer);
        break;
    case 7:
        printf("Solicitação por todos os dados de todos os filmes\n");
        break;
    default:
        exit(0);
    }
    return option;
}

int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    if (argc != 2)
    {
        fprintf(stderr, "Utilização: client <ip>\n");
        exit(1);
    }

    movie m;
    int option;
    option = menu(&m);
    printf("O título %s terá o id %d\n", m.title, m.id);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to create socket\n");
        return 2;
    }

    // Envia opção para o servidor
    numbytes = sendto(sockfd, &option, sizeof(int), 0,
                      p->ai_addr, p->ai_addrlen);
    if (numbytes == -1)
    {
        perror("client: sendto");
        exit(1);
    }

    // Envia filme para o servidor
    numbytes = sendto(sockfd, &m, sizeof(movie), 0,
                      p->ai_addr, p->ai_addrlen);

    if (numbytes == -1)
    {
        perror("Erro no envio do filme");
        exit(1);
    }

    freeaddrinfo(servinfo);

    printf("cliente: enviei %d bytes\n", numbytes);

    char response[RESPONSE_SIZE];
    strncpy(response, "", sizeof(response));

    numbytes = recvfrom(sockfd, &response, RESPONSE_SIZE - 1, 0, p->ai_addr, &p->ai_addrlen);
    if (numbytes == -1)
    {
        perror("Erro no recebimento de resposta");
        exit(1);
    }

    printf("Resposta: %s\n", response);

    close(sockfd);

    return 0;
}
