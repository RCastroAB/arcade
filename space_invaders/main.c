#include "../common.h"

#define LEFTARROW 'a'
#define RIGHTARROW 'd'
#define SHOOT ' '
#define MAXBULLETS 3
#define LIVES 5
#define ALIENMAXBULLETS 100


#define BULLET 7 //numeric coding for map
#define ENEMYBULLET 6

typedef struct starship{
    int x,y;
    int remain_bullets;
    int respawn;
} starship;

typedef struct player_bullet{
    int x,y;
    char active;//if the bullet is or not in the map
    starship *shooter;
} player_bullet;

typedef struct alien{
    int x,y;
    int alive;
    int shoot_counter;
    int chance_to_shoot;
    int evil_level;
} alien;

typedef struct alien_bullet{
    int x, y;
    char active;//if the bullet is or not in the map
    alien *shooter;
} alien_bullet;


//Global Variables
int col,lin;
int **map,**map2;
int ticks;
char *lineBuffer;

int win, score,alien_num,total_aliens, lives;

starship sp_player;
player_bullet bullets[MAXBULLETS];


alien * enemies = NULL;
float alien_speed, alien_move;
char move_dir;
alien_bullet enemy_bullets[ALIENMAXBULLETS];


//Headers

void sp_game_controller();
void sp_freeMem();
void sp_init();
void sp_createBullets();
void sp_createAliens();
void sp_createPlayer();

void sp_game();

void sp_renderGame();
void sp_printHeader();
void sp_printMap();

void sp_gameLogic();
void sp_movePlayer(char dir);
void sp_player_shoot();
void sp_forceGameOver();
void sp_killPlayer();


/*          MAIN          */
int sp_main(){
    char ch;
    clearScreen();
    printf("\n\t\t\t\t\tLoading...\n");
    delay(2000);
    do{
        clearScreen();
        show_file("sp_homescreen",5);
        ch=toUpper(wait_input());
        switch(ch){
            case 'G':
                clearScreen();
                sp_game_controller();
                break;
            case 'H':
                clearScreen();
                show_file("sp_manual", 500);
                break;
        }
    }while(ch!='Q');

    return 0;
}


/*          FUNCTIONS           */

/*          ESKELETON FUNCTIONS           */
void sp_game_controller(){
    char ch;
    do{
        sp_init();
        clearScreen();
        #ifndef __WIN32
            lineBuffer = (char *)calloc(col*3,sizeof(char));
        #endif // linux
        sp_game();
        do{
            gotoxy(col,lin+4);
            printf("\n%s.\nPlay again? (y/n)", win ? "You Win" : "Game Over");
            ch = wait_input();
            gotoxy(col,lin+4);
            printf("\n                           \n                   ");
        }while(ch != 'y' && ch != 'n');
        sp_freeMem();
    }while(ch != 'n');
}

void sp_freeMem(){
    freeMap(map, lin);
    free(enemies);
    #ifndef __WIN32
        free(lineBuffer);
    #endif // linux
}


void sp_init(){
    srand( (unsigned)time(NULL) );
    FILE *helper=NULL;
    helper = fopen("sp_helper","r");
    if(helper==NULL){
        printf("'sp_helper' file not found, maybe the software package is corrupted.\nExiting...\n");
        wait_input();
        exit(3);
    }
    int i,j;
    fscanf(helper,"%i %i %i",&lin,&col, &total_aliens);
    map = alocaMap(lin,col);
    #ifdef __WIN32
        map2 = alocaMap(lin,col);
    #endif // __WIN32
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            fscanf(helper,"%i ",&map[i][j]);
            #ifdef __WIN32
                map2[i][j]=999;
            #endif // __WIN32
        }
    }
    fclose(helper);
    score =0;
    alien_speed = 0.001;
    win = 0;
    sp_createAliens();
    sp_createBullets();
    sp_createPlayer();
}

void sp_createAliens(){
    int i, j, k=0;
    if ((enemies = (alien *)malloc(total_aliens*sizeof(alien))) == NULL){
        printf("error\n");
        exit(1);
    }
    alien_num =0;
    for(j=0;j<col;j++){
        for(i=0;i<lin;i++){
            if(map[i][j]>60 && map[i][j]<66){//alien found
                enemies[k].x = j;
                enemies[k].y = i;
                enemies[k].alive =1;
                enemies[k].evil_level = map[i][j] - 60;
                enemies[k].shoot_counter = 100 - (enemies[k].evil_level *10);
                enemies[k].chance_to_shoot = 20 * enemies[k].evil_level;
                alien_num++;
                k++;
            }
        }
    }
    if (alien_num != total_aliens){
        printf("map disagrees with parameters on number of aliens (%i , %i)\n", total_aliens, alien_num);
        exit(1);
    }
    move_dir = 'd';
}

