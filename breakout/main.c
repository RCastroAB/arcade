#ifndef __WIN32
	#define gotoxy(a,b) (0) //"Undefine function gotoxy() on linux
	#define textcolor(x) strcat(b_lineBuffer, x) //Atualiza função de cor
    char b_lineBuffer[1000];
#endif
/**
 * ATENÇÃO: A função gotoxy() só tem utilidade no Windows. Em Linux ela é inutilizada na declaração acima.
**/

#include "../common.h"


#define LEFTARROW   'a'
#define RIGHTARROW  'd'
#define PAUSEBUTTON 'p'
#define RELEASEBALL 't'

typedef struct b_v_ball{
    int x,y,has_moved,with_player;
}b_v_ball;

typedef struct b_player{
    int x,y,lives;
    int autoplay; //If 0, player mode. if 1, AI moves the player;
}b_players;


typedef struct spot{
    int x, y;
} spot;

typedef struct block{
    spot * occupied_spots;
    int size;
} block;


int col,lin;
int **map,**map2;
b_v_ball b_ball;

int blocks_to_win; //Quantidade de blocos em jogo que precisam ser quebrados
int crashed_blocks; //Quantidade de blocos quebrados

//char  b_lineBuffer[1000];
b_players b_player;

/* functions header*/
    void b_rollBallDirection(int linha);
    void b_init();
    void b_gameLogic();
    void b_createPlayers();
    void b_movePlayer(char dir);

    void b_game();
    void b_view_log();
    void b_log_result();
    void b_game_controller();

    void b_printMap();
    void b_renderGame();
    void b_b_printHeader();
    void b_colour(int i);

/*end*/

/** Conio test - works*/

void b_printHeader(){
    #ifdef __WIN32
        gotoxy(1,1);
        textcolor(RESET);
    #endif // __WIN32
    printf("\n\nBlocks to win: %i || Blocks remaining: %i || Lives: %i  \n",blocks_to_win,blocks_to_win-crashed_blocks,b_player.lives);
}

/** End*/
/*
void sera_que_eu_devo_rebater_a_bola(){
    vou_pegar_a_bola = //raid()<60 ? 1 : 0;
}
*/
void b_renderGame(){
    #ifndef __WIN32
        clearScreen();
    #endif
    b_printHeader();
    b_printMap();
}


void b_rollBallDirection(int coluna){
    if(b_ball.has_moved==2){
        b_ball.y*=-1;
        b_ball.x*= raid()<20 ? 0 : -1;
        return;
    }
    if(coluna==0){ //bolinha não lançada
        b_ball.x=0;
        b_ball.y=-1;
        return;
    }
    if((coluna==1 && b_ball.x==-1) || (coluna==col-2 && b_ball.x==1)){
        //nothing;
    }else{
        b_ball.y = raid()<50 ? 1 : -1;
    }
    if(coluna==1 || coluna==col-2)
        b_ball.x = raid()<50 ? 1 : -1;
    else
        b_ball.x = raid()<45 ? 1 : (raid()<55 ? 0 : -1);


}

void b_init(){
    srand( (unsigned)time(NULL) );

    FILE *helper;
    helper = fopen("b_helper","r");
    if(helper==NULL){
        printf("'b_helper' file not found, maybe the software package is corrupted.\nExiting...\n");
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
    b_createPlayers();
    //b_lineBuffer = (char *) malloc(5*col*sizeof(char));
}

void b_createBall(){
    b_ball.with_player=1;
    map[b_player.y-1][b_player.x+4]=7;
    gotoxy(col,lin+4);
    printf("\nPress 't' to launch the ball.                          ");
    gotoxy(1,1);

}

void b_gameLogic(){
    int i,j;
    b_ball.has_moved=0;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            if(map[i][j]==15){ //Roda a transição de um bloco sendo destruído.
                    map[i][j]=0;
            }else if(map[i][j]>=10){ //Última transição de blocos sendo destruídos
                    map[i][j]++;
            }else if(map[i][j]==7){ //bolinha
                if(b_ball.has_moved!=0 || b_ball.with_player==1) break;
                b_ball.has_moved=1;
                if(i+b_ball.y>lin || i+b_ball.y<0 || j+b_ball.x>col || j+b_ball.y<0){
                    b_rollBallDirection(i);
                    b_gameLogic();
                    break;
                }
                switch(map[i+b_ball.y][j+b_ball.x]){
                    case 8: //player area
                        map[i][j]=0;
                        b_player.lives--;
                        if(b_player.lives>=0){
                            b_createBall();
                            b_renderGame();
                            gotoxy(col,lin+4);
                            printf("\nOuch! You missed it.                          ");
                            gotoxy(1,1);
                            wait_input();
                            gotoxy(col,lin+4);
                            printf("\n                     ");
                            b_createBall();
                        }
                        break;
                    case 0: //blank space
                        map[i+b_ball.y][j+b_ball.x]=7;
                        map[i][j]=0;
                        break;
                    case 1: //player
                        b_ball.has_moved=2; //Bola tocou no player, logo deve ser rebatida trocando o sentido vertical do movimento.
                        b_rollBallDirection(j);
                        b_gameLogic();
                        break;
                    case 9: //block
                        map[i+b_ball.y][j+b_ball.x]=10;
                        crashed_blocks++;
                    default: //wall
                        b_rollBallDirection(j);
                        b_gameLogic();
                        break;
                }
                break;

            }
        }
    }
}


