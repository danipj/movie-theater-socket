#include "functions.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
char* filename = "db.txt";

int CLI_COMMAND_SIZE = sizeof(movie) + sizeof(char);
int RESPONSE_SIZE = 5000;

// int count_movies(){
//     FILE *fp;
//     int count = 0;  // Line counter (result)
//     char c;  //
//     FILE *f = fopen(filename, "r");
//     if (f == NULL)
//     {
//         printf("Error opening file!\n");
//         return(-1);
//     }

//     // Extract characters from file and store in character c
//     for (c = getc(fp); c != EOF; c = getc(fp))
//         if (c == '\n') // Increment count if this character is newline
//             count = count + 1;

//     // Close the file
//     fclose(fp);
//     return(count/5);
// }

/** recebe um filme
    int id;
    char title[50];
    char genre[50];
    int room;
    char synopsis[200];

    salva o filme no arquivo de banco de dados

    se correr tudo bem retorna 1, caso contrario retorna -1
**/
int create_movie(int id, char title[50], char genre[50], int room, char synopsis[200]){
    FILE *f = fopen(filename, "a+");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return(-1);
    }

    /*int id = count_movies()+1; /*descobre prox id*/
    fprintf(f, "%d\n", id);
    fprintf(f, "%s\n", genre);
    fprintf(f, "%s\n", title);
    fprintf(f, "%d\n", room);
    fprintf(f, "%s\n", synopsis);

    fclose(f);
    return (1);
}

int list_all(char *response){
    FILE *f = fopen(filename, "r");
    char c;
    int total = 0;
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return(-1);
    }
     while ((c = getc(f)) != EOF){
        response[total] = c;
        total++;
     }
    fclose(f);
    return(1);
}

int find_title_by_id(char id, char * response){
    char c;
    int lines = 0;
    int found = 0;
    int totalLetters = 0;
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return(-1);
    }
    //id a cada 5 linhas: 0, 5, 10 etc
    while ((c = getc(f)) != EOF){
        if(lines%5==0 && c-'0'==id){
            //achou o id
            found = 1;
        }
        if(lines%5==2 && found){
            //linha do titulo
            response[totalLetters] = c;
            totalLetters++;
        }
        if(c=='\n'){
             lines++;
        }
        if(c=='\n' && found && lines%5==2){
            fclose(f);
            return (1); //acabou a linha do titulo, encerrar
        }
     }
    fclose(f);
    return (0); //nao achou
}

int find_info_by_id(char id, char * response){
    char c;
    int lines = 0;
    int found = 0;
    int totalLetters = 0;
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return(-1);
    }
    //id a cada 5 linhas: 0, 5, 10 etc
    while ((c = getc(f)) != EOF){
        if(lines%5==0 && c-'0'==id){
            //achou o id
            found = 1;
        }
        if(found){
            response[totalLetters] = c;
            totalLetters++;
        }

        if(c=='\n'){
            lines++;
        }
        if(found && lines%5==0 && c-'0'!=id){
            fclose(f);
            return (1); //chegou no proximo registro, encerrar
        }
     }
    fclose(f);
    return (0); //nao achou
}

int list_movie_title_rooms(char * response){
    char c;
    int lines = 0;
    int total = 0;
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return(-1);
    }
    while ((c = getc(f)) != EOF){
        if(lines%5==2 || lines%5==3){
            response[total]=c;
            total++;
        }
        if(c=='\n'){
            lines++;
        }
    }
    fclose(f);
    return(1);
}

int list_movie_by_gender(char genre[50],char * response){
    char c;
    int i;
    int lines = 0;
    int totalGenre = 0;
    int total = 0;
    char genero[50];
    int found = 0;
    int check = 0;
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return(-1);
    }
    while ((c = getc(f)) != EOF){
        if(c=='\n'){
            lines++;
        }
        if(lines%5==1 && c!='\n'){ //pegar genero
            genero[totalGenre]=c;
            totalGenre++;
        }

        if(lines%5==2 && c=='\n'){ //terminou o genero pode comparar
            check = 1;
        }

        if(check){
            if(strcmp(genero, genre) == 0){ //é o genero certo
                found = 1;
            }
            check = 0;
            for (i = 0; i < totalGenre; i++)
            {
                genero[i]='\0';
            }
            totalGenre=0;
        }

        if(found && lines%5==2){ //pegar titulo
            response[total] = c;
            total++;
        }

        if(lines%5==3 && c=='\n'){ //terminou o titulo, zerar auxiliares
            found = 0;
        }
    }
    fclose(f);
    return(1);
}

int handle_menu(int menu_option, movie *m, int socket)
{
    int numbytes;
    char message[RESPONSE_SIZE];
    memset(message, '\0', RESPONSE_SIZE);

    switch (menu_option){
        case 1:
            printf("Servidor: Solicitação por cadastro do filme\n");
            create_movie(m->id, m->title, m->genre, m->room, m->synopsis);
            printf("Servidor: Filme '%s' cadastrado!\n", m->title);
            break;
        case 2:
            printf("Servidor: Solicitação de remoção do filme %d\n", m->id);
            printf("Não implementado!!");
            exit(1);
            delete_movie(m->id);
            break;
        case 3:
            printf("Solicitação de listagem dos títulos e salas de todos os filmes\n");

            list_all(message);
            printf("Servidor: message é '%s'\n", message);

            if (send(socket, message, sizeof(message), 0) == -1){
                printf("Erro ao enviar mensagem!\n");
            }

            printf("Servidor: %d bytes enviados", numbytes);
            break;
        case 4:
            printf("Solicitação de listagem de todos os filmes de um gênero\n");

            list_movie_by_gender(m->genre, message);

            printf("Servidor: message é '%s'\n", message);

            if (send(socket, message, sizeof(message), 0) == -1){
                printf("Erro ao enviar mensagem!\n");
            }

            printf("Servidor: %d bytes enviados", numbytes);
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
            return -1;

    }

    return 0;
}

int send_all(int socket, void *buffer, size_t length) {
    char *ptr = (char*) buffer;
    while (length > 0)
    {
        int i = send(socket, ptr, length, 0);
        if (i < 1)
            return -1;
        ptr += i;
        length -= i;
    }
    return 0;
}

void print_movie(movie* m)
{
    printf("----------------\n");
    printf("id: %d\n", m->id);
    printf("nome: %s\n", m->title);
    printf("gênero: %s\n", m->genre);
    printf("sinopse: %s\n", m->synopsis);
    printf("----------------\n");
}