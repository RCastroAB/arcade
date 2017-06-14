#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

/*#define col 31
#define lin 22
#define UPARROW     72
#define DOWNARROW   80*/
#define UPARROW     '8'
#define DOWNARROW   '2'
#define UPARROW2    'w'
#define DOWNARROW2  's'
#define centro 11,16

typedef struct v_ball{
    int x,y,moved;
}v_ball;

typedef struct player{
    int x,y,pontos;
}player;

int col,lin;
int **map;
char ch;
v_ball ball;

player players[2];

int** alocaMap(){
    int** map;
    map = (int **) malloc(lin*sizeof(int*));
    if(map==NULL || map==0){
        printf("Erro ao alocar memoria\n");
        exit(1);
    }
    int i=0;
    for(i=0;i<col;i++){
        map[i] = (int *) malloc(col*sizeof(int));
        if(map[i]==NULL || map[i]==0){
            printf("Erro ao alocar memoria\n");
            do{
                free(map[i]);
            }while(--i>=0);
            free(map);
            exit(1);
        }
    }
    return map;
}

int raid(){
	return rand() % 100;
}

void printMap(){
    system("@cls");
    printf("ball vector: (%i, %i) | Player 1: %i, Player 2: %i;\n\n",ball.x,ball.y,players[0].pontos,players[1].pontos);
    int i,j;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            switch(map[i][j]){
                case 5:
                    //if(j==0||j=lin-1)
                    printf("%c%c%c",178,178,178);
                    break;
                case 8:
                case 0:
                    printf("   ");
                    break;
                case 1:
                    printf("%c%c|",178,178);
                    break;
                case 2:
                    printf("|%c%c",178,178);
                    break;
                case 50:
                    printf(" | ");
                    break;
                case 9:
                case 7:
                    printf(" o ");
                    break;
                case 10:
                    printf("!!!");
                    break;
            }
        }
        printf("\n");
    }
}

void createBall(){
    if(ball.moved==2){
        ball.x*=-1;
    }else if(ball.x==0){
        ball.x = raid()<50 ? 1 : -1;
    }
    ball.y = raid()<40 ? 1 : (raid()<60 ? 0 : -1);
}

void printMap1(){
    system("@cls");
    int i,j;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            printf(" %i ",map[i][j]);
        }
        printf("\n");
    }
}

void init(){
    srand( (unsigned)time(NULL) );
    FILE *helper=NULL;
    helper = fopen("helper","r");
    int i,j;
    fscanf(helper,"%i %i",&lin,&col);
    map = alocaMap();
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++)
            fscanf(helper,"%i ",&map[i][j]);
    }
    fclose(helper);
    //createPlayers();
    printMap();
}

void gameLogic(){
    int i,j;
    ball.moved=0;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            switch(map[i][j]){
                case 9:
                    ball.x=0;
                    createBall();
                    map[i][j]=7;
                    break;
                case 7:
                    if(ball.moved!=0) break;
                    ball.moved=1;
                    if(i+ball.y>lin || i+ball.y<0 || j+ball.x>col || j+ball.y<0){
                        createBall();
                        gameLogic();
                        break;
                    }
                    switch(map[i+ball.y][j+ball.x]){
                        case 8:
                            map[i][j]=0;
                            //map[i+ball.y][j+ball.x]=10;
                            if(j<col/2) players[1].pontos++;
                            else players[0].pontos++;
                            map[1+(rand()%(lin-2))][col/2]=9;
                            break;
                        case 0:
                            map[i+ball.y][j+ball.x]=7;
                            map[i][j]=0;
                            break;
                        case 1:
                        case 2:
                            ball.moved=2;
                        default:
                            createBall();
                            gameLogic();
                            break;
                    }
                    break;
            }
        }
    }
}
/*
void ai(int linha){
    if(raid()>50){
        if(linha)
    }
}
*/
void createPlayers(){
  int i,j,count=0;
  for(i=0;i<lin;i++){
    if(map[i][1]==8 || map[i][5]==8){
        for(j=0;j<col;j++){
            if(map[i][j]==1 || map[i][j]==2){
                count++;
                players[count].x=i;
                players[count].y=j;
                players[count].pontos=0;
                break;
            }
            printf("Invalid 'helper' file. Some '8' are in wrong places.\nExiting...\n\n");
            exit(2);
        }
    }
  }
  if(count!=1){
    printf("Invalid 'helper' file. We need 2 players, found %i.\nExiting...\n\n",count+1);
    exit(3);
  }
}

void movePlayer(int id, char dir){
  int i,j;
  for(i=0;i<lin;i++){
    switch(id){
        case 1:
            j=1;
            break;
        case 2:
            j=col-2;
            break;
    }
    if(map[i][j]==id){
        if(dir=='u' && i-1>=0 && map[i-1][j]==8){
            map[i-1][j]=id;
            map[i+3][j]=8;
        }
        else if(dir=='d' && i+4<lin && map[i+4][j]==8){
            map[i+4][j]=id;
            map[i][j]=8;
        }
        break;
    }
  }
}

int main()
{
  init();
  while(1){
    if(kbhit()){
        //fflush(stdin);
	    ch = getch();
	    if(ch=='q') break;
	    switch(ch)
	    {
            case UPARROW    :  movePlayer(2,'u');
                break;
            case DOWNARROW  :  movePlayer(2,'d');
                break;
            case UPARROW2   :  movePlayer(1,'u');
                break;
            case DOWNARROW2 :  movePlayer(1,'d');
                break;
            case 'c' : map[lin/2][col/2]=9; break;
	    }//end of switch(ch)
	    //gameLogic();
	    //printMap();
    }
  gameLogic();
  printMap();
  Sleep(1);

  }
    return 0;
}
