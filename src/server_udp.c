/*
** listener.c -- a datagram sockets "server" demo
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

#define MYPORT "4950" // the port users will be connecting to

#define MAXBUFLEN 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("Servidor: Aguardando conexões...\n");

    int cli_option;

    movie* m =malloc(sizeof(struct movie));


    addr_len = sizeof their_addr;

    // Lê opção do cliente
    // numbytes = recvfrom(sockfd, &cli_option, sizeof(int), 0, (struct sockaddr *)&their_addr, &addr_len);

    // if (numbytes == -1)
    // {
    //     perror("Erro na leitura da opção");
    //     exit(1);
    // }

    // printf("Servidor: recebi pacote de %s\n", inet_ntop(their_addr.ss_family,
    //                                                     get_in_addr((struct sockaddr *)&their_addr),
    //                                                     s, sizeof s));

    // printf("Servidor: o cliente selecionou a opção %d\n", cli_option);

    numbytes = 0;
    int step = 0;

    while (1) {
        addr_len = sizeof(their_addr);
        numbytes = recvfrom(sockfd, m, sizeof(movie), 0, (struct sockaddr *)&their_addr, &addr_len);
        if (numbytes == -1)
        {
            perror("Erro na leitura do filme");
            exit(1);
        }
        printf("Servidor: recebi o filme:\n");
        print_movie(m);
    }






    close(sockfd);

    return 0;
}
