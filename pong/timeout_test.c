#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#define WAIT 3

int main ()
{
    char            name[20] = {0}; // in case of single character input
    fd_set          input_set;
    struct timeval  timeout;
    int             ready_for_reading = 0;
    int             read_bytes = 0;

    /* Empty the FD Set */
    FD_ZERO(&input_set );
    /* Listen to the input descriptor */
    FD_SET(0, &input_set);

    /* Waiting for some seconds */
    timeout.tv_sec = WAIT;    // WAIT seconds
    timeout.tv_usec = 0;    // 0 milliseconds

    /* Invitation for the user to write something */
    printf("Enter Username: (in %d seconds)\n", WAIT);
    printf("Time start now!!!\n");

    /* Listening for input stream for any activity */
    ready_for_reading = select(1, &input_set, NULL, NULL, &timeout);
    /* Here, first parameter is number of FDs in the set,
     * second is our FD set for reading,
     * third is the FD set in which any write activity needs to updated,
     * which is not required in this case.
     * Fourth is timeout
     */

    if (ready_for_reading == -1) {
        /* Some error has occured in input */
        printf("Unable to read your input\n");
        return -1;
    }

    if (ready_for_reading) {
        read_bytes = read(0, name, 19);
        if(name[read_bytes-1]=='\n'){
        --read_bytes;
        name[read_bytes]='\0';
        }
        if(read_bytes==0){
            printf("You just hit enter\n");
        } else {
            printf("Read, %d bytes from input : %s \n", read_bytes, name);
        }
    } else {
        printf(" %d Seconds are over - no data input \n", WAIT);
    }

    return 0;
}
