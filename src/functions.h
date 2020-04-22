#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct movie
{
    int id;
    char title[50];
    char genre[50];
    int room;
    char synopsis[200];
} movie;

int create_movie(int id, char title[50], char genre[50], int room, char synopsis[200]);
int delete_movie(int movie_id);
int list_movie_title_rooms();
int list_movie_by_gender(char genre[50]);
int find_title_by_id();
int find_info_by_id();
int list_all();

#endif