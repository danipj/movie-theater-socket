#include <stdio.h>
#include "functions.h"

int main(){
    printf("%d\n",create_movie(1, "seila", "testeeee", 0, "uauuuuu"));
    printf("%d\n",create_movie(2, "novo", "genero", 3, "descircao"));
    printf("%d\n",create_movie(3, "hmmmm", "action", 2, "sinoposen"));

    printf("loco\n");
    char respostassss[350];
    //printf("%d\n",list_all(respostassss));
    //printf("%s\n", respostassss);  

    printf("%d\n",find_title_by_id(2,respostassss));
    printf("%s\n", respostassss); 
}