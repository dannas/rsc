#include <raylib.h>

// TODO(dannas): Remove global temp variable
// TODO(dannas): Collision with fruit
// TODO(dannas): Collision with self

#define SQUARE_SIZE 20
#define SNAKE_LEN 1024

typedef struct Snake {
    Vector2 pos;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food {
    Vector2 pos;
    Vector2 size;
    bool active;
    Color color;
} Food;

const int screenWidth = 800;
const int screenHeight = 600;
Vector2 offset = {screenWidth / SQUARE_SIZE, screenHeight / SQUARE_SIZE};
Snake snake[SNAKE_LEN];
int len = 3;
Snake temp[SNAKE_LEN];
Food fruit;
unsigned framesCounter;

void Init(void) {

    // Create snake
    for (int i = 0; i < SNAKE_LEN; i++) {
        snake[i] = (Snake) {
            .pos = { 3 * SQUARE_SIZE, 3 * SQUARE_SIZE },
            .size = { SQUARE_SIZE, SQUARE_SIZE },
            .speed = { 0.0f, 0.0f }
        };
        if (i == 0) {
            snake[i].color = DARKBLUE;
        } else {
            snake[i].color = BLUE;
        }
    }
    // Create fruit
    fruit = (Food) {
        .pos = {100, 100},
        .size = {SQUARE_SIZE, SQUARE_SIZE},
        .active = false,
        .color = RED
    };
}

void Update(void) {
    // Input handling
    if (IsKeyDown(KEY_RIGHT) && snake[0].speed.x == 0) {
        snake[0].speed = (Vector2){ SQUARE_SIZE, 0.0f };
    }
    if (IsKeyDown(KEY_LEFT) && snake[0].speed.x == 0) {
        snake[0].speed = (Vector2){ -SQUARE_SIZE, 0.0f };
    }
    if (IsKeyDown(KEY_UP) && snake[0].speed.y == 0) {
        snake[0].speed = (Vector2){ 0.0f, -SQUARE_SIZE };
    }
    if (IsKeyDown(KEY_DOWN) && snake[0].speed.y == 0) {
        snake[0].speed = (Vector2){ 0.0f, SQUARE_SIZE };
    }

    if (!fruit.active) {
        fruit.pos.x = GetRandomValue(0, offset.x-1) * SQUARE_SIZE;
        fruit.pos.y = GetRandomValue(0, offset.y-1) * SQUARE_SIZE;
        fruit.active = true;
    }

    // Snake movement
    if ((framesCounter % 5) == 0) {
        for (int i = 0; i < len; i++) {
            temp[i] = snake[i];
        }
        snake[0].pos.x += snake[0].speed.x;
        snake[0].pos.y += snake[0].speed.y;
        if (snake[0].pos.x == fruit.pos.x && snake[0].pos.y == fruit.pos.y) {
            len++;
            fruit.active = false;
        }
        for (int i = 1; i < len; i++) {
            snake[i].pos = temp[i-1].pos;
        }
    }
    // Traverse walls
    if (snake[0].pos.x >= screenWidth) {
        snake[0].pos.x = 0;
    }
    if (snake[0].pos.x < 0) {
        snake[0].pos.x = screenWidth-1;
    }
    if (snake[0].pos.y >= screenHeight) {
        snake[0].pos.y = 0;
    }
    if (snake[0].pos.y < 0) {
        snake[0].pos.y = screenHeight-1;
    }

    framesCounter++;
}

void Draw(void) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (int i = 0; i < len; i++) {
        DrawRectangleV(snake[i].pos, snake[i].size, snake[i].color);
    }
    if (fruit.active) {
        DrawRectangleV(fruit.pos, fruit.size, fruit.color);
    }
    EndDrawing();
}

int main(int argc, char *argv[]) {
    InitWindow(screenWidth, screenHeight, "WIP snake game");
    Init();
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        Update();
        Draw();
    }
    CloseWindow();
    return 0;
}

