#include <stdio.h>
#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>

int getkey() {
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}




struct screen{
	int height;
	int width;
	char **matrix;
};
void render(struct screen frame){
	int i;
	system("clear");
	printf("\n");
	for (i=0; i < frame.height; i++){
		printf("%s\n", frame.matrix[i]);

	}
	for (i=0; i<1000; i++);
	return;

}
