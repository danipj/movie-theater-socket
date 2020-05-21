#include <stdio.h>
#include "functions.h"

int main(){
    printf("%d\n",create_movie(1, "Avengers", "herois", 0, "DRESCRIAO"));
    printf("%d\n",create_movie(2, "Reri poter", "magia", 3, "descircao"));
    printf("%d\n",create_movie(3, "Moana", "action", 2, "sinoposen"));
    printf("%d\n",create_movie(4, "Aladdin", "action", 1, "sinoposen"));

    printf("loco\n");
    char respostassss[350], teste[350];
    //printf("%d\n",list_all(respostassss));
    //printf("%s\n", respostassss);  

    // printf("%d\n",list_movie_title_rooms(respostassss));
    // printf("%s\n", respostassss);

    printf("%d\n", delete_movie('1'));
    printf("%d\n",list_all(teste));
    printf("%s\n",teste);

}