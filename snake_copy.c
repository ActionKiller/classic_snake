#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <wchar.h>

#define WIDTH 900
#define HEIGHT 600

#define CELL_SIZE 25
#define ROWS HEIGHT/CELL_SIZE
#define COLUMNS WIDTH/CELL_SIZE
#define LINE_WIDTH 1

#define COLOR_GRID 0x1f1f1f1f
#define COLOR_BLACK 0x000000
#define COLOR_WHITE 0xffffffff
#define COLOR_APPLE 0xff0000

#define SNAKE(x, y) fill_cell(psurface, x, y, COLOR_WHITE)
#define APPLE(x, y) fill_cell(psurface, x, y, COLOR_APPLE)
#define DRAW_GRID draw_grid(psurface)

// Linked List
struct SnakeElement {
    int x, y;
    struct SnakeElement *pnext;
};

struct Apple {
    int x, y;
};

struct Direction {
    int dx, dy;
};

int draw_grid(SDL_Surface* psurface) {
    SDL_Rect row_line = {0, 0, 900, LINE_WIDTH};
        for (row_line.y = 0; row_line.y < ROWS * CELL_SIZE; row_line.y += CELL_SIZE){
        SDL_FillRect(psurface, &row_line, COLOR_GRID);
    }
    SDL_Rect column_line = {0, 0, LINE_WIDTH, HEIGHT};
        for (column_line.x = 0; column_line.x < WIDTH; column_line.x += CELL_SIZE) {
        SDL_FillRect(psurface, &column_line, COLOR_GRID);
    }
}

void fill_cell(SDL_Surface* psurface, int x, int y, Uint32 color) {
    SDL_Rect rect = {x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_FillRect(psurface, &rect, color); 
}

void draw_snake(SDL_Surface *psurface, struct SnakeElement* psnake) {
    if(psnake != NULL) {
        SNAKE(psnake->x, psnake->y);
        draw_snake(psurface, psnake->pnext);
    }
}

//void draw_apple(SDL_Surface* psurface, struct Apple* papple) {
//}

void move_snake(struct SnakeElement *psnake, struct Direction *pdirection) {
    psnake->x += pdirection->dx;
    psnake->y += pdirection->dy;
}


void reset_apple(struct SnakeElement *psnake, struct Apple *papple) {
    papple->x =  COLUMNS * ((double) rand() / RAND_MAX);
    papple->y = ROWS * ((double) rand() / RAND_MAX);

    // If apple coordinates collide with snake, try again.
    struct SnakeElement *pcurrent = psnake;
    do
    {
        if (pcurrent->x == papple->x && pcurrent->y == papple->y) {
            reset_apple(psnake, papple);
            break;
        }
        pcurrent = pcurrent->pnext;
    }
    while (pcurrent != NULL);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("Classic Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* psurface = SDL_GetWindowSurface(window);

    SDL_Event event;
    int game = 1;
    
    struct SnakeElement snake = {5, 5, NULL};
    struct SnakeElement *psnake = &snake;
    struct Direction direction = {0, 0};
    struct Direction *pdirection = &direction;
    struct Apple apple;
    struct Apple *papple = &apple;
    reset_apple(psnake, papple);

    SDL_Rect override_rect = {0, 0, WIDTH, HEIGHT};
   
    while(game) {
        
        while(SDL_PollEvent(&event)){
                if (event.type == SDL_QUIT)
                    game = 0;
                if(event.type == SDL_KEYDOWN) {
                     direction = (struct Direction) {0,0};
                    if (event.key.keysym.sym == SDLK_RIGHT)
                        direction.dx = 1;
                    if (event.key.keysym.sym == SDLK_LEFT)
                        direction.dx = -1;
                    if (event.key.keysym.sym == SDLK_UP)
                        direction.dy = -1;
                    if (event.key.keysym.sym == SDLK_DOWN)
                        direction.dy = +1;
                }
        }
        SDL_FillRect(psurface, &override_rect, COLOR_BLACK);

        move_snake(psnake, pdirection);

        if (psnake->x ==  papple->x && psnake->y == papple->y) {
            reset_apple(psnake, papple);
        }

        printf("apple x=%d\napple y=%d\n", papple->x, papple->y);
        APPLE(papple->x, papple->y);
        // draw_apple(psurface, papple);
        draw_snake(psurface, psnake);
        DRAW_GRID;

        SDL_UpdateWindowSurface(window);
        SDL_Delay(300);
    }
}
