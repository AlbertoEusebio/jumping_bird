/*
	this is a clone of flappy bird	
	this is a game developed using the sdl2 library
	pleas note that if you want to compile it you have to use this command:

	gcc -Wall -o [NOME FILE EXECUTABLE]  [NOME FILE SORGENTE (.c)] $(pkg-config --cflags --libs sdl2) -lm	     ### SE solo SDL2
	gcc -Wall -o [NOME FILE EXECUTABLE]  [NOME FILE SORGENTE (.c)] $(pkg-config --cflags --libs SDL2_image) -lm  ### SE anche 		SDL2_image

	@EusebioAlberto
*/

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "my_functions.c"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CLOCKS_PER_MILLISECOND (CLOCKS_PER_SEC/1000)

typedef struct p{
	SDL_Rect body;
	SDL_Rect texture_progression;
	SDL_Texture *bird;
	int acceleration;
	_Bool accelerable;
}Player;

typedef struct o{
	SDL_Rect body;
	_Bool passed;
}Obstacle;


void Draw(Player *player, Obstacle field[][2], int size ,SDL_Renderer *renderer);
void UpdatePosition(Player *player, _Bool *ON);
void CheckBoundaries(Player * player, Obstacle field[][2], int size);
void CheckScore(Obstacle field[][2], int size, Player *player,int *score);
void CreateField(Obstacle field[][2], int size);
void UpdateField(Obstacle field[][2], int size);
void MoveField(Obstacle field[][2], int size);

int main(void){
	srand(time(0));
	if(SDL_Init(SDL_INIT_EVERYTHING)){
		printf("\nERROR INITIALIZING SDL: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	IMG_Init(IMG_INIT_PNG);

	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){ //I have to check it in this way because IMG_Init() returns the argument given
 		printf("\nERROR INITIALIZING SDL_image: %s\n", IMG_GetError());
		IMG_Quit();
		return 1;
	}

	//Creates the window and renderer structures
	SDL_Window *window = SDL_CreateWindow("FlappyBird", 
						SDL_WINDOWPOS_CENTERED, 
						SDL_WINDOWPOS_CENTERED, 
						SCREEN_WIDTH,SCREEN_HEIGHT, 
						SDL_WINDOW_SHOWN
					      );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	//This is the Player structure initialized
	Player player = {(SDL_Rect){SCREEN_WIDTH/2-20,SCREEN_HEIGHT/2-20,40,25},(SDL_Rect){0,0,40,25},IMG_LoadTexture(renderer, "./assets/bird.png"),2,1}; 

	int size = 10;
	Obstacle field[size][2];
	
	

	//creates a event structure
	SDL_Event event;

	clock_t start = 0,end=0,click_time;
	int score = 0;
	_Bool pressed = 0;
	_Bool ON = 1;
	_Bool ACTIVE = 1;
	while(ACTIVE){
		CreateField(field, size);
		while(ON){
			SDL_PollEvent(&event);
			if(event.type == SDL_QUIT){
				ON = 0;
				ACTIVE = 0;
			}
			if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && !pressed){
				start = clock();
				player.acceleration = -15;
				player.texture_progression.y = 25;
				pressed = 1; 
			}
			if(start != 0){
				end = clock();
				click_time = (end-start)/CLOCKS_PER_MILLISECOND;
				if(click_time >= 50){
					end = 0;
					start = 0;
					player.acceleration = 2;
				}
			}
			if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && pressed){
				end = 0;
				start = 0;
				player.acceleration = 2;
				player.texture_progression.y = 0;
				pressed = 0;
			}
			MoveField(field,size);
			UpdatePosition(&player, &ON);
			CheckBoundaries(&player,field,size);
			CheckScore(field, size, &player,&score);
			Draw(&player, field, size,renderer);

		}
		SDL_Texture *replay = IMG_LoadTexture(renderer, "./assets/replay.png");
		SDL_Rect replay_rect = {SCREEN_WIDTH/2-75,SCREEN_HEIGHT/2-50, 150,100};
		int d_x,d_y;
		while(1){
			SDL_PollEvent(&event);
			if(event.type == SDL_QUIT){
				ON = 0;
				ACTIVE = 0;
				break;
			}
			SDL_GetMouseState(&d_x,&d_y);
			d_x -= replay_rect.x;
			d_y -= replay_rect.y;
			if((d_x >= 0 && d_x <= replay_rect.w) && (d_y >= 0 && d_y <= replay_rect.h)){
				if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
					break;
				}
			}
			SDL_SetRenderDrawColor(renderer, 86, 148, 247, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(renderer);

			SDL_RenderCopy(renderer, player.bird, &player.texture_progression, &player.body);
			
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
			for(int i = 0; i<size; i++){
				if(field[i][0].body.x <= 798){
					SDL_RenderFillRect(renderer, &field[i][0].body);
					SDL_RenderFillRect(renderer, &field[i][1].body);
				}
			}
			SDL_RenderCopy(renderer, replay, NULL ,&replay_rect);
			SDL_RenderPresent(renderer);
			delay(10);
		}		
		player.body.y = SCREEN_HEIGHT/2-player.body.h;
		ON = 1;
		pressed = 0;
		score = 0;
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	IMG_Quit();

	return 0;
}