void b_createPlayers(){
  int i,j,count=0,aux;
  blocks_to_win=0;
  crashed_blocks=0;
  for(i=0;i<lin;i++){
      for(j=0;j<col;j++){
      		if(map[i][j]!=0 && map[i][j]!=1 && map[i][j]!=9 && map[i][j]!=5 && map[i][j]!=8){  //Illegal characters
      			printf("Invalid 'helper' file. Illegal characters found: %i on (%i,%i).\nExiting...\n\n",map[i][j],i,j);
			    wait_input();
			    exit(4);
      		}
      		if(map[i][j]==9){ //block
                blocks_to_win++;
            }

            if(map[i][j]==8){
                aux=count;
                for(j=0;j<col;j++){
                    if(map[i][j]==0){

                        printf("Malformed map. There are 0 on the player movement area on (%i,%i).\nExiting...\n\n",i,j);
                        wait_input();
                        exit(3);
                    }
                    if(map[i][j]==1){
                        if(1 || (map[i][j]==map[i][j+1] && map[i][j+1]==map[i][j+2] && map[i][j+2]==map[i][j+3] && map[i][j+3]==map[i][j+4])){
                            b_player.x=j;
                            b_player.y=i;
                            b_player.lives=5;
                            count++;
                            j+=10;
                        }else{
                            printf("Malformed player. A player must have length 5.\nExiting...\n\n");

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
  if(count!=1){
    printf("Invalid 'helper' file. We need 1 player, found %i.\nExiting...\n\n",count);
    wait_input();
    exit(3);
  }
  blocks_to_win*=.9; //90% dos blocos devem ser quebrados
}

void b_movePlayer(char dir){
    if(dir=='l' && b_player.x-1>=0){
        int i;
        for(i=1;i<=3;i++){
            if(map[b_player.y][b_player.x-1]==8){
                map[b_player.y][b_player.x+8]=8;
                b_player.x--;
                map[b_player.y][b_player.x]=1;
                if(b_ball.with_player==1){
                    map[b_player.y-1][b_player.x+4]=7;
                    map[b_player.y-1][b_player.x+5]=0;
                }
            }
        }
    }
    else if(dir=='r' && b_player.y+9<col){
        int i;
        for(i=1;i<=3;i++){
            if(map[b_player.y][b_player.x+9]==8){
                map[b_player.y][b_player.x]=8;
                b_player.x++;
                map[b_player.y][b_player.x+8]=1;
                if(b_ball.with_player==1){
                    map[b_player.y-1][b_player.x+4]=7;
                    map[b_player.y-1][b_player.x+3]=0;
                }
            }
        }

    }
}

void b_game(){
  b_init();
  char ch;
  b_player.lives = 5;
  b_ball.with_player=1;
  b_createBall(); //spawn the first ball
  b_renderGame();
  gotoxy(col, lin+4);
  printf("\nPress 't' to launch the ball.                    ");
  while(crashed_blocks<blocks_to_win && b_player.lives>=0){

    if(hitkey()){
	    ch = getkey();
	    if(ch=='q'){
            //exclui a bolinha da tela
            int i,j;
            for(i=0;i<lin;i++)
                for(j=0;j<col;j++)
                    if(map[i][j]==7) map[i][j]=0;

            break;
        }
	    switch(ch)
	    {

            case LEFTARROW    :  if(b_player.autoplay==0) b_movePlayer('l');
                break;
            case RIGHTARROW  :  if(b_player.autoplay==0) b_movePlayer('r');
                break;

            case RELEASEBALL  :
                if(b_ball.with_player){
                    b_ball.with_player=0;
                    b_rollBallDirection(0);
                    gotoxy(col,lin+4);
                    printf("\n                                              ");
                }
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
  b_gameLogic();
  b_renderGame();

  delay(40);

  }
}

void b_game_controller(){


    clearScreen();
    char ch;
    do{
        b_game();
        b_log_result();
        do{
            gotoxy(col,lin+4);

            printf("\nGame over. You %s                   \nPlay again? (y/n)                                ", (crashed_blocks<blocks_to_win ? "lose." : "win!"));
            ch = wait_input();
            gotoxy(col,lin+4);
            printf("\n                           \n                   ");

        }while(ch != 'y' && ch != 'n');
    }while(ch != 'n');
}

int b_main()
{

    char ch;
    clearScreen();
    printf("\n\t\t\t\t\tLoading...\n");
    delay(1000);

    do{
        #ifdef __WIN32
            textbackground(BLACK);
            textcolor(RESET);
        #endif // __WIN32
        clearScreen();
        show_file("b_homescreen",5);
        ch=wait_input();
        switch(ch){


            case 'g':
                clearScreen();
                b_game_controller();
                break;
            case 's':
                clearScreen();
                b_view_log();
                break;

            case 'h':
                clearScreen();
                show_file("b_help",5);
                wait_input();
                break;
        }
    }while(ch!='q');

    return 0;
}

void b_log_result(){
    FILE *log;
    log=fopen("b_game.log","r");
    if(log==NULL){
        log = fopen("b_game.log","w");

        fprintf(log,"Blocks to win,Crashed blocks,Lives,Result");

        fclose(log);
    };
    log = fopen("b_game.log","a");
    fprintf(log,"\n");

    fprintf(log,"%i,%i,%i,%s",blocks_to_win,crashed_blocks,b_player.lives,(crashed_blocks<blocks_to_win ? "Lose" : "Won"));

    fclose(log);
}

void b_view_log(){
    printf("\t\t\t   Matches\n\n");

    FILE *log;
    log=fopen("b_game.log","r");
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
        printf(" | %-13s",token);
        token = strtok(NULL,",");
        printf(" | %-14s",token);
        token = strtok(NULL,",");
        printf(" | %-5s",token);
        token = strtok(NULL,",");
        printf(" | %-6s |",token);
        printf("\n");
        if(linha==0)
            printf(" |---------------+----------------+-------+--------|\n");
        linha++;
        delay(5);
    }
    printf("\n Total logged matches: %i\n Press [enter] to go back.\n",linha-1);
    fclose(log);
    wait_input();
}

void b_colour(int i){
    if (i ==3 || i == 4){
        textcolor(RED);
    } else if(i==5 || i == 5){
        textcolor(YELLOW);
    } else if(i==6 || i ==7){
        textcolor(GREEN);
    } else if(i==8 || i == 9){
        textcolor(BLUE);
    } else if(i == 10 || i == 11 || 12){
        textcolor(CYAN);
    }
}

//Print map

#ifdef _WIN32  //Usando conio

void b_printMap(){
    int i,j;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            if(map[i][j]!=map2[i][j]){
				map2[i][j]=map[i][j];
                gotoxy(1+(j*3),4+i);
                switch(map[i][j]){
                    case 5: //parede não quebrável
                        printf("%c%c%c",178,178,178);
                        break;
                    case 8: //espaço do player
                    case 0: //espaço vazio
                        printf("   ");
                        break;
                    case 1: //player
                        printf("[|]");
                        break;
                    case 7: //bolinha
                        printf(" o ");
                        break;//fuck it, hardcode.
                    case 9: //bloco quebrável
                        b_colour(i);
                        printf("[#]");
                        break;
                    case 10: //bloco recém-quebrado [8][8]
                    case 11:
                    case 12:
                        b_colour(i);
                        printf("[8]");
                        break;
                    case 13: //bloco recém-quebrado 2
                    case 14:
                    case 15:
                        b_colour(i);
                        printf("[-]");
                        break;
                }
            }
            textcolor(RESET);
        }
    }
}

#else  //Sem usar conio

void b_printMap(){
    int i,j;
    for(i=0;i<lin;i++){
        strcpy(b_lineBuffer, "");
        for(j=0;j<col;j++){
                switch(map[i][j]){
                    case 5: //parede não quebrável
                        strcat(b_lineBuffer,  "|||");
                        break;
                    case 8: //espaço do player
                    case 0: //espaço vazio
                        strcat(b_lineBuffer, "   ");
                        break;
                    case 1: //player
                        strcat(b_lineBuffer, "[|]");
                        break;
                    case 7: //bolinha
                        strcat(b_lineBuffer, " o ");
                        break;//fuck it, hardcode.
                    case 9: //bloco quebrável
                        b_colour(i);
                        strcat(b_lineBuffer, "[#]");
                        break;
                    case 10: //bloco recém-quebrado [8][8]
                    case 11:
                    case 12:
                        b_colour(i);
                        strcat(b_lineBuffer, "[8]");
                        break;
                    case 13: //bloco recém-quebrado 2
                    case 14:
                    case 15:
                        b_colour(i);
                        strcat(b_lineBuffer, "[-]");
                        break;
                } strcat(b_lineBuffer, RESET);
        }
        printf("%s\n", b_lineBuffer);
    }
}
#endif
