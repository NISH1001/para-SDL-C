#include <SDL2/SDL.h>
#include <stdio.h>
#include "structs.h"
#define TRUE 1
#define FALSE 0

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 650

struct COLOR
{
	int r,g,b,a;
};

typedef struct COLOR Color;

struct Box
{
	float x,y;
	int w,h;
	Color color;
};

struct Wall
{
	struct Box attribute;
	int destroyed;
};

void renderBox(SDL_Renderer *renderer, struct Box *box)
{

	SDL_Rect rect = {box->x, box->y, box->w, box->h};
    SDL_SetRenderDrawColor(renderer, box->color.r,box->color.g, box->color.b,box->color.a);
    SDL_RenderFillRect(renderer, &rect);
}


void checkBoundary(struct Box *box)
{
	if(box->x <=0)
	{
		box->x = 0;
	}

	if((box->x + box->w) > SCREEN_WIDTH)
	{
		box->x = SCREEN_WIDTH-box->w;
	}

	if(box->y <=0)
	{
		box->y = 0;
	}

	if((box->y + box->h) > SCREEN_HEIGHT)
	{
		box->y = SCREEN_HEIGHT-box->h;
	}
}

int checkCollision(struct Box *box, struct Box *target)
{
	//The sides of the rectangles 
	int box_left, target_left;
	int box_right, target_right;
	int box_top, target_top;
	int box_bottom, target_bottom;

	//sides of box
	box_left = box->x;
	box_right = box->x + box->w;
	box_top = box->y;
	box_bottom = box->y + box->h;


	//sides of target
	target_left = target->x;
	target_right = target->x + target->w;
	target_top = target->y;
	target_bottom = target->y + target->h;

	//check sides
	if(box_bottom <= target_top)
	{
		return FALSE;
	}

	if(box_top >= target_bottom)
	{
		return FALSE;
	}

	if(box_right <= target_left)
	{
		return FALSE;
	}

	if(box_left >= target_right)
	{
		return FALSE;
	}

	//else return true
	return TRUE;
}

float xinc_bullet = 3;
float yinc_bullet = 6;

void fireBulletUp(SDL_Renderer *renderer, struct Box *box)
{
	box->y -= yinc_bullet;
	checkBoundary(box);
}

int main()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("cannot initialize\n");
		return -1;
	}

	SDL_Window *window =SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

	if(!window)
	{
		printf("cannot create window\n");
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(!renderer)
	{
		printf("cannot create renderer\n");	
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	int quit = FALSE;
	SDL_Event event;

	struct Box hero = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 50,50, {255,0,0,0}};
	hero.y = SCREEN_HEIGHT- hero.h;
	struct Box bullet = {hero.x, hero.y, 10, 10, {0,255,0,0}};
	bullet.x = hero.x + hero.w/2-bullet.w/2;
	bullet.y = hero.y - bullet.h;

	struct Box wallattr = {0, 0, 30, 30, {0,0,255,0}};
	struct Wall wall ={wallattr, FALSE};
	int wallnum = 5;
	struct Wall walls[5];
	struct Box wallsattributes[5] = {
										{SCREEN_WIDTH/2, 0, wallattr.w, wallattr.h, wallattr.color},
										{0,0, wallattr.w, wallattr.h, wallattr.color },
										{100,0, wallattr.w, wallattr.h, wallattr.color },
										{100,100, wallattr.w, wallattr.h, wallattr.color },
										{SCREEN_WIDTH-100,0, wallattr.w, wallattr.h, wallattr.color }
									};

	int i;
	for(i=0; i<5; i++)
	{
		walls[i].attribute = wallsattributes[i];
		walls[i].destroyed = FALSE;
	}


	float xinc_hero=8, yinc_hero=8;

	int bulletfired = FALSE;

	while(quit == FALSE)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit = TRUE;
					break;

				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
        					quit = TRUE;
        					break;

        				case SDLK_LEFT:
        					hero.x -= xinc_hero;
        					if(bulletfired == FALSE)
        					{
        						bullet.x = hero.x + hero.w/2-bullet.w/2;
        					}
        					break;

        				case SDLK_RIGHT:
        					hero.x += xinc_hero;
        					if(bulletfired == FALSE)
        					{
        						bullet.x = hero.x + hero.w/2-bullet.w/2;
        					}
        					break;

        				case SDLK_UP:
        					//hero.y -= yinc_hero;
        					break;

        				case SDLK_DOWN:
        					//hero.y += yinc_hero;
        					break;

        				case SDLK_SPACE:
        					bulletfired = TRUE;
        					break;
					}

					checkBoundary(&hero);
					break;
			}

		}

		if(bullet.y <=0)
		{
			bulletfired = FALSE;
		}

		if(bulletfired == FALSE)
		{
        	bullet.x = hero.x + hero.w/2-bullet.w/2;
			bullet.y = hero.y - bullet.h;
		}

		if(bulletfired == TRUE)
		{
			fireBulletUp(renderer, &bullet);
		}

		for(i=0; i<5; i++)
		{
			if((walls[i].destroyed==FALSE) &&(checkCollision(&bullet, &walls[i].attribute) == TRUE))
			{
				bulletfired = FALSE;
				walls[i].destroyed = TRUE;
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		renderBox(renderer, &hero);
		renderBox(renderer, &bullet);

		for(i=0; i<5; i++)
		{
			if(walls[i].destroyed == FALSE)
			{
				renderBox(renderer, &walls[i].attribute);
			}
		}
		SDL_RenderPresent(renderer);

		SDL_Delay(20);
	}


	printf("so far so good yay\n");

	return 0;
}