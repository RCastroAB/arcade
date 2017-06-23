#ifndef __WIN32
	#define gotoxy(a,b) (0) //"Undefine function gotoxy() on linux
#endif
/**
 * ATENÇÃO: A função gotoxy() só tem utilidade no Windows. Em Linux ela é inutilizada na declaração acima.
**/

#include "multiplatform.h"

#define UPARROW     'w'
#define DOWNARROW   's'
#define UPARROW2    'i'
#define DOWNARROW2  'k'
#define PAUSEBUTTON 'p'
#define max_points 10
#define BALLSPEED 2

typedef struct v_ball{
    int x,y,has_moved;
}v_ball;

typedef struct player{
    int x,y,pontos;
    int autoplay; //If 0, player mode. if 1, AI moves the player;
}player;

int col,lin;
int **map,**map2;
v_ball ball;

int vou_pegar_a_bola;
float ball_to_move;

player players[2];

/* functions header*/
    void printMap();
    void printNum();
    void rollBallDirection(int linha);
    void init();
    void gameLogic();
    void ai(int linha, int coluna, int id);
    void createPlayers();
    void movePlayer(int id, char dir);
    void game();
    int raid();
    void view_log();
    void log_result();
    void homescreen();
    void game_controller();
    void sera_que_eu_devo_rebater_a_bola();
/*end*/

/** Conio test - works*/

void printHeader(){
    gotoxy(1,1);
    printf("\n\n\t\tPlayer 1: %i | Player 2: %i || %i points to win",players[0].pontos,players[1].pontos,max_points);
    if(players[0].autoplay==0){
        printf(" | Player vs ");
    }else{
        printf(" | COM vs ");
    }
    if(players[1].autoplay==0){
        printf("Player mode           \n");
    }else{
        printf("COM mode           \n");
    }
}

void printMap(){
    int i,j;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            #ifdef __WIN32
            if(map[i][j]!=map2[i][j]){
				map2[i][j]=map[i][j];
            #endif
                gotoxy(1+(j*3),4+i);
                switch(map[i][j]){
                    case 5:
                        printf("###");
                        break;
                    case 8:
                    case 0:
                        printf("   ");
                        break;
                    case 1:
                    case 2:
                        printf("|-|");
                        break;
                    case 70:
                        printf(" o ");
                        break;
                    case 10:
                        printf("!!!");
                        break;
                }
            #ifdef __WIN32
            }
            #endif
        }
        printf("\n");
    }
}

/** End*/

void sera_que_eu_devo_rebater_a_bola(){
    vou_pegar_a_bola = raid()<60 ? 1 : 0;
}

void renderGame(){
    #ifndef __WIN32
        clearScreen();
    #endif
    printHeader();
    printMap();
}

void rollBallDirection(int linha){
    if(ball.has_moved==2){
        ball.x*=-1;
    }else if((linha==1 && ball.y==-1) || (linha==lin-2 && ball.y==1)){
        //nothing;
    }else{
        ball.x = raid()<50 ? 1 : -1;
    }
    if(linha==1 || linha==lin-2)
        ball.y = raid()<50 ? 1 : -1;
    else
        ball.y = raid()<45 ? 1 : (raid()<55 ? 0 : -1);
}

void init(){
    srand( (unsigned)time(NULL) );
    FILE *helper;
    helper = fopen("helper","r");
    if(helper==NULL){
        printf("'helper' file not found, maybe the software package is corrupted.\nExiting...\n");
        wait_input();
        exit(3);
    }
    int i,j;
    fscanf(helper,"%i %i",&lin,&col);
    map = alocaMap(lin,col);
    #ifdef __WIN32
	    map2 = alocaMap(lin,col);
    #endif
    int a=0;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            fscanf(helper,"%i ",&map[i][j]);
            #ifdef __WIN32
                map2[i][j]=999;
            #endif
        }
    }
    fclose(helper);
    createPlayers();
}

void createBall(){
    map[1+(rand()%(lin-2))][col/2]=70;
    rollBallDirection(0);
    ball_to_move=0;
}

