#include <stdio.h>
#include "commom.h"
#include "pong.h"
#include "invaders.h"
#include "breakout.h"
int main(){
char ch;
    clearScreen();
    do{
        clearScreen();
        printf("Choose the game you want to play:\n");
        printf("\t(P)ong\n");
        printf("\t(B)reakout\n");
        printf("\t(S)pace Invaders\n");
        printf("\nOr press 'Q' to exit.\n\n");
        printf("> ");
        ch=wait_input();
        switch(ch){
            case 'p':
                clearScreen();
                p_main();
                break;
            case 'b':
                clearScreen();
                b_main();
                break;
            case 's':
                clearScreen();
                sp_main();
                break;
        }
    }while(ch!='q');
    return 0;
}
