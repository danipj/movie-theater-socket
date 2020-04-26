#include <stdio.h>
#include "functions.h"

int main(){
    printf("%d\n",create_movie(1, "seila", "testeeee", 0, "uauuuuu"));

    printf("loco\n");
    char respostassss[350];
    printf("%d\n",list_all(respostassss));
    respostassss[349] = '\0';
    printf("%s\n", respostassss);    
}