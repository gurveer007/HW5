#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "csapp.h"

// Dimensions for the drawn grid (should be GRIDSIZE * texture dimensions)
#define GRID_DRAW_WIDTH 640
#define GRID_DRAW_HEIGHT 640

#define WINDOW_WIDTH GRID_DRAW_WIDTH
#define WINDOW_HEIGHT (HEADER_HEIGHT + GRID_DRAW_HEIGHT)

// Header displays current score
#define HEADER_HEIGHT 50

// Number of cells vertically/horizontally in the grid
#define GRIDSIZE 10

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

Position player1Position;
// Position playerPosition;
// Position playerPosition;
// Position playerPosition;

int score;
int level;
int numTomatoes;
char buf[MAXLINE];
bool shouldExit = false;

TTF_Font* font;

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
    if (grid[player1Position.x][player1Position.y] == TILE_TOMATO) {
        grid[player1Position.x][player1Position.y] = TILE_GRASS;
        numTomatoes--;
    }

    // ensure grid isn't empty
    while (numTomatoes == 0)
        initGrid();
}

void initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    int rv = IMG_Init(IMG_INIT_PNG);
    if ((rv & IMG_INIT_PNG) != IMG_INIT_PNG) {
        fprintf(stderr, "Error initializing IMG: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Error initializing TTF: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

void moveTo(int x, int y)
{
    // Prevent falling off the grid
    if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
        return;

    // Sanity check: player can only move to 4 adjacent squares
    if (!(abs(player1Position.x - x) == 1 && abs(player1Position.y - y) == 0) &&
        !(abs(player1Position.x - x) == 0 && abs(player1Position.y - y) == 1)) {
        fprintf(stderr, "Invalid move attempted from (%d, %d) to (%d, %d)\n", player1Position.x, player1Position.y, x, y);
        return;
    }

    player1Position.x = x;
    player1Position.y = y;

    if (grid[x][y] == TILE_TOMATO) {
        grid[x][y] = TILE_GRASS;
        score++;
        numTomatoes--;
        if (numTomatoes == 0) {
            level++;
            initGrid();
        }
    }
}

void handleKeyDown(SDL_KeyboardEvent* event)
{
    // ignore repeat events if key is held down
    if (event->repeat)
        return;

    if (event->keysym.scancode == SDL_SCANCODE_Q || event->keysym.scancode == SDL_SCANCODE_ESCAPE)
        shouldExit = true;

    if (event->keysym.scancode == SDL_SCANCODE_UP || event->keysym.scancode == SDL_SCANCODE_W)
        moveTo(player1Position.x, player1Position.y - 1);

    if (event->keysym.scancode == SDL_SCANCODE_DOWN || event->keysym.scancode == SDL_SCANCODE_S)
        moveTo(player1Position.x, player1Position.y + 1);

    if (event->keysym.scancode == SDL_SCANCODE_LEFT || event->keysym.scancode == SDL_SCANCODE_A)
        moveTo(player1Position.x - 1, player1Position.y);

    if (event->keysym.scancode == SDL_SCANCODE_RIGHT || event->keysym.scancode == SDL_SCANCODE_D)
        moveTo(player1Position.x + 1, player1Position.y);
}

void processInputs()
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				shouldExit = true;
				break;

            case SDL_KEYDOWN:
                handleKeyDown(&event.key);
				break;

			default:
				break;
		}
	}
}

void drawGrid(SDL_Renderer* renderer, SDL_Texture* grassTexture, SDL_Texture* tomatoTexture, SDL_Texture* player1Texture, SDL_Texture* player2Texture, SDL_Texture* player3Texture, SDL_Texture* player4Texture)
{
    SDL_Rect dest;
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            dest.x = 64 * i;
            dest.y = 64 * j + HEADER_HEIGHT;
            SDL_Texture* texture = (grid[i][j] == TILE_GRASS) ? grassTexture : tomatoTexture;
            SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
            SDL_RenderCopy(renderer, texture, NULL, &dest);
        }
    }

    //creating player texture (override the grass texture)
    dest.x = 64 * player1Position.x;
    dest.y = 64 * player1Position.y + HEADER_HEIGHT;
    SDL_QueryTexture(player1Texture, NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopy(renderer, player1Texture, NULL, &dest);
}

void drawUI(SDL_Renderer* renderer)
{
    // largest score/level supported is 2147483647
    char scoreStr[18];
    char levelStr[18];
    sprintf(scoreStr, "Score: %d", score);
    sprintf(levelStr, "Level: %d", level);

    SDL_Color white = {255, 255, 255};
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreStr, white);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

    SDL_Surface* levelSurface = TTF_RenderText_Solid(font, levelStr, white);
    SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);

    SDL_Rect scoreDest;
    TTF_SizeText(font, scoreStr, &scoreDest.w, &scoreDest.h);
    scoreDest.x = 0;
    scoreDest.y = 0;

    SDL_Rect levelDest;
    TTF_SizeText(font, levelStr, &levelDest.w, &levelDest.h);
    levelDest.x = GRID_DRAW_WIDTH - levelDest.w;
    levelDest.y = 0;

    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreDest);
    SDL_RenderCopy(renderer, levelTexture, NULL, &levelDest);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    SDL_FreeSurface(levelSurface);
    SDL_DestroyTexture(levelTexture);
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    //level = 1;

    int clientfd;
    char *host, *port;
    rio_t rio;

    if (argc != 3) {
	    fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
	    exit(0);
    }
    host = argv[1];
    port = argv[2];

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);


    initSDL();

    font = TTF_OpenFont("resources/Burbank-Big-Condensed-Bold-Font.otf", HEADER_HEIGHT);
    if (font == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    player1Position.x = player1Position.y = GRIDSIZE / 2;
    initGrid();

    SDL_Window* window = SDL_CreateWindow("Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (window == NULL) {
        fprintf(stderr, "Error creating app window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	if (renderer == NULL)
	{
		fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
	}

    SDL_Texture *grassTexture = IMG_LoadTexture(renderer, "resources/grass.png");
    SDL_Texture *tomatoTexture = IMG_LoadTexture(renderer, "resources/tomato.png");
    SDL_Texture *player1Texture = IMG_LoadTexture(renderer, "resources/player1.png");
    SDL_Texture *player2Texture = IMG_LoadTexture(renderer, "resources/player2.png");
    SDL_Texture *player3Texture = IMG_LoadTexture(renderer, "resources/player3.png");
    SDL_Texture *player4Texture = IMG_LoadTexture(renderer, "resources/player4.png");

    // main game loop
    while (!shouldExit) {
        SDL_SetRenderDrawColor(renderer, 0, 105, 6, 255);
        SDL_RenderClear(renderer);
        
        Rio_writen(clientfd, buf, strlen(buf));
        processInputs();

        drawGrid(renderer, grassTexture, tomatoTexture, player1Texture, player2Texture, player3Texture, player4Texture);
        drawUI(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // 16 ms delay to limit display to 60 fps
    }

    // clean up everything
    SDL_DestroyTexture(grassTexture);
    SDL_DestroyTexture(tomatoTexture);
    SDL_DestroyTexture(player1Texture);
    SDL_DestroyTexture(player2Texture);
    SDL_DestroyTexture(player3Texture);
    SDL_DestroyTexture(player4Texture);

    TTF_CloseFont(font);
    TTF_Quit();

    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    Close(clientfd);
    exit(0);
}
