#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <iostream>

#define WIDTH 640
#define HEIGHT 480
#define FONT_SIZE HEIGHT/10
#define SIZE WIDTH/40
#define SPEED HEIGHT/50
#define PI 3.14159265358979323846
bool quit = false;
int frameCount, timerFPS, lastFrame, fps, scorel; // fix for garbage text on screen

// Create window and renderer
SDL_Window* window = SDL_CreateWindow("Pong", WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

// Render font
TTF_Font* font;

// Render text to surface
SDL_Color textColor = {255, 255, 255, 255}; // White text

// Define destination rectangle
SDL_FRect l_paddle, r_paddle, ball, score_board;
float velX, velY;
std::string score;
int l_s, r_s;
bool turn;

void serve() {
	l_paddle.y=r_paddle.y=(HEIGHT/2)-(l_paddle.h/2);
	if(turn) {
		ball.x=l_paddle.x+(l_paddle.w*4);
		velX=SIZE/2;
	}
	else {
		ball.x=r_paddle.x-(r_paddle.w*4);
		velX=-SIZE/2;
	}
	velY=0;
	ball.y=HEIGHT/2-(SIZE/2);
	turn=!turn;
}

void write(std::string text, int x, int y) {    
	const char* t = text.c_str();
	SDL_Surface *surface = TTF_RenderText_Solid(font, t, scorel, textColor);
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
	
	score_board.w=surface->w;
	score_board.h=surface->h;
	score_board.x=x-score_board.w;
	score_board.y=y-score_board.h;
	SDL_DestroySurface(surface); // Destroy the surface after creating the texture
	SDL_RenderTexture(renderer, texture, nullptr, &score_board);
	SDL_DestroyTexture(texture); // Destroy the texture
}

void update() {
	if(SDL_HasRectIntersectionFloat(&ball, &r_paddle)) {
		double rel = (r_paddle.y+(r_paddle.h/2))-(ball.y+(SIZE/2));
		double norm = rel/(r_paddle.h/2);
		double bounce = norm * (5*PI/12);
		velX=-SIZE*cos(bounce);
		velY=SIZE*-sin(bounce);
	}
	if(SDL_HasRectIntersectionFloat(&ball, &l_paddle)) {
		double rel = (l_paddle.y+(l_paddle.h/2))-(ball.y+(SIZE/2));
		double norm = rel/(l_paddle.h/2);
		double bounce = norm * (5*PI/12);
		velX=SIZE*cos(bounce);
		velY=SIZE*-sin(bounce);
	}
	if(ball.y > r_paddle.y+(r_paddle.h/2)) r_paddle.y+=SPEED;
	if(ball.y < r_paddle.y+(r_paddle.h/2)) r_paddle.y-=SPEED;
	if(ball.x<=0) {
		r_s++;serve();
	}
	if(ball.x+SIZE>=WIDTH) {
		l_s++;serve();
	}
	if(ball.y<=0 || ball.y+SIZE>=HEIGHT) velY=-velY;
	ball.x+=velX;
	ball.y+=velY;
	score=std::to_string(l_s) + "     " + std::to_string(r_s);
	scorel=score.length();
	if(l_paddle.y<0)l_paddle.y=0;
	if(l_paddle.y+l_paddle.h>HEIGHT)l_paddle.y=HEIGHT-l_paddle.h;
	if(r_paddle.y<0)r_paddle.y=0;
	if(r_paddle.y+r_paddle.h>HEIGHT)r_paddle.y=HEIGHT-r_paddle.h;
}

void input() {
	SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_EVENT_QUIT) {
            quit = true;
        }
    }
	const bool *keystates = SDL_GetKeyboardState(NULL);
	if(keystates[SDL_SCANCODE_ESCAPE]) quit=true;
	if(keystates[SDL_SCANCODE_UP]) l_paddle.y-=SPEED;
	if(keystates[SDL_SCANCODE_DOWN]) l_paddle.y+=SPEED;
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);
    
	frameCount++;
	timerFPS = SDL_GetTicks()-lastFrame;
	if(timerFPS<(1000/60)) {
		SDL_Delay((1000/60-timerFPS));
	}
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White objects
	SDL_RenderFillRect(renderer, &l_paddle);
	SDL_RenderFillRect(renderer, &r_paddle);
	SDL_RenderFillRect(renderer, &ball);
	write(score, WIDTH/2+FONT_SIZE*3.5, FONT_SIZE*2);
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
    
    l_s=r_s=0;
    l_paddle.x=32; l_paddle.h=HEIGHT/4;
    l_paddle.y=(HEIGHT/2)-(l_paddle.h/2);
    l_paddle.w=SIZE;
    r_paddle=l_paddle;
    r_paddle.x=WIDTH-r_paddle.w-32;
    ball.w=ball.h=SIZE;
    
    serve();
    
    while (!quit) {
    	lastFrame=SDL_GetTicks();
    	if(lastFrame>=(lastTime+1000)) {
    		lastTime=lastFrame;
    		fps=frameCount;
    		frameCount=0;
    	}
        update();
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