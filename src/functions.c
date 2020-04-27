#include "functions.h"
#include <stdio.h>
#include <string.h>

char* filename = "db.txt";

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
    fprintf(f, "%s\n", title);
    fprintf(f, "%s\n", genre);
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
        if(lines%5==1 && found){
            //linha após a linha do id certo = titulo
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