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
#define max_points 10

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

int a,b,c,vou_pegar_a_bola;

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
    int wait_input();
    void view_log();
    void log_result();
    void homescreen();
    void game_controller();
    void clearScreen();
/*end*/

void sera_que_eu_devo_rebater_a_bola(){
    vou_pegar_a_bola = raid()<70 ? 1 : 0;
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
    clearScreen();
    printf("Ball direction: (%i, %i) | COM will hit the ball? %i | Loops: %i | Player hits: %i | Wall hits: %i\n",ball.x,ball.y,vou_pegar_a_bola,a,b,c);
    printf("\n\tPlayer 1 (%i,%i): %i | Player 2 (%i,%i): %i || %i points to win",players[0].x,players[0].y,players[0].pontos,players[1].x,players[1].y,players[1].pontos,max_points);
    if(players[0].autoplay==-1){
        printf(" | Player vs ");
    }else{
        printf(" | COM vs ");
    }
    if(players[1].autoplay==-1){
        printf("Player mode\n");
    }else{
        printf("COM mode\n");
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
                case 88:
                case 0:
                    printf("   ");
                    break;
                case 1:
                case 2:
                    printf("|%c|",178);
                    break;
                case 50:
                    printf(" | ");
                    break;
                case 90:
                case 70:
                    printf(" o ");
                    break;
                case 71:
                case 10:
                    printf("!!!");
                    break;
            }
        }
        printf("\n");
    }
}

void printNum(){
    clearScreen();
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
    }else if(linha!=1 && linha!=lin-2){
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
                    printf("\nPoint! Press enter to launch another ball.");
                    //wait_input();
                    sera_que_eu_devo_rebater_a_bola();
                    map[i][j]=8;
                    if(players[0].pontos<max_points && players[1].pontos<max_points)
                        map[1+(rand()%(lin-2))][col/2]=90;
                    break;
                case 71:
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
                            map[i][j]=0;
                            if(j<col/2) players[1].pontos++;
                            else players[0].pontos++;
                            map[i+ball.y][j+ball.x]=10;
                            //gameLogic();
                            break;
                        case 88:
                            if(map[i][j]==71) map[i][j]=88;
                            else map[i][j]=0;
                            map[i+ball.y][j+ball.x]=71;
                            printMap();
                            printf("Debug block (%i,%i)",i+ball.y,j+ball.x);
                            wait_input();
                            break;
                        case 0:
                            map[i+ball.y][j+ball.x]=70;
                            if(map[i][j]==71) map[i][j]=88;
                            else map[i][j]=0;
                            break;
                        case 1:
                        case 2:
                            ball.moved=2;
                            sera_que_eu_devo_rebater_a_bola();
                            b++;
                        default:
                            createBall(i);
                            //gameLogic();
                            break;
                    }
                    if(map[i+ball.y][j+ball.x]==5) c++;
                    break;
            }
        }
    }
}

void ai(int linha, int coluna, int id){
    int r=raid();
    if(vou_pegar_a_bola) {
        if((id==0 && ball.x==-1)||(id==1 && ball.x==1)){ //Seja inteligente
            if(linha<players[id].y+1) movePlayer(id,'u');
            else if(linha>players[id].y+2) movePlayer(id,'d');
        }else{
            if((int)(lin/2)<players[id].y) movePlayer(id,'u');
            else if((int)(lin/2)>players[id].y+3) movePlayer(id,'d');
        }
    }else{
        if((id==0 && ball.x==-1)||(id==1 && ball.x==1)){ //Seja inteligente
            if(linha<players[id].y) movePlayer(id,'u');
            else if(linha>players[id].y+3) movePlayer(id,'d');
        }else{
            if((int)(lin/2)<players[id].y) movePlayer(id,'u');
            else if((int)(lin/2)>players[id].y+3) movePlayer(id,'d');
        }
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
  a=0;b=0;c=0;vou_pegar_a_bola=1;
  char ch;
  players[0].autoplay = 1;
  players[1].autoplay = 1;
  players[0].pontos = 0;
  players[1].pontos = 0;
  printMap();
  printf("\nPress enter to launch the ball and start game.");
  fflush(stdin);
  wait_input();
  map[lin/2][col/2]=90; //spawn the first ball
  while(players[0].pontos<max_points && players[1].pontos<max_points){
    a++;
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
            //case 'c' : map[lin/2][col/2]=90; break;
            case 'a' : players[0].autoplay*=-1; break;
            case 'z' : players[1].autoplay*=-1; break;
	    }//end of switch(ch)
	    //gameLogic();
	    //printMap();
    }
  gameLogic();
  printMap();
  sleepFunc(2);
  }
  //printMap();
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

