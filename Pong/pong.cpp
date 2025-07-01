#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <iostream>

#define WIDTH 640
#define HEIGHT 480
#define FONT_SIZE HEIGHT / 10
#define SIZE WIDTH / 40
#define SPEED HEIGHT / 50
#define PI 3.14159265358979323846
bool quit = false;
bool pause = false;
bool p2 = false;
int frameCount, timerFPS, lastFrame, fps, scoreLength; // fix for garbage text on screen

// Create window and renderer
SDL_Window* window = SDL_CreateWindow("Pong", WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

// Render font
TTF_Font* font;

// Render text to surface
SDL_Color textColor = {255, 255, 255, 255}; // White text

// Define destination rectangle
SDL_FRect lPaddle, rPaddle, ball, scoreBoard;
float velX, velY;
std::string score;
int lScore, rScore;
bool turn;

void serve() {
	lPaddle.y = rPaddle.y = (HEIGHT / 2) - (lPaddle.h / 2);
	if (turn) {
		ball.x = lPaddle.x + (lPaddle.w * 4);
		velX = SIZE / 2;
	}
	else {
		ball.x = rPaddle.x - (rPaddle.w * 4);
		velX = -SIZE / 2;
	}
	velY = 0;
	ball.y = HEIGHT / 2 - (SIZE / 2);
	turn = !turn;
}

void write(std::string text, int textLength, int x, int y) {
	const char* t = text.c_str();
	SDL_Surface *surface = TTF_RenderText_Solid(font, t, scoreLength, textColor);
	if (surface == nullptr) {
		std::cerr << "Failed to render text surface! TTF_Error: " << SDL_GetError() << std::endl;
		TTF_CloseFont(font);
		TTF_Quit();
		SDL_Quit();
	}
	        		
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == nullptr) {
		std::cerr << "Failed to create texture from surface! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_DestroySurface(surface);
		TTF_CloseFont(font);
		TTF_Quit();
		SDL_Quit();
	}
	
	scoreBoard.w = surface -> w;
	scoreBoard.h = surface -> h;
	scoreBoard.x = x - scoreBoard.w;
	scoreBoard.y = y - scoreBoard.h;
	SDL_DestroySurface(surface); // Destroy the surface after creating the texture
	SDL_RenderTexture(renderer, texture, nullptr, &scoreBoard);
	SDL_DestroyTexture(texture); // Destroy the texture
}

void update() {
	if (SDL_HasRectIntersectionFloat(&ball, &rPaddle)) {
		double rel = (rPaddle.y + (rPaddle.h / 2)) - (ball.y + (SIZE / 2));
		double norm = rel / (rPaddle.h / 2);
		double bounce = norm * (5 * PI / 12);
		velX= -SIZE * cos(bounce);
		velY= SIZE * -sin(bounce);
	}

	if (SDL_HasRectIntersectionFloat(&ball, &lPaddle)) {
		double rel = (lPaddle.y + (lPaddle.h / 2)) - (ball.y + (SIZE / 2));
		double norm = rel / (lPaddle.h / 2);
		double bounce = norm * (5 * PI / 12);
		velX = SIZE * cos(bounce);
		velY = SIZE * -sin(bounce);
	}

	if (!p2) {
		if (ball.y > rPaddle.y + (rPaddle.h / 2)) {
			rPaddle.y += SPEED;
		}
		if (ball.y < rPaddle.y + (rPaddle.h / 2)) {
			rPaddle.y -= SPEED;
		}
	}

	if (ball.x <= 0) {
		rScore++;
		serve();
	}
	if (ball.x + SIZE >= WIDTH) {
		lScore++;
		serve();
	}

	if (ball.y <= 0 || ball.y + SIZE >= HEIGHT) {
		velY = -velY;
	}

	ball.x += velX;
	ball.y += velY;
	score = std::to_string(lScore) + "     " + std::to_string(rScore);
	scoreLength = score.length();

	if (lPaddle.y < 0) {
		lPaddle.y = 0;
	}

	if (lPaddle.y + lPaddle.h > HEIGHT) {
		lPaddle.y = HEIGHT - lPaddle.h;
	}

	if (rPaddle.y < 0) {
		rPaddle.y = 0;
	}

	if (rPaddle.y + rPaddle.h > HEIGHT) {
		rPaddle.y = HEIGHT - rPaddle.h;
	}
}

