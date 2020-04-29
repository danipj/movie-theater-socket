/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include "functions.h"

#define PORT "8110" // the port client will be connecting to

#define MAXDATASIZE 5000 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int menu(movie *m)
{
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

    scanf("%d", &option);

    switch (option)
    {
    case 1:
        printf("Solicitação por cadastro do filme\n");

        printf("Insira o ID: ");
        scanf("%d", &(m->id));

        printf("Insira o título: ");
        scanf("%s", m->title);

        printf("Insira o genero: ");
        scanf("%s", m->genre);

        printf("Insira a sala: ");
        scanf("%d", &(m->room));

        printf("Insira a sinopse: ");
        scanf("%s", m->synopsis);

        break;
    case 2:
        printf("Solicitação de remoção de um filme\n");

        printf("Insira o ID: ");
        scanf("%d", &(m->id));
        break;
    case 3:
        printf("Solicitação de listagem dos títulos e salas de todos os filmes\n");
        break;
    case 4:
        printf("Solicitação de listagem de todos os filmes de um gênero\n");
        break;
    case 5:
        printf("Solicitação de busca pelo título de um filme\n");
        break;
    case 6:
        printf("Solicitação de busca por todas as informações de um filme\n");
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
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

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
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    // if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
    // {
    //     perror("recv");
    //     exit(1);
    // }

    // buf[numbytes] = '\0';

    // printf("client: received '%s'\n", buf);

    if ((numbytes = write(sockfd, &m, sizeof(movie)) != sizeof(movie)))
    {
        printf("error writing my message");
    }

    char command[309];
    memset(command, '\0', 309);
    command[0] = (char) option + '0';

    send(sockfd, command, 309, 0);

    close(sockfd);

    return 0;
}
