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
Position player2;
Position player3;
Position player4;

int freeX;
int freeY;
int score;
int level;
int numTomatoes;
int playerId = 0;
pthread_mutex_t lock;

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

    /*
    // force player's position to be grass
    if (grid[player1.x][player1.y] == TILE_TOMATO) {
        grid[player1.x][player1.y] = TILE_GRASS;
        numTomatoes--;
    }
    */

    // ensure grid isn't empty
    while (numTomatoes == 0)
        initGrid();
}

//finding a spot on grid that is grass and storing it into freeX and freeY
void findFreeSpot (int spot) {
    freeX = 100;
    freeY = 100;

    for (int x = 0; x < GRIDSIZE; x++) {
        for (int y = 0; y < GRIDSIZE; y++) {
            //first player
            if (spot == 1) {
                if (grid[x][y] == TILE_GRASS) {
                    freeX = x;
                    freeY = y;
                    return;
                }
            }
            //second player (must make sure not taking player1's position)
            else if (spot == 2) {
                if (grid[x][y] == TILE_GRASS && x != player1.x && y != player1.y) {
                    freeX = x;
                    freeY = y;
                    return;
                }
            } 
            //third player (must make sure not taking other player's position)
            else if (spot == 3) {
                if (grid[x][y] == TILE_GRASS && x != player1.x && y != player1.y && x != player2.x && y != player2.y) {
                    freeX = x;
                    freeY = y;
                    return;
                }
            }
            //fourth player (must make sure not taking other player's position)
            else if (spot == 4) {
                if (grid[x][y] == TILE_GRASS && x != player1.x && y != player1.y && x != player2.x && y != player2.y && x != player3.x && y != player3.y) {
                    freeX = x;
                    freeY = y;
                    return;
                }
            }  
        }
    }
    return;
}

int main(int argc, char **argv) 
{
    srand(time(NULL));

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

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
    player1.x = -1;
    player1.y = -1;

    player2.x = -1;
    player2.y = -1;

    player3.x = -1;
    player3.y = -1;

    player4.x = -1;
    player4.y = -1;

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
    int localPlayerId = 0;
    pthread_mutex_lock(&lock);
    playerId++;
    localPlayerId = playerId;
    findFreeSpot(playerId);
    if (playerId == 1) {
        player1.x = freeX;
        player1.y = freeY;
    }
    else if (playerId == 2) {
        player2.x = freeX;
        player2.y = freeY;
    }
    else if (playerId == 3) {
        player3.x = freeX;
        player3.y = freeY;
    }
    else if (playerId == 4) {
        player4.x = freeX;
        player4.y = freeY;
    }
    pthread_mutex_unlock(&lock);
    position(connfd, localPlayerId);
    Close(connfd);
    return NULL;
}