void gameLogic(){
    int i,j;
    ball.has_moved=0;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            switch(map[i][j]){
                case 10:
                    renderGame();
                    gotoxy(col,lin+4);
                    printf("\nPoint! Press [enter] to launch another ball.                          ");
                    wait_input();
                    gotoxy(col,lin+4);
                    printf("\n                                             ");
                    sera_que_eu_devo_rebater_a_bola();
                    map[i][j]=8;
                    if(players[0].pontos<max_points && players[1].pontos<max_points)
                        createBall();
                    break;
                case 70:
                    if(ball.has_moved!=0) break;
                    if (ball_to_move >= 1){
                    if(players[0].autoplay==1) ai(i,j,0);
                    if(players[1].autoplay==1) ai(i,j,1);
                    ball.has_moved=1;
                    if(i+ball.y>lin || i+ball.y<0 || j+ball.x>col || j+ball.y<0){
                        rollBallDirection(i);
                        gameLogic();
                        break;
                    }
                    switch(map[i+ball.y][j+ball.x]){
                        case 8:
                            map[i][j]=0;
                            if(j<col/2) players[1].pontos++;
                            else players[0].pontos++;
                            map[i+ball.y][j+ball.x]=10;
                            break;
                        case 0:
                            map[i+ball.y][j+ball.x]=70;
                            if(map[i][j]==71) map[i][j]=88;
                            else map[i][j]=0;
                            break;
                        case 1:
                        case 2:
                            ball.has_moved=2; //Bola tocou no player, logo deve ser rebatida trocando o sentido horizontal do movimento.
                            sera_que_eu_devo_rebater_a_bola();
                        default:
                            rollBallDirection(i);
                            gameLogic();
                            break;
                    }
                    ball_to_move -= 1;
                    } else ball_to_move += 1.0/BALLSPEED;
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
}

void createPlayers(){
  int i,j,count=0,aux;
  for(j=0;j<col;j++){
      for(i=0;i<lin;i++){
      		if(map[i][j]!=0 && map[i][j]!=1 && map[i][j]!=2 && map[i][j]!=5 && map[i][j]!=8){  //Illegal characters
      			printf("Invalid 'helper' file. Illegal characters found: %i on (%i,%i).\nExiting...\n\n",map[i][j],i,j);
			    wait_input();
			    exit(4);
      		}
            if(map[i][j]==8){
                aux=count;
                for(i=0;i<lin;i++){
                    if(map[i][j]==0){
                        printf("Malformed map. There are 0 on the player movement area on (%i,%i).\nExiting...\n\n",i,j);
                        wait_input();
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
                            wait_input();
                            exit(2);
                        }
                    }
                }
                if(aux==count){
                    printf("Invalid 'helper' file. Some '8' are in wrong places on (%i,%i).\nExiting...\n\n",i,j);
                    wait_input();
                    exit(2);
                }else{
                    break;
                }
            }
        }
  }
  if(count!=2){
    printf("Invalid 'helper' file. We need 2 players, found %i.\nExiting...\n\n",count);
    wait_input();
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
  vou_pegar_a_bola=1;
  char ch;
  players[0].pontos = 0;
  players[1].pontos = 0;
  renderGame();
  gotoxy(col, lin+4);
  printf("\nPress [enter] to launch the ball and start game.                    ");
  wait_input();
  gotoxy(col, lin+4);
  printf("\n                                                ");
  createBall(); //spawn the first ball
  while(players[0].pontos<max_points && players[1].pontos<max_points){
    if(hitkey()){
	    ch = getkey();
	    if(ch=='q'){
            //exclui a bolinha da tela
            int i,j;
            for(i=0;i<lin;i++)
                for(j=0;j<col;j++)
                    if(map[i][j]==70) map[i][j]=0;
            break;
        }
	    switch(ch)
	    {
            case UPARROW    :  if(players[0].autoplay==0) movePlayer(0,'u');
                break;
            case DOWNARROW  :  if(players[0].autoplay==0) movePlayer(0,'d');
                break;
            case UPARROW2   :  if(players[1].autoplay==0) movePlayer(1,'u');
                break;
            case DOWNARROW2 :  if(players[1].autoplay==0) movePlayer(1,'d');
                break;

            case PAUSEBUTTON:
            	gotoxy(col,lin+4);
            	printf("\nGame paused. Press any key to resume.                                   ");
            	wait_input();
	            gotoxy(col,lin+4);
    	        printf("\n                                       ");
    	        break;
	    }//end of switch(ch)
    }
  gameLogic();
  renderGame();
  delay(40);
  }
}

void game_controller(){
    init();
    clearScreen();
    char ch;
    do{
        game();
        log_result();
        do{
            gotoxy(col,lin+4);
            printf("\nGame over. Player %i wins!                   \nPlay again? (y/n)                                ", (players[0].pontos>=players[1].pontos ? 1 : 2));
            ch = wait_input();
            gotoxy(col,lin+4);
            printf("\n                           \n                   ");
        }while(ch != 'y' && ch != 'n');
    }while(ch != 'n');
}

int main()
{
    char ch;
    clearScreen();
    printf("\n\t\t\t\t\tLoading...\n");
    delay(1000);
    do{
        clearScreen();
        show_file("homescreen",5);
        ch=wait_input();
        switch(ch){
            case 'g':
                clearScreen();
                show_file("qtd_players",5);
    			ch=wait_input();
			    switch(ch){
			        case '1':
						players[0].autoplay = 1;
						players[1].autoplay = 1;
						game_controller();
			            break;
			        case '2':
						players[0].autoplay = 0;
						players[1].autoplay = 1;
						game_controller();
			            break;
			        case '3':
						players[0].autoplay = 0;
						players[1].autoplay = 0;
						game_controller();
			            break;
		        }
                break;
            case 's':
                clearScreen();
                view_log();
                break;
            case 'h':
                clearScreen();
                show_file("help",5);
                wait_input();
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
        fprintf(log,"Game mode,Points to win,Player 1,Player 2");
        fclose(log);
    };
    log = fopen("game.log","a");
    fprintf(log,"\n");
    if(players[0].autoplay==0){
        fprintf(log,"Human vs ");
    }else{
        fprintf(log,"COM vs ");
    }
    if(players[1].autoplay==0){
        fprintf(log,"Human");
    }else{
        fprintf(log,"COM");
    }
    fprintf(log,",%i,%i,%i",max_points,players[0].pontos,players[1].pontos);
    fclose(log);
}

void view_log(){
    printf("\t\t\t   Matches\n\n");
    FILE *log;
    log=fopen("game.log","r");
    if(log==NULL){
        printf("No log found. Press [enter] to go back.\n");
        wait_input();
        return;
    }
    char buf[50];
    char *token;
    int linha=0;
    while(!feof(log)){
        fgets(buf,50,log);
        token = strtok(strtok(buf,"\n"),",");
        printf(" | %-14s",token);
        token = strtok(NULL,",");
        printf(" | %-13s",token);
        token = strtok(NULL,",");
        printf(" | %-8s",token);
        token = strtok(NULL,",");
        printf(" | %-8s |",token);
        printf("\n");
        if(linha==0)
            printf(" |----------------+---------------+----------+----------|\n");
        linha++;
        delay(5);
    }
    printf("\n Total logged matches: %i\n Press [enter] to go back.\n",linha-1);
    fclose(log);
    wait_input();
}
