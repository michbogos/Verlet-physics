#include <SDL2/SDL.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 1000

#define QUITKEY SDLK_ESCAPE
#define WIDTH 700
#define HEIGHT 700
#define SUBSTEPS 8
#define SUB_COEFF 1.0/SUBSTEPS/SUBSTEPS

SDL_Window* screen = NULL;
SDL_Renderer* renderer;
SDL_Event event;
SDL_Rect source, destination, dst;

int errorCount = 0;
int keypressed;
int rectCount = 0;

void drawcircle(int x0, int y0, int radius)
{
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        SDL_RenderDrawPoint(renderer, x0 + x, y0 + y);
        SDL_RenderDrawPoint(renderer, x0 + y, y0 + x);
        SDL_RenderDrawPoint(renderer, x0 - y, y0 + x);
        SDL_RenderDrawPoint(renderer, x0 - x, y0 + y);
        SDL_RenderDrawPoint(renderer, x0 - x, y0 - y);
        SDL_RenderDrawPoint(renderer, x0 - y, y0 - x);
        SDL_RenderDrawPoint(renderer, x0 + y, y0 - x);
        SDL_RenderDrawPoint(renderer, x0 + x, y0 - y);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

int length = 0;
float balls[4*MAX];
int mouseX, mouseY;

void addBall(int x, int y){
    balls[2*length] = x;
    balls[2*length+1] = y;
    balls[2*length+2] = x;
    balls[2*length+3] = y;
    length++;
}

void LogError(char* msg) {
	printf("%s\n", msg);
	errorCount++;
}
/* Initialize all setup, set screen mode, load images etc */
void InitSetup() {
	srand((int)time(NULL));
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &screen, &renderer);
	if (!screen) {
		LogError("InitSetup failed to create window");
	}
}

void end() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(screen);
	SDL_Quit();
	exit(0);
}


void Draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    for(int i = 0; i < length; i++){
        drawcircle(balls[2*i], balls[2*i+1], 50);
    }
	SDL_RenderPresent(renderer);
}

void Update(){
    for(int i = 0; i < length; i++){
        float posx = balls[6*i];
        float posy = balls[6*i+1];
        float prex = balls[6*i+2];
        float prey = balls[6*i+3];
        const float vx = posx-prex;
        const float vy = posy-prey;
        balls[6*i+1] += vx + 1000*SUB_COEFF*0.0001;
        balls[6*i+1] += vy + 1000*SUB_COEFF*0.0001;
        balls[6*i+2] = posx;
        balls[6*i+3] = posy;
    }
}

void GameLoop() {
	int gameRunning = 1;
	while (gameRunning)
	{
		Draw();
        Update();
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    SDL_GetMouseState(&mouseX, &mouseY);
                    addBall(mouseX, mouseY);
                case SDL_KEYDOWN:
                    keypressed = event.key.keysym.sym;
                    if (keypressed == QUITKEY)
                    {
                        gameRunning = 0;
                        break;
                    }

                    break;
                case SDL_QUIT: /* if mouse click to close window */
                {
                    gameRunning = 0;
                    break;
                }
                case SDL_KEYUP: {
                    break;
                }
			} 

		}
	}
}

int main(int argc, char* args[])
{
    memset(balls, -1, MAX);
	InitSetup();
	GameLoop();
	end();
	return 0;
}
