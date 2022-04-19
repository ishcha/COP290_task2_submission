#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>
#include "enet.h"
#include <string>
#include <sstream>

#define EDGE_WIDTH 1 // width of the road
#define PARTICLE_WIDTH 20 // width of the moving particle
#define SCREEN_HEIGHT 500
#define SCREEN_WIDTH 1000
#define STRIDE 20   
#define NDOGS 3
#define NGUARDS 5
#define NESTATIONS 6
// #include <iostream>
const int BYTES_PER_PIXEL = 3; /// 3 pixels: red, green, & blue

const int height = 25; // use after multiplication with STRIDE
const int width = 50; // use after multiplication with STRIDE
const int sheight = height*STRIDE;
const int swidth = width*STRIDE;

enum rgb {R, G, B};

const int ROADGRAY[] = {119, 136, 153};
const int RED[] = {150, 0, 0};
const int GREEN_BG[] = {0, 150, 50};
const int BLUE[] = {0, 0, 150};
const int WHITE[] = {255, 255, 255};
const int BROWN[] = {100, 75, 0};
const int BRIGHTRED[] = {255, 0, 0};
const int GOLD[] = {255, 215, 0};
const int BRIGHTBLUE[] = {0, 0, 255};
const int DARKGREEN[] = {0, 200, 0};
const int SILVER[] = {192, 192, 192};
const int FIRE[] = {0, 255, 255}; // actually CYAN
const int PURPLE[] = {128,0,128};
const int BLACK[] = {0,0,0};

enum direction {
    UP, DOWN, LEFT, RIGHT
};

enum orientation {
    HORIZONTAL, VERTICAL
};

void colorRect(unsigned char image[sheight][swidth][BYTES_PER_PIXEL], int x, int y, int lx, int ly, const int color[3]) {
    int i, j;
    // printf("%d\n", y+ly*STRIDE);
    for (i = x*STRIDE; i < (x + lx)*STRIDE; i++) {
        for (j = y*STRIDE; j < (y + ly)*STRIDE; j++) {
            image[j][i][2] = (unsigned char) (color[R]);              ///red
            image[j][i][1] = (unsigned char) (color[G]);              ///green
            image[j][i][0] = (unsigned char) (color[B]);              ///blue
        }
    }
}

void makePaths(unsigned char image[sheight][swidth][BYTES_PER_PIXEL]) {
    colorRect(image, 0, EDGE_WIDTH, EDGE_WIDTH, height-EDGE_WIDTH, ROADGRAY);//1
    colorRect(image, 0, height-EDGE_WIDTH, width, EDGE_WIDTH, ROADGRAY);//2
    colorRect(image, width-EDGE_WIDTH, EDGE_WIDTH, EDGE_WIDTH, height-EDGE_WIDTH, ROADGRAY);//3
    colorRect(image, 0, EDGE_WIDTH, width, EDGE_WIDTH, ROADGRAY);//4
    colorRect(image, width/8, EDGE_WIDTH, EDGE_WIDTH, 3*height/5, ROADGRAY);//5
    colorRect(image, width/8, 3*height/5, 3*width/4, EDGE_WIDTH, ROADGRAY);//6
    colorRect(image, 7*width/8-EDGE_WIDTH, EDGE_WIDTH, EDGE_WIDTH, 3*height/5, ROADGRAY);//7
    colorRect(image, width/8, EDGE_WIDTH + height/5, width/8, EDGE_WIDTH, ROADGRAY);//8
    colorRect(image, width/4-EDGE_WIDTH, EDGE_WIDTH + height/5, EDGE_WIDTH, 2*height/5, ROADGRAY);//9
    colorRect(image, width/4-EDGE_WIDTH, 4*EDGE_WIDTH/3 + 4*height/15, width/8, EDGE_WIDTH, ROADGRAY);//10
    colorRect(image, 3*width/8-2*EDGE_WIDTH, EDGE_WIDTH + height/5, EDGE_WIDTH, 2*height/5, ROADGRAY);//11
    colorRect(image, 3*width/8-2*EDGE_WIDTH, EDGE_WIDTH + height/5, width/8, EDGE_WIDTH, ROADGRAY);//12
    colorRect(image, width/2-3*EDGE_WIDTH, EDGE_WIDTH + height/5, EDGE_WIDTH, 2*height/5, ROADGRAY);//13
    colorRect(image, 9*width/16 - EDGE_WIDTH, EDGE_WIDTH + height/10, 5*width/16 + EDGE_WIDTH, EDGE_WIDTH, ROADGRAY);//14
    colorRect(image, 9*width/16 - EDGE_WIDTH, EDGE_WIDTH + height/10, EDGE_WIDTH, 2*height/5, ROADGRAY);//15
    colorRect(image, 9*width/16 - EDGE_WIDTH, height/2, 5*width/16+EDGE_WIDTH, EDGE_WIDTH, ROADGRAY);//16
    colorRect(image, 9*width/16 + 5*width/48 - 2*EDGE_WIDTH/3, EDGE_WIDTH + height/10 + 2*height/15, EDGE_WIDTH, 4*height/15, ROADGRAY);//17
    colorRect(image, 9*width/16 + 5*width/48 - 2*EDGE_WIDTH/3, EDGE_WIDTH + height/10 + 2*height/15, 5*width/24 + 2*EDGE_WIDTH/3, EDGE_WIDTH, ROADGRAY);//18
    colorRect(image, 7*width/8 - EDGE_WIDTH, EDGE_WIDTH + height/6, width/12, EDGE_WIDTH, ROADGRAY);//19
    
}

