#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "functions.h"

#define PORT "8110" // Porta de conexão ao servidor

#define BACKLOG 10 // Quantas conexões pendentes são permitidas

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

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
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
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
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("Servidor: Aguardando conexões...\n");

    /*
     * Loop principal de conexões. Basicamente é um spin-lock que aguarda
     * por um novo cliente, aceita a conexão e procede com o pedido
     */
    while (1)
    {

        // Esse bloco cuida da conexão em si, aceitando o pedido do cliente
        // e abrindo um novo socket
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("Servidor: obtive conexão de... %s\n", s);

        // Dentro desse bloco, o pedido do cliente será de fato processado
        if (!fork())
        {
            int cli_option;
            int read_bytes;
            char cli_command[CLI_COMMAND_SIZE];
            close(sockfd);

            // Primeiro, lemos o comando do cliente
            read_bytes = recv(new_fd, &cli_option, sizeof(int), 0);

            if (read_bytes != sizeof(int))
            {
                perror("Erro na leitura do comando do cliente!");
                printf("Recebi apenas %db enquanto esperava %lub!", read_bytes, sizeof(int));
                exit(1);
            }

            printf("Servidor: o cliente selecionou a opção %d\n", cli_option);

            // memset(cli_command, '\0', CLI_COMMAND_SIZE * sizeof(char));
            // read_bytes = recv(new_fd, cli_command, CLI_COMMAND_SIZE, 0);

            // if (read_bytes != CLI_COMMAND_SIZE)
            // {
            //     perror("Erro na leitura do comando do cliente!");
            //     printf("Recebi apenas %db enquanto esperava %db!", read_bytes, CLI_COMMAND_SIZE);
            //     exit(1);
            // }

            // printf("Servidor: recebi o comando '%s'\n", cli_command);
            // handle_menu(cli_command, new_fd);

            // if (send(new_fd, "Hello, world!", 13, 0) == -1)
            //     perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd); // parent doesn't need this
    }

    return 0;
}