void position(int connfd, int localId) 
{   
    char buf[MAXLINE] = "";
    //int localId = playerId;

    size_t n; 
    rio_t rio;
    //puts("start of function");
    Rio_readinitb(&rio, connfd);

    pthread_mutex_lock(&lock);
    //encoding the grid into buf (100 chars)
    for (int y = 0; y < GRIDSIZE; y++) {
        for (int x = 0; x < GRIDSIZE; x++) {
            if (player1.x == x && player1.y == y) { //player1
                strcat(buf, "p1,");
            }
            else if (playerId >= 2 && player2.x == x && player2.y == y) { //player 2
                strcat(buf, "p2,");
            }
            else if (playerId >= 3 && player3.x == x && player3.y == y) { //player 3
                strcat(buf, "p3,");
            }
            else if (playerId >= 4 && player4.x == x && player4.y == y) { //player 4
                strcat(buf, "p4,");
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

    sprintf(intToChar, "%d", localId);
    strcat(buf, intToChar);
    strcat(buf, "\n");
    
    //printf("inital id is: %d\n", localId);

    pthread_mutex_unlock(&lock);

    //sending the intial positions to client
    Rio_writen(connfd, buf, strlen(buf));
    strcpy(buf, "");
    char* p;
    char * temp[200];
    int length;
    int tempcounter = 0;

    //continiously read from client
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) { //line:netp:echo:eof
       //puts("start of loop");
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

        //printf("now id is: %d\n", localId);

        pthread_mutex_lock(&lock);

        //saving player positions based on localId
        int tempx = atoi(temp[tempcounter]);
        tempcounter++;
        int tempy = atoi(temp[tempcounter]);

        if (localId == 1) {
            if ((tempx != player2.x || tempy != player2.y) && (tempx != player3.x || tempy != player3.y) && (tempx != player4.x || tempy != player4.y)) {
                player1.x = tempx;
                player1.y = tempy;
            }
        }
        else if (localId == 2) {
            if ((tempx != player1.x || tempy != player1.y) && (tempx != player3.x || tempy != player3.y) && (tempx != player4.x || tempy != player4.y)) {
                player2.x = tempx;
                player2.y = tempy;
            }
        }
        else if (localId == 3) {
            if ((tempx != player1.x || tempy != player1.y) && (tempx != player2.x || tempy != player2.y) && (tempx != player4.x || tempy != player4.y)) {
                player3.x = tempx;
                player3.y = tempy;
            }
        }
        else if (localId == 4) {
            if ((tempx != player1.x || tempy != player1.y) && (tempx != player3.x || tempy != player3.y) && (tempx != player2.x || tempy != player2.y)) {
                player4.x = tempx;
                player4.y = tempy;
            }
        }

        tempcounter = 0;
        strcpy(buf, "");

        //checking if all players has obtained a tomato
        if (grid[player1.x][player1.y] == TILE_TOMATO) {
            grid[player1.x][player1.y] = TILE_GRASS;
            score++;
            numTomatoes--;

            if (numTomatoes == 0) {
                level++;
                initGrid();
            }
        }
        if (playerId >= 2 && grid[player2.x][player2.y] == TILE_TOMATO) {
            grid[player2.x][player2.y] = TILE_GRASS;
            score++;
            numTomatoes--;
            if (numTomatoes == 0) {
                level++;
                initGrid();
            }
        }
        if (playerId >= 3 && grid[player3.x][player3.y] == TILE_TOMATO) {
            grid[player3.x][player3.y] = TILE_GRASS;
            score++;
            numTomatoes--;
            if (numTomatoes == 0) {
                level++;
                initGrid();
            }
        }
        if (playerId >= 4 && grid[player4.x][player4.y] == TILE_TOMATO) {
            grid[player4.x][player4.y] = TILE_GRASS;
            score++;
            numTomatoes--;
            if (numTomatoes == 0) {
                level++;
                initGrid();
            }
        }

        //encoding the grid into buf (100 chars)
        for (int y = 0; y < GRIDSIZE; y++) {
            for (int x = 0; x < GRIDSIZE; x++) {
                if (player1.x == x && player1.y == y) { //player1
                    strcat(buf, "p1,");
                }
                else if (playerId >= 2 && player2.x == x && player2.y == y) { //player 2
                    strcat(buf, "p2,");
                }
                else if (playerId >= 3 && player3.x == x && player3.y == y) { //player 3
                    strcat(buf, "p3,");
                }
                else if (playerId >= 4 && player4.x == x && player4.y == y) { //player 4
                    strcat(buf, "p4,");
                }
                else if (grid[x][y] == TILE_TOMATO) { //tomato
                    strcat(buf, "1,");
                }
                else { //grass
                    strcat(buf, "0,");
                }
            }
        }
        
        // if (localId == 1) {
        //     printf("%s\n", buf); 
        //     printf("p2.x is %d, p2.y is %d\n", player2.x, player2.y);
        //     printf("id is %d\n", playerId); 
        // }

        sprintf(intToChar, "%d", score);
        strcat(buf, intToChar);
        strcat(buf, ",");

        sprintf(intToChar, "%d", numTomatoes);
        strcat(buf, intToChar);
        strcat(buf, ",");

        sprintf(intToChar, "%d", level);
        strcat(buf, intToChar);
        strcat(buf, ",");

        sprintf(intToChar, "%d", localId);
        strcat(buf, intToChar);
        strcat(buf, "\n");

        pthread_mutex_unlock(&lock);
        Rio_writen(connfd, buf, strlen(buf));
        strcpy(buf, "");
        //puts("beginning to read client data");
    }

}