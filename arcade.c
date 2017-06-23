#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void run(char[30]);
void compile(char[30]);
void clearScreen();


int main(){
    
    int choice;
    do {
        choice = -1;
        clearScreen();
        printf("ARCADE\n");
        printf("Choose your game\n\n(1)Pong\n(2)Breakout\n(3)Space Invaders\n(0)Exit\n");
        scanf("%i", &choice);
        switch(choice){
            case 1:
                run("pong");
                break;
            case 2:
                run("breakout");
                break;
            case 3:
                run("space_invaders");
                break;
        }

    } while (choice != 0);
    return 0;
    
}


void run(char game[30]){
    compile(game);
    char execute[100];
    strcpy(execute, "");
    strcat(execute, "cd ");
    strcat(execute, game);
    strcat(execute, "; ./");
    strcat(execute, game);

    system(execute);

    char remove[100];
    strcpy(remove, "");
    strcat(remove, "rm ");
    strcat(remove, game);
    strcat(remove, "/");
    strcat(remove, game);
    system(remove);
    return;
}

void compile(char game[30]){
    char src_path[100];
    strcpy(src_path, "");
    strcat(src_path, game);
    strcat(src_path, "/main.c");
    FILE * game_src = fopen(src_path, "r");
    if (game_src == NULL){
        clearScreen();
        printf("Game files not found, try downloading again.\n");
        exit(3);
    }
    char compile[200];
    strcpy(compile, "gcc ");
    strcat(compile, src_path);
    strcat(compile, " -o ");
    strcat(compile, game);
    strcat(compile, "/");
    strcat(compile, game);

    system(compile);
}

void clearScreen(){
    #ifdef linux
        system("clear");
    #else
        system("@cls");
    #endif
}