bool on_goal(int x, int y, unsigned char image[sheight][swidth][BYTES_PER_PIXEL]) {
    int sx = x*STRIDE, sy = y*STRIDE;
    if ( ((int)(image[sy][sx][2]) == GOLD[R]) && ((int)(image[sy][sx][1]) == GOLD[G]) && ((int)(image[sy][sx][0]) == GOLD[B]) ) {
        return true;
    }
    return false;
}

bool on_key(int x, int y, unsigned char image[sheight][swidth][BYTES_PER_PIXEL]) {
    int sx = x*STRIDE, sy = y*STRIDE;
    if ( ((int)(image[sy][sx][2]) == SILVER[R]) && ((int)(image[sy][sx][1]) == SILVER[G]) && ((int)(image[sy][sx][0]) == SILVER[B]) ) {
        return true;
    }
    return false;
}

bool on_energy(int x, int y, unsigned char image[sheight][swidth][BYTES_PER_PIXEL]) {
    int sx = x*STRIDE, sy = y*STRIDE;
    if ( ((int)(image[sy][sx][2]) == FIRE[R]) && ((int)(image[sy][sx][1]) == FIRE[G]) && ((int)(image[sy][sx][0]) == FIRE[B]) ) {
        return true;
    }
    return false;
}

int on_obstacle(int x, int y, unsigned char image[sheight][swidth][BYTES_PER_PIXEL]) {
    int sx = x*STRIDE, sy = y*STRIDE;
    if ( ((int)(image[sy][sx][2]) == BRIGHTRED[R]) && ((int)(image[sy][sx][1]) == BRIGHTRED[G]) && ((int)(image[sy][sx][0]) == BRIGHTRED[B]) ) {
        return 1;
    }
    if ( ((int)(image[sy][sx][2]) == DARKGREEN[R]) && ((int)(image[sy][sx][1]) == DARKGREEN[G]) && ((int)(image[sy][sx][0]) == DARKGREEN[B]) ) {
        return 2;
    }
    return 0;
}

bool onroad(int x, int y, unsigned char image[sheight][swidth][BYTES_PER_PIXEL]) {
    // std::cout << "given:" << color[2] << " " << "actual: " << ROADGRAY[R] <<'\n';
    // std::cout << color[2] <<'\n';
    int sx = x*STRIDE, sy = y*STRIDE;
    if ((x < 0 || x > width-EDGE_WIDTH) || (y < EDGE_WIDTH || y > height-EDGE_WIDTH)) return false;
    if ( ((int)(image[sy][sx][2]) != ROADGRAY[R]) || ((int)(image[sy][sx][1]) != ROADGRAY[G]) || ((int)(image[sy][sx][0]) != ROADGRAY[B]) ) {
        // printf("%d %d \n", (int)(image[y][x][1]), ROADGRAY[]);
        return false;
    }
    return true;
}