void Draw(Player *player, Obstacle field[][2], int size ,SDL_Renderer *renderer){

	SDL_SetRenderDrawColor(renderer, 86, 148, 247, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, player->bird, &player->texture_progression, &player->body);
	
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	for(int i = 0; i<size; i++){
		if(field[i][0].body.x <= 798){
			SDL_RenderFillRect(renderer, &field[i][0].body);
			SDL_RenderFillRect(renderer, &field[i][1].body);
		}
	}

	SDL_RenderPresent(renderer);
	delay(10);
}

void CreateField(Obstacle field[][2], int size){
	srand(time(0));
	int cone_width = 100;
	int cone_height = 0;
	int cone_space = 150;
	int cone_distance = 0;
	int x = SCREEN_WIDTH;
	int sum_diff = 1;
	int previous_height = 140; 
	for(int i = 0; i<size; i++){
		if(previous_height >= SCREEN_HEIGHT-cone_space-25){
			sum_diff = -1;
		}else if(previous_height <= 80){
			sum_diff = 1;			
		}
		else{
			sum_diff = rand()%2? 1 : -1;
		}
		do{
			cone_height = previous_height + rand()%(SCREEN_HEIGHT/2-60)*sum_diff;
		}while(abs(cone_height - previous_height) >= 60);

		cone_distance = 100 + rand()%((SCREEN_WIDTH/3)-cone_width);
 		field[i][0] = (Obstacle){{x,0, cone_width, cone_height},0};
		field[i][1] = (Obstacle){{x,cone_space+cone_height, cone_width, SCREEN_HEIGHT-(cone_space+cone_height)},0};
		previous_height = cone_height;
		x += (cone_width + cone_distance);
	}

}

void UpdateField(Obstacle field[][2], int size){
	srand(time(0));
	int cone_width = 100;
	int cone_height = 0;
	int cone_space = 150;
	int cone_distance = 0;
	int sum_diff = 1;
	int previous_height = field[size-2][0].body.h;
	int x = 0;
	for(int i = 0; i< size; i++){	
		if(field[i][0].body.x <= -field[i][0].body.w){
			for(int j = i; j< size-1; j++){
						field[j][0].body.x = field[j+1][0].body.x;
						field[j][0].body.y = field[j+1][0].body.y;
						field[j][0].body.w = field[j+1][0].body.w;
						field[j][0].body.h = field[j+1][0].body.h;
						field[j][0].passed = field[j+1][0].passed;						

						field[j][1].body.x = field[j+1][1].body.x;
						field[j][1].body.y = field[j+1][1].body.y;
						field[j][1].body.w = field[j+1][1].body.w;
						field[j][1].body.h = field[j+1][1].body.h;
						field[j][1].passed = field[j+1][1].passed;
			}
			cone_distance = 100+rand()%(SCREEN_WIDTH/3-cone_width);
			x = field[size-2][0].body.x+cone_width + cone_distance;
			if(previous_height >= SCREEN_HEIGHT-cone_space-25){
				sum_diff = -1;
			}else if(previous_height <= 50){
				sum_diff = 1;			
			}
			else{
				sum_diff = rand()%2? 1 : -1;
			}
			do{
				cone_height = previous_height + rand()%(SCREEN_HEIGHT/2-60)*sum_diff;
			}while(abs(cone_height - previous_height) >= 60);

			field[size-1][0].body.x = x;
			field[size-1][0].body.y = 0;
			field[size-1][0].body.w = cone_width;
			field[size-1][0].body.h = cone_height;
			field[size-1][0].passed = 0;			

			field[size-1][1].body.x = x;
			field[size-1][1].body.y = field[size-1][0].body.h+cone_space;
			field[size-1][1].body.w = cone_width;
			field[size-1][1].body.h = SCREEN_HEIGHT-(field[size-1][0].body.h+cone_space);
			field[size-1][1].passed = 0;
		}
	}

}

void MoveField(Obstacle field[][2], int size){
	for(int i = 0; i< size; i++){
		field[i][0].body.x += -2;
		field[i][1].body.x += -2;
		UpdateField(field, size);
	}
}

void UpdatePosition(Player *player, _Bool *ON){
	if(player->accelerable){
		player->body.y += player->acceleration;
	}else{
		*ON = 0;
		player->accelerable = 1;
	}
}

void CheckScore(Obstacle field[][2], const int size, Player *player,int *score){
	for(int i = 0; i<size;i++){
		if((player->body.x >= field[i][0].body.x+field[i][0].body.w) && (field[i][0].passed == 0)){
			(*score) += 1;
			field[i][0].passed = 1;
			printf("\nSCORE: %d\n", *score);
		}
	}
}

void CheckBoundaries(Player * player, Obstacle field[][2], int size){
	if(player->body.y <= (0-player->body.h) || player->body.y >= (SCREEN_HEIGHT-player->body.h)){
		player->accelerable = 0;
	}
	int d_x = 0;	
	int d_y_0 = 0;
	int d_y_1 = 0;
	for(int i = 0; i<size; i++){
		d_x = field[i][0].body.x - player->body.x;
		d_y_0 = player->body.y-(field[i][0].body.y+field[i][0].body.h);
		d_y_1 = field[i][1].body.y-player->body.y;

		if((d_x <= player->body.w && d_x >= -field[i][0].body.w) && (d_y_0 <= 0 || d_y_1 <= player->body.h)){
			player->accelerable = 0;
		}
	}
		
}

