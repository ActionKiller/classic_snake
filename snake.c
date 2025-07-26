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
#include <time.h>

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
#define DRAW_GRID() draw_grid(psurface)

// Linked List
struct SnakeElement {
    int x, y;
    struct SnakeElement *pnext;
};

struct Apple {
    int x, y;
};

typedef enum {
    DIR_NONE,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
} Direction;

void draw_grid(SDL_Surface* psurface) {
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
    struct SnakeElement* current = psnake;
    while (current != NULL) {
        SNAKE(current->x, current->y);
        current = current->pnext;
    }
}


void reset_apple(struct SnakeElement *psnake, struct Apple *papple) {
    int valid = 0;
    while (!valid) {
        papple->x = rand() % COLUMNS;
        papple->y = rand() % ROWS;

        valid = 1;
        struct SnakeElement *pcurrent = psnake;
        while (pcurrent != NULL) {
            if (pcurrent->x == papple->x && pcurrent->y == papple->y) {
                valid = 0;
                break;
            }
            pcurrent = pcurrent->pnext;
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("Classic Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* psurface = SDL_GetWindowSurface(window);

    srand((unsigned int)time(NULL));

    SDL_Event event;
    int game = 1;
    
    int score = 0;
    int frameDelay = 150;

    struct SnakeElement *psnake = malloc(sizeof(struct SnakeElement));
    psnake->x = 5;
    psnake->y = 5;
    psnake->pnext = NULL;

    Direction direction = DIR_NONE;
    struct Apple apple;
    struct Apple *papple = &apple;
    reset_apple(psnake, papple);

    SDL_Rect override_rect = {0, 0, WIDTH, HEIGHT};
   
    while(game) {
        
        while(SDL_PollEvent(&event)){
                if (event.type == SDL_QUIT)
                    game = 0;
                if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            if (direction != DIR_DOWN) direction = DIR_UP;
                            break;
                        case SDLK_DOWN:
                            if (direction != DIR_UP) direction = DIR_DOWN;
                            break;
                        case SDLK_LEFT:
                            if (direction != DIR_RIGHT) direction = DIR_LEFT;
                            break;
                        case SDLK_RIGHT:
                            if (direction != DIR_LEFT) direction = DIR_RIGHT;
                            break;
                    }
                }
        }
        SDL_FillRect(psurface, &override_rect, COLOR_BLACK);

        // ✅ Wait for direction input before moving
        if (direction == DIR_NONE) {
            APPLE(papple->x, papple->y);
            draw_snake(psurface, psnake);
            DRAW_GRID();
            SDL_UpdateWindowSurface(window);
            SDL_Delay(100);
            continue;
        }
        
        // Now safe to insert a new head
        struct SnakeElement* newHead = malloc(sizeof(struct SnakeElement));
        newHead->x = psnake->x;
        newHead->y = psnake->y;
        newHead->pnext = psnake;

        switch (direction) {
          case DIR_UP:    newHead->y--; break;
          case DIR_DOWN:  newHead->y++; break;
          case DIR_LEFT:  newHead->x--; break;
          case DIR_RIGHT: newHead->x++; break;
          default: break;  
        }

        // Update snake head pointer
        psnake = newHead;

       // Check self-collision
        struct SnakeElement *pcurrent = psnake->pnext;
        while (pcurrent != NULL) {
            if(pcurrent->x == psnake->x && pcurrent->y == psnake->y) {
                printf("Game Over: Snake hit itself!\n");
                game = 0;
                break;
            }
            pcurrent = pcurrent->pnext;
        }

        // Wall collision check
        if (psnake->x <0 || psnake->x >= COLUMNS || psnake->y < 0 || psnake->y >= ROWS) {
            printf("Game Over: Snake hit the wall!\n");
            game = 0;
            continue;
        }

        // Check apple collision
        int ateApple = 0;
        if (psnake->x ==  papple->x && psnake->y == papple->y) {
            ateApple = 1;
            score++;

            // Speed up every 10 apples (down to a minmum delay)
            if (score % 3 == 0 && frameDelay > 50) {
                frameDelay -= 20; // increased speed
                printf("Speed increased! frameDelay = %d ms\n", frameDelay);
            }
            
            // Move the apple to some randome position
            reset_apple(psnake, papple);

            printf("Score: %d\n", score);
        }

        // If no apple eaten, remove tail
        if (!ateApple) {
            struct SnakeElement *prev = NULL;
            struct SnakeElement *curr = psnake;

            while (curr->pnext != NULL) {
                prev = curr;
                curr = curr->pnext;
            }

            if (prev != NULL) {
                // More than one node — safe to remove tail
                prev->pnext = NULL;
                free(curr);
            } else {
                // Only one node — don't remove or free
                // Nothing to do
            }
        }

        APPLE(papple->x, papple->y);
        draw_snake(psurface, psnake);
        DRAW_GRID();

        SDL_UpdateWindowSurface(window);
        SDL_Delay(frameDelay);
    }
}
