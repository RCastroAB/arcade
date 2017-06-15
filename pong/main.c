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
    printf("Ball direction: (%i, %i) | Player 1 (%i,%i): %i | Player 2 (%i,%i): %i || 10 points to win.\n\n",ball.x,ball.y,players[0].x,players[0].y,players[0].pontos,players[1].x,players[1].y,players[1].pontos);
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

void printNum(){
    system("@cls");
    printf("Ball direction: (%i, %i) | Player 1 (%i,%i): %i | Player 2 (%i,%i): %i || 10 points to win.\n\n",ball.x,ball.y,players[0].x,players[0].y,players[0].pontos,players[1].x,players[1].y,players[1].pontos);
    int i,j;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            printf(" %i ",map[i][j]);
        }
        printf("\n");
    }
}

void createBall(int linha){
    if(ball.moved==2){
        ball.x*=-1;
    }else if(ball.x==0){
        ball.x = raid()<50 ? 1 : -1;
    }
    if(linha==1 || linha==lin-2)
        ball.y = raid()<50 ? 1 : -1;
    else
        ball.y = raid()<45 ? 1 : (raid()<55 ? 0 : -1);
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
    createPlayers();
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
                    createBall(0);
                    map[i][j]=7;
                    break;
                case 10:
                    printMap();
                    printf("\nPoint! Press any key to launch another ball.");
                    fflush(stdin);
                    getche();
                    map[i][j]=8;
                    map[1+(rand()%(lin-2))][col/2]=9;
                    break;
                case 7:
                    if(ball.moved!=0) break;
                    //ai(i,j,0);
                    ai(i,j,1);
                    ball.moved=1;
                    if(i+ball.y>lin || i+ball.y<0 || j+ball.x>col || j+ball.y<0){
                        createBall(i);
                        gameLogic();
                        break;
                    }
                    switch(map[i+ball.y][j+ball.x]){
                        case 8:
                            /*if((j+ball.x)==players[0].x){
                                ball.moved=2;
                                createBall(i);
                                gameLogic();
                                break;
                            }*/
                            map[i][j]=0;
                            map[i+ball.y][j+ball.x]=10;
                            if(j<col/2) players[1].pontos++;
                            else players[0].pontos++;
                            gameLogic();
                            break;
                        case 0:
                            map[i+ball.y][j+ball.x]=7;
                            map[i][j]=0;
                            break;
                        case 1:
                        case 2:
                            ball.moved=2;
                        default:
                            createBall(i);
                            gameLogic();
                            break;
                    }
                    break;
            }
        }
    }
}

void ai(int linha, int coluna, int id){
    int r=raid();
    if(r<15){}
    else if((id==0 && ball.x==-1)||(id==1 && ball.x==1)){ //Seja inteligente
        if(linha<players[id].y+1) movePlayer(id,'u');
        else if(linha>players[id].y+2) movePlayer(id,'d');
    }else{
        if((int)(lin/2)<players[id].y) movePlayer(id,'u');
        else if((int)(lin/2)>players[id].y+3) movePlayer(id,'d');
    }
    //Ou fique parado
}

void createPlayers(){
  int i,j,count=0,aux;
  for(j=0;j<col;j++){
    if(map[1][j]==8 || map[5][j]==8){
        aux=count;
        for(i=0;i<lin;i++){
            if(map[i][j]==1 || map[i][j]==2){
                players[count].x=j;
                players[count].y=i;
                players[count].pontos=0;
                count++;
                break;
            }
        }
        if(aux==count){
            printf("Invalid 'helper' file. Some '8' are in wrong places.\nExiting...\n\n");
            exit(2);
        }
    }
  }
  if(count!=2){
    printf("Invalid 'helper' file. We need 2 players, found %i.\nExiting...\n\n",count);
    exit(3);
  }
}

void movePlayer(int id, char dir){
    if(dir=='u' && players[id].y-1>=0 && map[players[id].y-1][players[id].x]==8){
        map[players[id].y+3][players[id].x]=8;
        players[id].y--;
        map[players[id].y][players[id].x]=id+1;
    }
    else if(dir=='d' && players[id].y+4<lin && map[players[id].y+4][players[id].x]==8){
        map[players[id].y][players[id].x]=8;
        players[id].y++;
        map[players[id].y+3][players[id].x]=id+1;
    }
}

void game(){
  players[0].pontos = 0;
  players[1].pontos = 0;
  while(players[0].pontos<10 && players[1].pontos<10){
    if(kbhit()){
        //fflush(stdin);
	    ch = getch();
	    if(ch=='q') break;
	    switch(ch)
	    {
            /*case UPARROW    :  movePlayer(2,'u');
                break;
            case DOWNARROW  :  movePlayer(2,'d');
                break;*/
            case UPARROW2   :  movePlayer(0,'u');
                break;
            case DOWNARROW2 :  movePlayer(0,'d');
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
}

int main()
{
    init();
    do{
        game();
        do{
            printf("\nGame over. Player %i wins!\nPlay again? (y/n) ", (players[0].pontos>=players[1].pontos ? 1 : 2));
            fflush(stdin);
            ch = getche();
        }while(ch != 'y' && ch != 'n');
    }while(ch != 'n');

    return 0;
}