void sp_createBullets() {
    int i;
    for(i=0;i<MAXBULLETS;i++){
        bullets[i].active =0;
    }
    for(i=0;i<ALIENMAXBULLETS;i++){
        enemy_bullets[i].active =0;
    }
    return;
}
void sp_createPlayer(){
  int i,j,count=0,aux;
  for(j=0;j<col;j++){
      for(i=0;i<lin;i++){
            if(map[i][j]==8){//line found
                aux=count;
                for(j=0;j<col;j++){
                    if(map[i][j]==0){
                        printf("Malformed map. There are 0 on the player movement area. (%i,%i)\nExiting...\n\n",i,j);
                        wait_input();
                        exit(3);
                    } else if(map[i][j]==1){
                        lives = LIVES;
                        sp_player.x=j;
                        sp_player.y=i;
                        sp_player.remain_bullets = MAXBULLETS;
                        sp_player.respawn = 0;
                        count++;
                    }
                }
                if(aux==count){//no player was found in movement area
                    printf("Invalid 'helper' file. Some '8' are in wrong places.\nExiting...\n\n");
                    wait_input();
                    exit(2);
                }else{
                    break;
                }
            }
        }
  }
  if(count!=1){
    printf("wrong number of players: %i.\nExiting...\n\n",count);
    wait_input();
    exit(3);
  }
}

void sp_game(){
  char ch;
  sp_renderGame();
  while(alien_num > 0 && (lives > 0)){
    ticks++;
    if(hitkey()){
	    ch = getkey();
	    switch(ch)
	    {
            case LEFTARROW:
                sp_movePlayer('l');
                break;
            case RIGHTARROW:
                sp_movePlayer('r');
                break;
            case SHOOT:
                sp_player_shoot();
                break;
	    }//end of switch(ch)
    }
    sp_gameLogic();
    sp_renderGame();
    delay(40);
  }
  if (alien_num == 0) win =1;
}

/*      RENDER        */


void sp_renderGame(){
    #ifndef __WIN32
        clearScreen();
    #endif // __WIN32
    sp_printHeader();
    sp_printMap();
}

void sp_printHeader(){
    gotoxy(1,1);
    printf("Score: %i | Remaining Aliens %i | Killed Aliens: %i | Lives: %i | Bullets: %i         \n",score,alien_num,total_aliens-alien_num,lives, sp_player.remain_bullets);

    return;
}
int maxr, maxl;


#ifdef __WIN32 // Usar a conio caso Windows

void sp_printMap(){
    int i,j;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
          if(map[i][j]!=map2[i][j]){
			map2[i][j]=map[i][j];
			gotoxy(1+(j*3),3+i);
            switch(map[i][j]){
                case 5:
                    printf("|||");
                    break;
                case 8:
                case 88:
                case 0:
                    printf("   ");
                    break;
                case 1:
                    printf("/^\\");
                    break;
                case 61:
                    printf(")-(");
                    break;
                case 62:
                    printf("}W{");
                    break;
                case 63:
                    printf("w0w");
                    break;
                case 64:
                    printf("|0|");
                    break;
                case 65:
                    printf("/=\\");
                    break;
                case 6:
                    printf(" + ");
                    break;
                case 7:
                    printf(" * ");
                    break;
                    case 10:
                case 71:
                    printf("!!!");
                    break;
            }
          }
        }
    }
}

#else

void sp_printMap(){
    int i,j;
    for(i=0;i<lin;i++){
        strcpy(lineBuffer, "");
        for(j=0;j<col;j++){
            switch(map[i][j]){
                case 5:
                    strcat(lineBuffer, "|||");
                    break;
                case 8:
                case 88:
                case 0:
                    strcat(lineBuffer, "   ");
                    break;
                case 1:
                    strcat(lineBuffer, "/^\\");
                    break;
                case 61:
                    strcat(lineBuffer, ")-(");
                    break;
                case 62:
                    strcat(lineBuffer, "}W{");
                    break;
                case 63:
                    strcat(lineBuffer, "w0w");
                    break;
                case 64:
                    strcat(lineBuffer, "|0|");
                    break;
                case 65:
                    strcat(lineBuffer, "/=\\");
                    break;
                case 6:
                    strcat(lineBuffer, " + ");
                    break;
                case 7:
                    strcat(lineBuffer, " * ");
                    break;
                    case 10:
                case 71:
                    strcat(lineBuffer, "!!!");
                    break;
            }
        } printf("%s\n",lineBuffer);
    }

}

#endif

/*      LOGIC       */