void input() {
	SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_EVENT_QUIT) {
            quit = true;
        }
    }
	const bool *keystates = SDL_GetKeyboardState(NULL);
	if (keystates[SDL_SCANCODE_ESCAPE]) {
		quit = true;
	}
	if (keystates[SDL_SCANCODE_END]) {
		quit = true;
	}
	if (pause) {
		if (keystates[SDL_SCANCODE_RETURN]) {
			pause = false;
		}
		if (keystates[SDL_SCANCODE_KP_ENTER]) {
			pause = false;
		}
		if (keystates[SDL_SCANCODE_PAUSE]) {
			pause = false;
		}
	}
	else {
		if (keystates[SDL_SCANCODE_RETURN]) {
			pause = true;
		}
		if (keystates[SDL_SCANCODE_KP_ENTER]) {
			pause = true;
		}
		if (keystates[SDL_SCANCODE_PAUSE]) {
			pause = true;
		}
		if (keystates[SDL_SCANCODE_W]) {
			lPaddle.y -= SPEED;
		}
		if (keystates[SDL_SCANCODE_S]) {
			lPaddle.y += SPEED;
		}
		if (p2) {
			if (keystates[SDL_SCANCODE_2]) {
				p2 = false;
			}
			if (keystates[SDL_SCANCODE_UP]) {
				rPaddle.y -= SPEED;
			}
			if (keystates[SDL_SCANCODE_DOWN]) {
				rPaddle.y += SPEED;
			}
			if (keystates[SDL_SCANCODE_KP_8]) {
				rPaddle.y -= SPEED;
			}
			if (keystates[SDL_SCANCODE_KP_2]) {
				rPaddle.y += SPEED;
			}
		}
		else {
			if (keystates[SDL_SCANCODE_2]) {
				p2 = true;
			}
			if (keystates[SDL_SCANCODE_UP]) {
				lPaddle.y -= SPEED;
			}
			if (keystates[SDL_SCANCODE_DOWN]) {
				lPaddle.y += SPEED;
			}
			if (keystates[SDL_SCANCODE_KP_8]) {
				lPaddle.y -= SPEED;
			}
			if (keystates[SDL_SCANCODE_KP_2]) {
				lPaddle.y += SPEED;
			}
		}
	}
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);
    
	frameCount++;
	timerFPS = SDL_GetTicks() - lastFrame;
	if (timerFPS < (1000 / 60)) {
		SDL_Delay((1000 / 60 - timerFPS));
	}
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White objects
	// Draw the net
	for (int y = 0; y < HEIGHT; ++y)
	{
		if (y % 10)
		{
			SDL_RenderPoint(renderer, WIDTH / 2, y);
		}
	}
	SDL_RenderFillRect(renderer, &lPaddle);
	SDL_RenderFillRect(renderer, &rPaddle);
	SDL_RenderFillRect(renderer, &ball);
	write(score, scoreLength, WIDTH / 2 + FONT_SIZE * 3.5, FONT_SIZE * 2);
	SDL_RenderPresent(renderer);
}

// Main loop
int main(int argc, char* argv[]) {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: %s\n" << SDL_GetError() << std::endl;
		return -1;
	}

	// Initialize TTF
	if (TTF_Init() < 0) {
		std::cerr << "TTF could not initialize! SDL_Error: %s\n" << SDL_GetError() << std::endl;
		return -1;
	}

	font = TTF_OpenFont("../PressStart2P-Regular.ttf", FONT_SIZE);
    static int lastTime = 0;
    
    lScore = rScore = 0;
    lPaddle.x = 32; lPaddle.h = HEIGHT / 4;
    lPaddle.y = (HEIGHT / 2) - (lPaddle.h / 2);
    lPaddle.w = SIZE;
    rPaddle = lPaddle;
    rPaddle.x = WIDTH - rPaddle.w - 32;
    ball.w = ball.h = SIZE;
    
    serve();
    
    while (!quit) {
    	lastFrame = SDL_GetTicks();
    	if (lastFrame >= (lastTime + 1000)) {
    		lastTime = lastFrame;
    		fps = frameCount;
    		frameCount = 0;
    	}
    	if (!pause) {
			update();
		}
		input();
		render();
    }

    // Clean up
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
