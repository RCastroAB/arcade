#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    #define DIV 1048576
    #define WIDTH 7
#endif

#ifdef linux
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif


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
    int autoplay; //Se -1, player mode, se 1, AI moves the player;
}player;

int col,lin;
int **map;
v_ball ball;

player players[2];

/* functions header*/
    int hitkey(void);
    int getkey(void);
    int **alocaMap();
    int raid();
    void printMap();
    void printNum();
    void createBall(int linha);
    void init();
    void gameLogic();
    void ai(int linha, int coluna, int id);
    void createPlayers();
    void movePlayer(int id, char dir);
    void game();
    void sleepFunc(int i);
/*end*/

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



int** alocaMap(){
    int** map;
    map = (int **) malloc(lin*sizeof(int*));
    if(map==NULL || map==0){
        printf("Erro ao alocar memoria\n");
        exit(1);
    }
    int i=0;
    for(i=0;i<lin;i++){
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
    system("@cls||clear");
    printf("Ball direction: (%i, %i) | Player 1 (%i,%i): %i | Player 2 (%i,%i): %i || 10 points to win",ball.x,ball.y,players[0].x,players[0].y,players[0].pontos,players[1].x,players[1].y,players[1].pontos);
    if(players[0].autoplay==-1){
        printf(" | Player vs ");
    }else{
        printf(" | COM vs ");
    }
    if(players[1].autoplay==-1){
        printf("Player mode\n\n");
    }else{
        printf("COM mode\n\n");
    }
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
                case 90:
                case 70:
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
    system("@cls||clear");
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
}

void gameLogic(){
    int i,j;
    ball.moved=0;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            switch(map[i][j]){
                case 90:
                    ball.x=0;
                    createBall(0);
                    map[i][j]=70;
                    break;
                case 10:
                    printMap();
                    printf("\nPoint! Press any key to launch another ball.");
                    wait_input();
                    map[i][j]=8;
                    map[1+(rand()%(lin-2))][col/2]=90;
                    break;
                case 70:
                    if(ball.moved!=0) break;
                    if(players[0].autoplay==1) ai(i,j,0);
                    if(players[1].autoplay==1) ai(i,j,1);
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
                            map[i+ball.y][j+ball.x]=70;
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
    if((id==0 && ball.x==-1)||(id==1 && ball.x==1)){ //Seja inteligente
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
      for(i=0;i<lin;i++){
            if(map[i][j]==8){
                aux=count;
                for(i=0;i<lin;i++){
                    if(map[i][j]==0){
                        printf("Malformed map. There are 0 on the player movement area. (%i,%i)\nExiting...\n\n",i,j);
                        exit(3);
                    }
                    if(map[i][j]==1 || map[i][j]==2){
                        if(map[i][j]==map[i+1][j] && map[i+1][j]==map[i+2][j] && map[i+2][j]==map[i+3][j]){
                            players[count].x=j;
                            players[count].y=i;
                            players[count].pontos=0;
                            count++;
                            i+=3;
                        }else{
                            printf("Malformed player. A player must have length 4.\nExiting...\n\n");
                            exit(2);
                        }
                    }
                }
                if(aux==count){
                    printf("Invalid 'helper' file. Some '8' are in wrong places.\nExiting...\n\n");
                    exit(2);
                }else{
                    break;
                }
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
  char ch;
  players[0].autoplay = -1;
  players[1].autoplay = 1;
  players[0].pontos = 0;
  players[1].pontos = 0;
  printMap();
  printf("\nPress any key to launch the ball and start game.");
  fflush(stdin);
  wait_input();
  map[lin/2][col/2]=90; //spawn the first ball
  while(players[0].pontos<10 && players[1].pontos<10){
    if(hitkey()){
	    ch = getkey();
	    if(ch=='q') break;
	    switch(ch)
	    {
            case UPARROW    :  if(players[1].autoplay==-1) movePlayer(1,'u');
                break;
            case DOWNARROW  :  if(players[1].autoplay==-1) movePlayer(1,'d');
                break;
            case UPARROW2   :  if(players[0].autoplay==-1) movePlayer(0,'u');
                break;
            case DOWNARROW2 :  if(players[0].autoplay==-1) movePlayer(0,'d');
                break;
            case 'c' : map[lin/2][col/2]=90; break;
            case 'a' : players[0].autoplay*=-1; break;
            case 'z' : players[1].autoplay*=-1; break;
	    }//end of switch(ch)
	    //gameLogic();
	    //printMap();
    }
  gameLogic();
  printMap();
  sleepFunc(10);
  }
}

int getkey(void){
    #ifdef _WIN32
		return getche();
	#else
        return getchar();
	#endif
}

void sleepFunc(int i)
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

int main()
{
    init();
    char ch;
    do{
        game();
        do{
            printf("\nGame over. Player %i wins!\nPlay again? (y/n) ", (players[0].pontos>=players[1].pontos ? 1 : 2));
            ch = wait_input();
        }while(ch != 'y' && ch != 'n');
    }while(ch != 'n');

    return 0;
}