void game_controller(){
    init();
    char ch;
    do{
        game();
        log_result();
        do{
            printf("\nGame over. Player %i wins!\nPlay again? (y/n) ", (players[0].pontos>=players[1].pontos ? 1 : 2));
            ch = wait_input();
        }while(ch != 'y' && ch != 'n');
    }while(ch != 'n');
}

int main()
{
    char ch;
    clearScreen();
    printf("\n\t\t\t\t\tLoading...\n");
    sleepFunc(2000);
    do{
        clearScreen();
        homescreen();
        ch=wait_input();
        switch(ch){
            case 's':
                clearScreen();
                game_controller();
                break;
            case 'l':
                clearScreen();
                view_log();
                break;
        }
    }while(ch!='q');
    return 0;
}

void log_result(){
    FILE *log;
    log=fopen("game.log","r");
    if(log==NULL){
        log = fopen("game.log","w");
        fprintf(log,"Game mode,Logic loops,Player hits,Wall hits,Points to win,Player 1,Player 2\n");
        fclose(log);
    };
    log = fopen("game.log","a");
    fprintf(log,"\n");
    if(players[0].autoplay==-1){
        fprintf(log,"Human vs ");
    }else{
        fprintf(log,"COM vs ");
    }
    if(players[1].autoplay==-1){
        fprintf(log,"Human");
    }else{
        fprintf(log,"COM");
    }
    fprintf(log,",%i,%i,%i,%i,%i,%i",a,b,c,max_points,players[0].pontos,players[1].pontos);
    fclose(log);
}

void view_log(){
    printf("\t\t\t\t\t   Matches\n\n");
    FILE *log;
    log=fopen("game.log","r");
    if(log==NULL){
        printf("No log found. Press [enter] to go back.\n");
        wait_input();
        return;
    }
    char buf[100];
    char *token;
    int linha=0;
    while(!feof(log)){
        fgets(buf,100,log);
        token = strtok(strtok(buf,"\n"),",");
        printf(" | %-14s",token);
        token = strtok(NULL,",");
        printf(" | %-11s",token);
        token = strtok(NULL,",");
        printf(" | %-11s",token);
        token = strtok(NULL,",");
        printf(" | %-9s",token);
        token = strtok(NULL,",");
        printf(" | %-13s",token);
        token = strtok(NULL,",");
        printf(" | %-8s",token);
        token = strtok(NULL,",");
        printf(" | %-8s |",token);
        printf("\n");
        if(linha==0)
            printf(" |----------------|-------------|-------------|-----------|---------------|----------|----------|\n");
        linha++;
    }
    printf("\n\t\t\t\t   Total logged matches: %i\n\t\t\t\t   Press [enter] to go back.\n",linha-1);
    fclose(log);
    wait_input();
}


void homescreen(){
printf("\n\t8 888888888o       ,o888888o.     b.             8     ,o888888o.    ");
sleepFunc(20);
printf("\n\t8 8888    `88.  . 8888     `88.   888o.          8    8888     `88.  ");
sleepFunc(20);
printf("\n\t8 8888     `88 ,8 8888       `8b  Y88888o.       8 ,8 8888       `8. ");
sleepFunc(20);
printf("\n\t8 8888     ,88 88 8888        `8b .`Y888888o.    8 88 8888           ");
sleepFunc(20);
printf("\n\t8 8888.   ,88' 88 8888         88 8o. `Y888888o. 8 88 8888           ");
sleepFunc(20);
printf("\n\t8 888888888P'  88 8888         88 8`Y8o. `Y88888o8 88 8888           ");
sleepFunc(20);
printf("\n\t8 8888         88 8888        ,8P 8   `Y8o. `Y8888 88 8888   8888888 ");
sleepFunc(20);
printf("\n\t8 8888         `8 8888       ,8P  8      `Y8o. `Y8 `8 8888       .8' ");
sleepFunc(20);
printf("\n\t8 8888          ` 8888     ,88'   8         `Y8o.`    8888     ,88'  ");
sleepFunc(20);
printf("\n\t8 8888             `8888888P'     8            `Yo     `8888888P'    ");
sleepFunc(500);
printf("\n\n\t\t\t\t    (S)tart");
printf("\n\t\t\t\t\(L)og of matches");
printf("\n\t\t\t\t    (Q)uit\n");
}

void clearScreen(){
    #ifdef __WIN32
        system("@cls");
    #else
        system("clear");
    #endif
}
