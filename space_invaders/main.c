#include "invaderlibs.h"

#define LEFTARROW 'a'
#define RIGHTARROW 'd'
#define SHOOT ' '

typedef struct starship{
    int x,y;
    int remain_bullets;
    int respawn;
} starship;

typedef struct player_bullet{
    int x,y;
    starship *shooter;
} player_bullet;

typedef struct alien{
    int x,y;
    int shoot_counter;
    int chance_to_shoot;
    int evil_level;
} alien;

typedef struct alien_bullet{
    int x, y;
    alien *shooter;
} alien_bullet;

int col,lin;
int **map,**map2;
int ticks;
char *lineBuffer;
int score,alien_num=2,total_aliens=2, lives=2;
float alien_speed;
starship player;

//Headers

void game_controller();
void init();
void createPlayer();

void game();

void renderGame();
void printHeader();
void printMap();

void gameLogic();
void movePlayer(char dir);
void player_shoot();


/*          MAIN          */
int main(){
    // #ifndef __WIN32
    //     initconio();
    // #endif // linux
    game_controller();
    //
    // #ifndef __WIN32
    //     endconio();
    // #endif // linux
    return 0;
}

/*          FUNCTIONS           */

/*          ESKELETON FUNCTIONS           */
void game_controller(){
    init();
    clearScreen();
    lineBuffer = (char *)calloc(col*3,sizeof(char));
    char ch;
    do{
        game();
        do{
            printf("\nGame over.\nPlay again? (y/n)                                ");

            ch = wait_input();
            printf("\n                           \n                   ");
        }while(ch != 'y' && ch != 'n');
    }while(ch != 'n');
}


void init(){
    srand( (unsigned)time(NULL) );
    FILE *helper=NULL;
    helper = fopen("helper","r");
    int i,j;
    fscanf(helper,"%i %i",&lin,&col);
    map = alocaMap(lin,col);
    map2 = alocaMap(lin,col);
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++)
            fscanf(helper,"%i ",&map[i][j]);
    }
    fclose(helper);
    createPlayer();
}

void createPlayer(){
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
                        player.x=j;
                        player.remain_bullets = 3;
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

void game(){
  char ch;
  renderGame();
  while(alien_num > 0 && (lives > 0)){
    ticks++;
    if(hitkey()){
	    ch = getkey();
	    switch(ch)
	    {
            case LEFTARROW    :
                movePlayer('l');
                break;
            case RIGHTARROW  :
                movePlayer('r');
                break;
            case SHOOT   :
                player_shoot();
                break;
            //case 'c' : map[lin/2][col/2]=90; break;
	    }//end of switch(ch)
    }
    gameLogic();
    renderGame();
    delay(40);
  }
}

/*      RENDER        */


void renderGame(){
    clearScreen();
    printHeader();
    printMap();
}

void printHeader(){
    printf("Score: %i | Remaining Aliens %i | Killed Aliens: %i | Lives: %i          \n",score,alien_num,total_aliens-alien_num,lives);

    return;
}



void printMap(){
    int i,j;
    for(i=0;i<lin;i++){
        strcpy(lineBuffer, "");
        for(j=0;j<col;j++){
            switch(map[i][j]){
                case 5:
                    //if(j==0||j=lin-1)
                    //printf("%c%c%c",178,178,178);
                    strcat(lineBuffer, "|||");
                    break;
                case 8:
                case 88:
                case 0:
                    //printf("   ");
                    strcat(lineBuffer, "   ");
                    break;
                case 1:
                    //printf("|%c|",178);
                    strcat(lineBuffer, "/^\\");
                    break;
                case 50:
                    //printf(" | ");
                    strcat(lineBuffer, " | ");
                    break;
                case 90:
                case 70:
                    //printf(" o ");
                    strcat(lineBuffer, " o ");
                    break;
                case 71:
                case 10:
                    //printf("!!!");
                    strcat(lineBuffer, "!!!");
                    break;
            }
        }
        printf("%s\n",lineBuffer);
    }
}

// void printMap(){
//     int i,j;
//     for(i=0;i<lin;i++){
//         for(j=0;j<col;j++){
//             //if(map[i][j]!=map2[i][j]){
//             //    map2[i][j]=map[i][j];
//             switch(map[i][j]){
//                 case 5:
//                     printf("%c%c%c",178,178,178);
//
//                     break;
//                 case 8:
//                 case 88:
//                 case 0:
//                     printf("   ");
//
//                     break;
//                 case 1:
//                     printf("/^\\");
//
//                     break;
//                 case 50:
//                     printf(" %c ",178);
//
//                     break;
//                 case 90:
//                 case 70:
//                     printf(" o ");
//
//                     break;
//             //}
//             }
//             printf("\n");
//         }
//     }
// }


/*      LOGIC       */


void gameLogic(){
    int i,j;
    for(i=0;i<lin;i++){
        for(j=0;j<col;j++){
            switch(map[i][j]){
                case 90:
                    map[i][j]=70;
                    break;
                /*case 10:
                    renderGame();
                    printf("\nPoint! Press [enter] to launch another ball.                          ");
                    wait_input();
                    printf("\n                                           ");
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
                            renderGame();
                            printf("Debug block (%i,%i)                                         ",i+ball.y,j+ball.x);
                            wait_input();
                            printf("                    ",i+ball.y,j+ball.x);
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
                            gameLogic();
                            break;
                    }
                    if(map[i+ball.y][j+ball.x]==5) c++;*/
                    break;
            }
        }
    }
}


void movePlayer(char dir){
    if(dir=='l' && player.x-1>=0){
        map[player.y][player.x]=8;
        player.x--;
        map[player.y][player.x]=1;
    }
    else if(dir=='r' && player.x<col){
        map[player.y][player.x]=8;
        player.x++;
        map[player.y][player.x]=1;
    }
}

void player_shoot(){
    player_bullet bull;
    bull.x = player.x;
    bull.y = player.y;
    bull.shooter = &player;
}
