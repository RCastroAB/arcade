#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pong_render.h"
#define WIDTH 50
#define HEIGHT 20

struct bar{
	int x;
	int y;
};

void moveBar(struct bar *entity, char dir, struct screen frame){
	if (dir == 'w'){
		if (entity->y != 0){
			frame.matrix[entity->y][entity->x] = 'a';
			entity->y--;
			frame.matrix[entity->y][entity->x] = 'p';
		}
	} else if (dir == 'a'){
		if (entity->x != 0){
			frame.matrix[entity->y][entity->x] = 'a';
			entity->x--;
			frame.matrix[entity->y][entity->x] = 'p';
		}
	} else if (dir == 'd'){
		if (entity->x != WIDTH-1){
			frame.matrix[entity->y][entity->x] = 'a';
			entity->x++;
			frame.matrix[entity->y][entity->x] = 'p';
		}
	} else if (dir == 's'){
		if (entity->y != HEIGHT-1){
			frame.matrix[entity->y][entity->x] = 'a';
			entity->y++;
			frame.matrix[entity->y][entity->x] = 'p';
		}
	}
}
int main(){
	printf("this works\n");
	struct screen frame;
	int i,j;
	char yeah;
	struct bar me;
	me.x = me.y = 0;
	frame.height = HEIGHT;
	frame.width = WIDTH;
	frame.matrix = (char **) calloc(HEIGHT, sizeof(char *));
	for (i=0;i<HEIGHT;i++){
		frame.matrix[i] = (char *) calloc(WIDTH, sizeof(char));
	}

	for(i=0;i<HEIGHT;i++){
		for(j=0;j<WIDTH;j++){
			frame.matrix[i][j] = 'a';
		}
	}
	frame.matrix[0][0] = 'p';

	for(;;){
		render(frame);
		printf("x= %i y= %i \n", me.x, me.y);
		yeah = getkey();
		moveBar(&me, yeah, frame);

	}


}