void sp_gameLogic(){//dividido em partes
    int i,j;
    /* PLAYER_BULLETS  */
    for(i = 0; i < MAXBULLETS; i++){
        if(bullets[i].active){
            map[bullets[i].y][bullets[i].x] = 0;
            switch(map[bullets[i].y-1][bullets[i].x]){
                case 61:
                case 62:
                case 63:
                case 64:
                case 65:
                case 0:
                    map[bullets[i].y-1][bullets[i].x] = BULLET;
                    bullets[i].y--;
                break;
                case 5:
                    bullets[i].y = 0;
                    bullets[i].x = 0;
                    bullets[i].active = 0;
                    sp_player.remain_bullets++;
                break;
            }
        }
    }

    /* ALIEN BULLETS */

    for(i = 0; i < MAXBULLETS; i++){
        if(enemy_bullets[i].active){
            map[enemy_bullets[i].y][enemy_bullets[i].x] = 0;
            switch(map[enemy_bullets[i].y+1][enemy_bullets[i].x]){
                case 1:
                case 0:
                    map[enemy_bullets[i].y+1][enemy_bullets[i].x] = ENEMYBULLET;
                    enemy_bullets[i].y++;
                    break;
                case 5:
                    map[enemy_bullets[i].y][enemy_bullets[i].x] = 8;
                    enemy_bullets[i].y = 0;
                    enemy_bullets[i].x = 0;
                    enemy_bullets[i].active = 0;
                    break;
                default:
                    enemy_bullets[i].y++;
                break;

            }
        }
    }

    /* PLAYER */
    if (sp_player.respawn == 0){
        switch(map[sp_player.y][sp_player.x]){
            case 0:
            case 8:
                map[sp_player.y][sp_player.x] = 1;
                break;
            case 61:    //if alien or
            case 62:
            case 63:
            case 64:
            case 65:
                sp_forceGameOver();
                break;
            case 6:     //bullet
                sp_killPlayer();
                break;
        }
    } else {
        map[sp_player.y][sp_player.x] = 8;
        sp_player.respawn--;
    }
    /* ALIEN */
    alien_move+=alien_speed;
    int maxleft = col, maxright = 0;

    if (alien_move >= 1){ //MOVEMENT
        alien_move =0;
        if (move_dir == 'r'){
            for(i=0;i<total_aliens;i++){
                if (enemies[i].alive){
                    map[enemies[i].y][enemies[i].x] = 0;
                    if (enemies[i].x == col-3) move_dir = 'd';
                    enemies[i].x++;
                }
            }
        } else if(move_dir == 'l'){
            for(i=0;i<total_aliens;i++){
                if (enemies[i].alive){
                    map[enemies[i].y][enemies[i].x] = 0;
                    if (enemies[i].x == 2) move_dir = 'd';
                    enemies[i].x--;
                }
            }
        } else {
            alien_speed+=0.001;
            for(i=0;i<total_aliens;i++){//procura a parede para evitar
                if (enemies[i].alive){
                    map[enemies[i].y][enemies[i].x] = 0;
                    maxleft = maxleft < enemies[i].x ? maxleft : enemies[i].x;
                    maxright = maxright > enemies[i].x ?  maxright : enemies[i].x;
                    enemies[i].y++;
                }
            }
            if (maxleft>2){
                move_dir ='l';
            } else if(maxright < col-2){
                move_dir ='r';
            } else move_dir = 'd';

        }

    }

    //DIE N' SHOOT
    for(i=0;i<total_aliens;i++){
        if (enemies[i].alive){
            switch(map[enemies[i].y][enemies[i].x]){
                case 0:
                case 6:
                    map[enemies[i].y][enemies[i].x] = 60 + enemies[i].evil_level;
                    break;
                case 7:
                    enemies[i].alive = 0;
                    score+= enemies[i].evil_level*10;
                    for(j=0; j<MAXBULLETS;j++){
                        if(bullets[j].x == enemies[i].x && bullets[j].y == enemies[i].y){
                            bullets[j].active = 0;
                            sp_player.remain_bullets++;
                            break;
                        }
                    }
                    alien_num--;
                    alien_speed+= enemies[i].evil_level/1000.0;
                    map[enemies[i].y][enemies[i].x] = 0;
                    break;
		        case 1:
                case 8:
                    sp_forceGameOver();
                    break;
            }
            if (enemies[i].shoot_counter==0 ){
                if (RollDice() < enemies[i].chance_to_shoot){
                    for(j=0;j<ALIENMAXBULLETS;j++){
                        if (enemy_bullets[j].active == 0) break;
                    }
                    enemy_bullets[j].active = 1;
                    enemy_bullets[j].x = enemies[i].x;
                    enemy_bullets[j].y = enemies[i].y;
                }
                enemies[i].shoot_counter = 100 - (enemies[i].evil_level *10);

            } else{
                enemies[i].shoot_counter--;
            }
        }
    }

    ticks++;
    return;
}


void sp_movePlayer(char dir){
    if(dir=='l' && sp_player.x>1){
        map[sp_player.y][sp_player.x]=8;
        sp_player.x--;
        map[sp_player.y][sp_player.x]=1;
    }
    else if(dir=='r' && sp_player.x<col-2){
        map[sp_player.y][sp_player.x]=8;
        sp_player.x++;
        map[sp_player.y][sp_player.x]=1;
    }
}

void sp_player_shoot(){
    if (sp_player.remain_bullets == 0 || sp_player.respawn >0 ){
        return;
    }
    int i;
    for (i=0; i < MAXBULLETS; i++){
        if (bullets[i].active == 0){
            bullets[i].x = sp_player.x;
            bullets[i].y = sp_player.y;
            bullets[i].shooter = &sp_player;
            sp_player.remain_bullets--;
            bullets[i].active = 1;
            return;
        }
    }
    printf("error\n");
    exit(1);

}

void sp_killPlayer(){
    sp_player.respawn = 20;
    lives--;
}

void sp_forceGameOver(){
    lives =0;
}
