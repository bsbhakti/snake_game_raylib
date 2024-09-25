# include <iostream>
# include "raylib.h"
# include <deque>
#include <raymath.h>

int cellSize = 30;
int cellCount = 25;
double lastUpdateTime = 0;
int offset = 75;

bool TimePassed(double interval){
    double curTime = GetTime();
    if(curTime - lastUpdateTime >= interval){
        lastUpdateTime = curTime;
        return true;
    }
    return false;
}

bool ElementInDeque(Vector2 element, std::deque<Vector2> deque){
    for(int i = 0; i< deque.size(); i++){
        if(Vector2Equals(element, deque[i])){
            return true;
        }
    }
    return false;
}

class Food {
    public:
        Vector2 position; //stores (x,y) value in 1 variable
        Texture2D texture;

        Food(std::deque<Vector2> snakeBody){
            Image img = LoadImage("graphics/apple.png");
            texture = LoadTextureFromImage(img);
            UnloadImage(img);
            position = generateRandomPos(snakeBody);
        }
        ~Food(){
            UnloadTexture(texture);
        }

        void draw(){
            // DrawRectangle(position.x * cellSize, position.y * cellSize, cellSize, cellSize,RED);
            DrawTexture(texture,offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
        }

        Vector2 generateRandomCell(){
            float x = GetRandomValue(0, cellCount-1);
            float y = GetRandomValue(0, cellCount-1);
            return Vector2{x,y};

        }

        Vector2 generateRandomPos(std::deque<Vector2> snakeBody){
            Vector2 position = generateRandomCell();
            while(ElementInDeque(position, snakeBody)){
                position = generateRandomCell();
            }
            return position;
        }
};

class Snake {
    public:
        std::deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        Vector2 dir = Vector2{1,0};
        bool addSegement = false;

        void draw(){
            for(int i =0; i < body.size(); i++){
                Rectangle rec = Rectangle{offset + body[i].x * cellSize,offset + body[i].y * cellSize,(float)cellSize,(float) cellSize};
                DrawRectangleRounded(rec,0.5,6,BLACK);
            }
        }

        void update(){
            if(addSegement){
                body.push_front(Vector2Add(body[0], dir));
                addSegement = false;
            }
            else {
                 body.pop_back();
                body.push_front(Vector2Add(body[0], dir));
            }
        }

        void reset(){
            body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
            dir = Vector2{1,0};

        }

};


class Game{
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool isRunning = true;
        int score = 0;
        Sound eat;
        Sound wall;

        Game(){
            InitAudioDevice();
            eat = LoadSound("sounds/eat.mp3");
            wall = LoadSound("sounds/wall.mp3");
        }

        ~Game(){
            UnloadSound(eat);
            UnloadSound(wall);
            CloseAudioDevice();

        }


        void update(){
            if(isRunning){
                snake.update();
                checkCollision();
                checkCollisionWithWall();
                checkCollisionWithItself();
            }
        }

        void draw(){
            food.draw();
            snake.draw();
        }

        void checkCollision(){
            if(Vector2Equals(snake.body[0], food.position)){
                food.position = food.generateRandomPos(snake.body);
                snake.addSegement = true;
                score++;
                PlaySound(eat);
            }
        }

        void checkCollisionWithWall(){
            if(snake.body[0].x == cellCount ||  snake.body[0].x == -1 ||
             snake.body[0].y == -1 || snake.body[0].y == cellCount ){
                gameOver();
            }
        }

        void gameOver(){
            snake.reset();
            food.position = food.generateRandomPos(snake.body);
            isRunning = false;
            score = 0;
            PlaySound(wall);
        }

        void checkCollisionWithItself(){
            std::deque<Vector2> headless = snake.body;
            headless.pop_front();
            if(ElementInDeque(snake.body[0], headless)){
                gameOver();
            }
        }
};


int main(){
    InitWindow(2* offset +  cellSize * cellCount,2* offset +cellSize*cellCount,"Snake");
    SetTargetFPS(60);

    // Food food = Food();
    // Snake snake = Snake();
    Game game = Game();

    while(!WindowShouldClose()){ //window is open
        BeginDrawing();
        if(TimePassed(0.2)){
            game.update();
        }
        if(IsKeyPressed(KEY_UP) && game.snake.dir.y != 1){
            game.snake.dir = {0,-1};
            game.isRunning = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.dir.y != -1){
            game.snake.dir = {0,1};
            game.isRunning = true;

        }if(IsKeyPressed(KEY_RIGHT) && game.snake.dir.x != -1){
            game.snake.dir = {1,0};
            game.isRunning = true;

        }if(IsKeyPressed(KEY_LEFT) && game.snake.dir.x != 1){
            game.snake.dir = {-1,0};
            game.isRunning = true;

        }
        ClearBackground(YELLOW);
        Rectangle rec = Rectangle{(float)offset -5, (float)offset -5, float(cellSize * cellCount + 10), float(cellSize * cellCount + 10)};
        DrawRectangleLinesEx(rec,5,BLACK);
        // food.draw();
        // snake.draw();
        DrawText("My Snake Game", offset-5, 20, 40, BLACK);
        DrawText(TextFormat("%d",game.score), offset + cellSize * cellCount - 15, 20, 40, BLACK);

        game.draw();

        EndDrawing();
    }


    CloseWindow();
    return 0;
}