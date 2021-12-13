/* 
 * echoservert.c - A concurrent echo server using threads
 */
/* $begin echoservertmain */
#include "csapp.h"

// Number of cells vertically/horizontally in the grid
#define GRIDSIZE 10

void position(int connfd, int playerId);
void *thread(void *vargp);

typedef struct
{
    int x;
    int y;
} Position;

typedef enum
{
    TILE_GRASS,
    TILE_TOMATO
} TILETYPE;

TILETYPE grid[GRIDSIZE][GRIDSIZE];

Position player1;
//Position player2;
//Position player3;
//Position player4;

int score;
int level;
int numTomatoes;
int playerId = 0;
char buf[MAXLINE] = "";

// get a random value in the range [0, 1]
double rand01()
{
    return (double) rand() / (double) RAND_MAX;
}

void initGrid()
{
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            double r = rand01();
            if (r < 0.1) {
                grid[i][j] = TILE_TOMATO;
                numTomatoes++;
            }
            else
                grid[i][j] = TILE_GRASS;
        }
    }

    // force player's position to be grass
    if (grid[player1.x][player1.y] == TILE_TOMATO) {
        grid[player1.x][player1.y] = TILE_GRASS;
        numTomatoes--;
    }

    // ensure grid isn't empty
    while (numTomatoes == 0)
        initGrid();
}

int main(int argc, char **argv) 
{
    srand(time(NULL));

    //creating socket variables
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid; 

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }
    
    //create player position and create the grid
    player1.x = player1.y = GRIDSIZE / 2;
    initGrid();
    level = 1;

    //establish connection with client
    listenfd = Open_listenfd(argv[1]);

    //when new clients connects, create new thread (multithreading)
    while (1) {
        
        clientlen=sizeof(struct sockaddr_storage);
	    connfdp = Malloc(sizeof(int)); //line:conc:echoservert:beginmalloc
	    *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen); //line:conc:echoservert:endmalloc
	    Pthread_create(&tid, NULL, thread, connfdp);
    }
}

//inside the new thread (new client)
void *thread(void *vargp) 
{  
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self()); //line:conc:echoservert:detach
    Free(vargp);                    //line:conc:echoservert:free
    
    //call game related stuff
    playerId++;
    position(connfd, playerId);
    Close(connfd);
    return NULL;
}

void position(int connfd, int playerId) 
{
    int localPlayerId = playerId;
    size_t n; 
    rio_t rio;
    puts("start of function");
    Rio_readinitb(&rio, connfd);

    //encoding the grid into buf (100 chars)
    for (int y = 0; y < GRIDSIZE; y++) {
        for (int x = 0; x < GRIDSIZE; x++) {
            if (player1.x == x && player1.y == y) { //player
                strcat(buf, "5,");
            }
            else if (grid[x][y] == TILE_TOMATO) { //tomato
                strcat(buf, "1,");
            }
            else { //grass
                strcat(buf, "0,");
            }
        }
    }
    char intToChar[10];

    sprintf(intToChar, "%d", score);
    strcat(buf, intToChar);
    strcat(buf, ",");

    sprintf(intToChar, "%d", numTomatoes);
    strcat(buf, intToChar);
    strcat(buf, ",");

    sprintf(intToChar, "%d", level);
    strcat(buf, intToChar);
    strcat(buf, ",");
    
    sprintf(intToChar, "%d", localPlayerId);
    strcat(buf, intToChar);
    strcat(buf, "\n");
    
    //sending the intial positions to client
    Rio_writen(connfd, buf, strlen(buf));
    strcpy(buf, "");
    char* p;
    char * temp[200];
    int length;
    int tempcounter = 0;

    //continiously read from client
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) { //line:netp:echo:eof
       puts("start of loop");
        //do your parsing here and add into local variable
        length = strlen(buf);
        p = strtok(buf, ",");

        //storing values from buf into temp array
        for (size_t i = 0; i < length; i++) {
            if (p) {
                temp[i] = p;
            }
            p = strtok(NULL, ",");
        }

        //saving player positions
        player1.x =  atoi(temp[tempcounter]);
        tempcounter++;
        player1.y =  atoi(temp[tempcounter]);
        tempcounter++;
        localPlayerId = atoi(temp[tempcounter]);
        tempcounter = 0;
        strcpy(buf, "");

        //checking if player has obtained a tomato
        if (grid[player1.x][player1.y] == TILE_TOMATO) {
            grid[player1.x][player1.y] = TILE_GRASS;
            score++;
            numTomatoes--;

            if (numTomatoes == 0) {
                level++;
                initGrid();
            }
        }

        //encoding the grid into buf
        for (int y = 0; y < GRIDSIZE; y++) {
            for (int x = 0; x < GRIDSIZE; x++) {
                if (player1.x == x && player1.y == y) { //player
                    strcat(buf, "5,");
                }
                else if (grid[x][y] == TILE_TOMATO) { //tomato
                    strcat(buf, "1,");
                }
                else { //grass
                    strcat(buf, "0,");
                }
            }
        }
        

        sprintf(intToChar, "%d", score);
        strcat(buf, intToChar);
        strcat(buf, ",");

        sprintf(intToChar, "%d", numTomatoes);
        strcat(buf, intToChar);
        strcat(buf, ",");

        sprintf(intToChar, "%d", level);
        strcat(buf, intToChar);
        strcat(buf, ",");
        
        sprintf(intToChar, "%d", localPlayerId);
        strcat(buf, intToChar);
        strcat(buf, "\n");

        Rio_writen(connfd, buf, n);
        strcpy(buf, "");
    }

}