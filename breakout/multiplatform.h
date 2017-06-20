#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "multiconio.h"

#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef linux
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif

int hitkey(void);
int getkey(void);
int **alocaMap(int lin, int col);
void freeMap(int **mapa, int lin);
void delay(int i);
int wait_input();
void clearScreen();

int raid(){
	return rand() % 100;
}

int hitkey(void)
{
	#ifdef _WIN32
		return kbhit();
	#else
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getkey();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
	#endif
}

int getkey(void){
    return getch();
}

int** alocaMap(int lin, int col){
    int** mapa;
    mapa = (int **) calloc(lin,sizeof(int*));
    if(mapa==NULL || mapa==0){
        printf("Erro ao alocar memoria\n");
        exit(1);
    }
    int i=0;
    for(i=0;i<lin;i++){
        mapa[i] = (int *) calloc(col,sizeof(int));
        if(mapa[i]==NULL || mapa[i]==0){
            printf("Erro ao alocar memoria\n");
            do{
                free(mapa[i]);
            }while(--i>=0);
            free(mapa);
            exit(1);
        }
    }
    return mapa;
}

void freeMap(int** mapa, int lin){
    while(--lin>=0){
        free(mapa[lin]);
    }
    free(mapa);
}

void delay(int i)
{
    #ifdef _WIN32
	    Sleep(i);
	#else
        sleep((float)i/1000);
    #endif
}

int wait_input(){
    #ifdef _WIN32
	    fflush(stdin);
	#else
        __fpurge(stdin);
    #endif
    return getkey();
}

void clearScreen(){
    #ifdef __WIN32
        system("@cls");
    #else
        system("clear");
    #endif
}