bool sdl_initialize(SDL_Window*& window, SDL_Renderer*& renderer) {
	bool success = true;
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else {
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		window = SDL_CreateWindow( "IITD Champ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( window == NULL ) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else {
			//Create renderer for window
			renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
			if( renderer == NULL ) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				SDL_SetRenderDrawColor( renderer, GREEN_BG[R], GREEN_BG[G], GREEN_BG[B], 0xFF );

                #if defined(SDL_IMAGE_MAJOR_VERSION)
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
                #endif
                if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool enet_initialization(ENetAddress& address, ENetHost*& server) {
    
    address.host = 122.176.198.9;
    /* Bind the server to port 1234. */
    address.port = 1234;
    server = enet_host_create (& address, 32, 2, 0, 0);
    if (server == NULL) {
        printf ("An error occurred while trying to create an ENet server host.\n");
        return false;
    }
    return true;
}

void close(SDL_Window*& window, SDL_Renderer*& renderer, TTF_Font*& font, ENetHost*& server) {
    TTF_CloseFont(font);
    font = NULL;
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    window = NULL;
    renderer = NULL;
    SDL_Quit();
    enet_host_destroy(server);
}

void close(SDL_Window*& window, SDL_Renderer*& renderer, TTF_Font*& font) {
    TTF_CloseFont(font);
    font = NULL;
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    window = NULL;
    renderer = NULL;
    SDL_Quit();
}

void close(SDL_Window*& window, SDL_Renderer*& renderer) {
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    window = NULL;
    renderer = NULL;
    SDL_Quit();
}

void lost_message(SDL_Renderer*& renderer, TTF_Font* font) {
    std::string mystr = "You lost!";
    SDL_Color col = {(unsigned char)(BRIGHTRED[R]), (unsigned char)(BRIGHTRED[G]), (unsigned char)(BRIGHTRED[B])};
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, mystr.c_str(), col );
    if( textSurface == NULL ) {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
        SDL_FreeSurface( textSurface );
    }
    else {
        SDL_Texture* mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if( mTexture == NULL ) {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
            SDL_FreeSurface( textSurface );
        }
        else {
            // SDL_SetRenderDrawColor(renderer)
            SDL_Rect renderQuad = { 0, 0, swidth, sheight };

            SDL_RenderCopyEx( renderer, mTexture, NULL, &renderQuad, 0, NULL, SDL_FLIP_NONE);
            SDL_DestroyTexture(mTexture);
            mTexture = NULL;
        }
        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
}

void win_message(SDL_Renderer*& renderer, TTF_Font* font) {
    std::string mystr = "You Won!";
    SDL_Color col = {(unsigned char)(BRIGHTBLUE[R]), (unsigned char)(BRIGHTBLUE[G]), (unsigned char)(BRIGHTBLUE[B])};
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, mystr.c_str(), col );
    if( textSurface == NULL ) {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
        SDL_FreeSurface( textSurface );
    }
    else {
        SDL_Texture* mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if( mTexture == NULL ) {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
            SDL_FreeSurface( textSurface );
        }
        else {
            // SDL_SetRenderDrawColor(renderer)
            SDL_Rect renderQuad = { 0, 0, swidth, sheight };

            SDL_RenderCopyEx( renderer, mTexture, NULL, &renderQuad, 0, NULL, SDL_FLIP_NONE);
            SDL_DestroyTexture(mTexture);
            mTexture = NULL;
        }
        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
}

bool load_game_screen(SDL_Window*& window, SDL_Renderer*& renderer) {
    SDL_Surface* game_screen = SDL_LoadBMP("iitdmapImage.bmp");
    if (game_screen == NULL) {
        printf( "Unable to load image! SDL Error: %s\n", SDL_GetError() );
        return false;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, game_screen);
    SDL_FreeSurface( game_screen );
    SDL_RenderCopy( renderer, texture, NULL, NULL );
    SDL_Rect topbar = {0, 0, swidth, EDGE_WIDTH*STRIDE};
    SDL_SetRenderDrawColor(renderer, BROWN[R], BROWN[G], BROWN[B], 0xFF);
    SDL_RenderFillRect(renderer, &topbar);

    // SDL_DestroyTexture(texture);
    texture = NULL;
    return true;
}