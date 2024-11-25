#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRID_SIZE 20

typedef struct {
    int x, y;
} Segment;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

Segment snake[100];
int snake_length = 1;
Segment food;
Segment poisonousFood;
Direction direction = RIGHT;
bool isRunning = true;
bool isPoisonousFoodVisible = false;
Uint32 poisonousFoodSpawnTime = 0;
int score = 0;
int foodsEaten = 0;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (TTF_Init() < 0) return false;
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;
    font = TTF_OpenFont("Limon.ttf", 24);  // Make sure you have Arial font installed
    if (!font) return false;
    return true;
}

void spawnFood() {
    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE);
}

void spawnPoisonousFood() {
    poisonousFood.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
    poisonousFood.y = rand() % (SCREEN_HEIGHT / GRID_SIZE);
    isPoisonousFoodVisible = true;
    poisonousFoodSpawnTime = SDL_GetTicks();
}

void handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP: if (direction != DOWN) direction = UP; break;
                case SDLK_DOWN: if (direction != UP) direction = DOWN; break;
                case SDLK_LEFT: if (direction != RIGHT) direction = LEFT; break;
                case SDLK_RIGHT: if (direction != LEFT) direction = RIGHT; break;
            }
        }
    }
}

void update() {
    for (int i = snake_length - 1; i > 0; --i) snake[i] = snake[i - 1];
    switch (direction) {
        case RIGHT: snake[0].x++; break;
        case LEFT: snake[0].x--; break;
        case UP: snake[0].y--; break;
        case DOWN: snake[0].y++; break;
    }
    if (snake[0].x < 0) snake[0].x = (SCREEN_WIDTH / GRID_SIZE) - 1;
    if (snake[0].x >= SCREEN_WIDTH / GRID_SIZE) snake[0].x = 0;
    if (snake[0].y < 0) snake[0].y = (SCREEN_HEIGHT / GRID_SIZE) - 1;
    if (snake[0].y >= SCREEN_HEIGHT / GRID_SIZE) snake[0].y = 0;
    if (snake[0].x == food.x && snake[0].y == food.y) {
        snake_length += 3;
        score += 10;
        spawnFood();
        foodsEaten++;
        if (foodsEaten % 4 == 0) spawnPoisonousFood();
    }
    if (isPoisonousFoodVisible && snake[0].x == poisonousFood.x && snake[0].y == poisonousFood.y) {
        score -= 10;
        isPoisonousFoodVisible = false;
        if (score < 0) isRunning = false;
    }
    if (isPoisonousFoodVisible && SDL_GetTicks() - poisonousFoodSpawnTime > 4000) isPoisonousFoodVisible = false;
    for (int i = 1; i < snake_length; ++i) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) isRunning = false;
    }
}

void renderScore() {
    char scoreText[20];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, scoreText, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect foodRect = {food.x * GRID_SIZE, food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE};
    SDL_RenderFillRect(renderer, &foodRect);
    if (isPoisonousFoodVisible) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_Rect poisonousFoodRect = {poisonousFood.x * GRID_SIZE, poisonousFood.y * GRID_SIZE, GRID_SIZE, GRID_SIZE};
        SDL_RenderFillRect(renderer, &poisonousFoodRect);
    }
    for (int i = 0; i < snake_length; ++i) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect snakeRect = {snake[i].x * GRID_SIZE, snake[i].y * GRID_SIZE, GRID_SIZE, GRID_SIZE};
        SDL_RenderFillRect(renderer, &snakeRect);
    }
    renderScore();
    SDL_RenderPresent(renderer);
}

void cleanup() {
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
    snake[0].x = 10;
    snake[0].y = 10;
    if (!init()) return -1;
    spawnFood();
    while (isRunning) {
        handleInput();
        update();
        render();
        SDL_Delay(100);
    }
    cleanup();
    return 0;
}
