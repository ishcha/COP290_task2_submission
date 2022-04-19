#include <SDL2/SDL.h>
#include <stdio.h>
#include <cstdlib>
#include "utils.hpp"

using namespace std;
class Particle {
    // location of particle
    int x, y;

    // velocity of particle
    int vx, vy; 

    int energy; // energy of particle

    bool has_key;

    public:
    Particle(int x, int y, int energy, int vx=1, int vy=1) {
        this->x = x;
        this->y = y;
        this->vx = vx;
        this->vy = vy;
        this->energy = energy;
        has_key = false;
    }

    void draw_particle(SDL_Renderer*& renderer) {
        SDL_Rect spot = {x*STRIDE, y*STRIDE, PARTICLE_WIDTH, PARTICLE_WIDTH};
        SDL_SetRenderDrawColor(renderer, BLUE[R], BLUE[G], BLUE[B], 0xFF);
        SDL_RenderFillRect(renderer, &spot);
    }

    bool show_energy(SDL_Renderer*& renderer, TTF_Font*& font) {
        std::ostringstream oss;
        oss << "Player1 Energy: " << energy << ", Key? " << has_key;
        std::string mystr = oss.str();
        // printf("%s \n", mystr.c_str());
        SDL_Color col = {(unsigned char)(WHITE[R]), (unsigned char)(WHITE[G]), (unsigned char)(WHITE[B])};
        SDL_Surface* textSurface = TTF_RenderText_Solid( font, mystr.c_str(), col );
        // printf("Till here\n");
        if( textSurface == NULL ) {
            printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
            SDL_FreeSurface( textSurface );
            return false;
        }
        else {
            //Create texture from surface pixels
            SDL_Texture* mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
            if( mTexture == NULL ) {
                printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
                SDL_FreeSurface( textSurface );
                return false;
            }
            else {
                SDL_SetRenderDrawColor(renderer, WHITE[R], WHITE[G], WHITE[B], 0xFF);
                SDL_Rect renderQuad = { 0, 0, swidth/2, EDGE_WIDTH*STRIDE };

                SDL_RenderCopyEx( renderer, mTexture, NULL, &renderQuad, 0, NULL, SDL_FLIP_NONE);
                SDL_DestroyTexture(mTexture);
                mTexture = NULL;
            }
            //Get rid of old surface
            SDL_FreeSurface( textSurface );
            
            return true;
        }
    }

    void update(int try_x, int try_y, unsigned char image[sheight][swidth][BYTES_PER_PIXEL], SDL_Renderer*& renderer, Mix_Chunk* all_sound_effects) {
        if (onroad(try_x, try_y, image)) {
            x = try_x;
            y = try_y;
            energy -= 2; 
            return;
        }
        if (on_goal(try_x, try_y, image) && has_key) {
            x = try_x;
            y = try_y;
            energy = 1000;
            return;
        }
        if (on_key(try_x, try_y, image)) {
            Mix_PlayChannel( -1, &all_sound_effects[0], 0 );
            has_key = true;
            colorRect(image, try_x, try_y, 1, 1, GREEN_BG);
            SDL_Rect spot = {try_x*STRIDE, try_y*STRIDE, PARTICLE_WIDTH, PARTICLE_WIDTH};
            SDL_SetRenderDrawColor(renderer, GREEN_BG[R], GREEN_BG[G], GREEN_BG[B], 0xFF);
            SDL_RenderFillRect(renderer, &spot);
            return;
        }
        if (on_energy(try_x, try_y, image)) {
            Mix_PlayChannel( -1, &all_sound_effects[1], 0 );
            x = try_x;
            y = try_y;
            if (energy + 12 <= 100) energy += 12;
            else if (energy < 100) energy = 100; // fully replenished
        }
        int type = on_obstacle(try_x, try_y, image);
        if (type) {
            x = try_x;
            y = try_y;
            if (type == 1) energy = 0; // dog
            else {energy /= 2; // half energy due to guard
            Mix_PlayChannel( -1, &all_sound_effects[4], 0 );}
            return;
        }
    }

    bool now_has_key() {return has_key;}

    bool lost() {
        return (energy <= 0);
    }

    bool won() {
        return (energy == 1000);
    }

    void update_energy(int e) {energy = e;}

    int get_energy() {return energy;}
    int get_x() {return x;}
    int get_y() {return y;}

    void handleEvent( SDL_Event& e, unsigned char image[sheight][swidth][BYTES_PER_PIXEL], SDL_Renderer*& renderer, Mix_Chunk* all_sound_effects) {
        if( e.type == SDL_KEYDOWN) {
            switch( e.key.keysym.sym ) {
                case SDLK_UP: update(x, y-vy, image, renderer, all_sound_effects); break;
                case SDLK_DOWN: update(x, y+vy, image, renderer, all_sound_effects); break;
                case SDLK_LEFT: update(x-vx, y, image, renderer, all_sound_effects); break;
                case SDLK_RIGHT: update(x+vx, y, image, renderer, all_sound_effects); break;
            }
        }
    }
};

class goal {
    int x, y; // position of goal

    public:
    goal(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void add_show_goal(unsigned char image[sheight][swidth][BYTES_PER_PIXEL], SDL_Renderer*& renderer) {
        colorRect(image, x, y, 1, 1, GOLD);
        SDL_Rect goal = {x*STRIDE, y*STRIDE, PARTICLE_WIDTH, PARTICLE_WIDTH};
        SDL_SetRenderDrawColor(renderer, GOLD[R], GOLD[G], GOLD[B], 0xFF);
        SDL_RenderFillRect(renderer, &goal);
    }
};

class estation { // energy station
    int x, y; // position of energy station

    public:
    estation() {}
    estation(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void set_estation(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void add_show_estation(unsigned char image[sheight][swidth][BYTES_PER_PIXEL], SDL_Renderer*& renderer) {
        colorRect(image, x, y, 1, 1, FIRE);
        SDL_Rect es = {x*STRIDE, y*STRIDE, PARTICLE_WIDTH, PARTICLE_WIDTH};
        SDL_SetRenderDrawColor(renderer, FIRE[R], FIRE[G], FIRE[B], 0xFF);
        SDL_RenderFillRect(renderer, &es);
    }
};

class obstacle {
    int x, y; // position of obstacle
    int vx, vy; // velocities of obstacle
    int dir;
    int moves;
    int disappear;
    int type;

    public:
    obstacle() {}
    obstacle(int x, int y, int type, int vx = 1, int vy = 1) {
        this->x = x;
        this->y = y;
        this->vx = vx;
        this->vy = vy;
        srand(moves);
        dir = rand()%4;
        moves = 0;
        disappear = 1;
        this->type = type;
    }

    void set_obstacle(int x, int y, int type, int vx = 1, int vy = 1) {
        this->x = x;
        this->y = y;
        this->vx = vx;
        this->vy = vy;
        srand(moves);
        dir = rand()%4;
        moves = 0;
        disappear = 1;
        this->type = type;
    }
    
    void add_show_obstacle(unsigned char image[sheight][swidth][BYTES_PER_PIXEL], SDL_Renderer*& renderer, Mix_Chunk* guard_frisk) {
        if (disappear) return;
        if (type == 1) { // dog
            colorRect(image, x, y, 1, 1, BRIGHTRED);
            SDL_Rect obs = {x*STRIDE, y*STRIDE, PARTICLE_WIDTH, PARTICLE_WIDTH};
            SDL_SetRenderDrawColor(renderer, BRIGHTRED[R], BRIGHTRED[G], BRIGHTRED[B], 0xFF);
            SDL_RenderFillRect(renderer, &obs);
        }
        else { // guard
            colorRect(image, x, y, 1, 1, DARKGREEN);
            SDL_Rect obs = {x*STRIDE, y*STRIDE, PARTICLE_WIDTH, PARTICLE_WIDTH};
            SDL_SetRenderDrawColor(renderer, DARKGREEN[R], DARKGREEN[G], DARKGREEN[B], 0xFF);
            SDL_RenderFillRect(renderer, &obs);
        }
        
    }

    void check_player_overlap(int x, int y, Particle& p, Mix_Chunk* guard_frisk) {
        if (p.get_x() == x && p.get_y() == y) {
            if (type == 1) {
                p.update_energy(0);
            }
            else {p.update_energy(p.get_energy()/2);
            Mix_PlayChannel( -1, guard_frisk, 0 );
            }
        }
    }

    bool can_move(int dir, unsigned char image[sheight][swidth][BYTES_PER_PIXEL], Particle& p, Mix_Chunk* guard_frisk) {
        switch(dir) {
            case 0: // up
                if (onroad(x, y-vy, image)) {
                    colorRect(image, x, y, 1, 1, ROADGRAY);
                    y -= vy;
                    check_player_overlap(x,y,p, guard_frisk);
                    return true;
                }
                break;
            case 2: // down
                if (onroad(x, y+vy, image)) {
                    colorRect(image, x, y, 1, 1, ROADGRAY);
                    y += vy;
                    check_player_overlap(x,y,p, guard_frisk);
                    return true;
                }
                break;
            case 3: // left
                if (onroad(x-vx, y, image)) {
                    colorRect(image, x, y, 1, 1, ROADGRAY);
                    x -= vx;
                    check_player_overlap(x,y,p, guard_frisk);
                    return true;
                }
                break;
            case 1: // right
                if (onroad(x+vx, y, image)) {
                    colorRect(image, x, y, 1, 1, ROADGRAY);
                    x += vx;
                    check_player_overlap(x,y,p, guard_frisk);
                    return true;
                }
                break;
            default:
                break;
        }
        return false;
    }

    void move_actuate(unsigned char image[sheight][swidth][BYTES_PER_PIXEL], Particle& p, Mix_Chunk* guard_frisk) {
        // after choosing random direction, move from dir -> dir + 1 -> dir + 2 -> dir + 3 (all mod 4)
        // choose first which is on road
        int i = 0;
        while (i < 4) {
            if (can_move((dir+i)%4, image, p, guard_frisk)) {
                dir = (dir+i)%4;
                break;
            }
            i++;
        }
    } 

    void move(unsigned char image[sheight][swidth][BYTES_PER_PIXEL], Particle& p, Mix_Chunk* guard_frisk) {
        if (moves % 3 == 0) {
            srand(moves);
            disappear = rand()%2;
        }
        moves++;
        if (moves % 10 == 0) { // randomize after 10 moves
            srand(moves);
            dir = rand()%4;
        }
        move_actuate(image, p, guard_frisk);
    }
};

void set_dogs(obstacle dogs[NDOGS]) {
    // assuming NDOGS = 3
    dogs[0].set_obstacle(3*width/8-2*EDGE_WIDTH, EDGE_WIDTH + height/5, 1);//12
    dogs[1].set_obstacle(9*width/16 - EDGE_WIDTH, height/2, 1);//16
    dogs[2].set_obstacle(width/8, EDGE_WIDTH, 1);//5
}

void set_stations(estation energy_stations[NESTATIONS]) {
    // assuming NESTATIONS = 6
    energy_stations[0].set_estation(width/8-EDGE_WIDTH, 3*height/5);
    energy_stations[1].set_estation(3*width/16, height/5);
    energy_stations[2].set_estation(width/4-EDGE_WIDTH + width/16, EDGE_WIDTH/3 + 4*height/15); 
    energy_stations[3].set_estation(width/2-2*EDGE_WIDTH, height/5 + EDGE_WIDTH); 
    energy_stations[4].set_estation(9*width/16 + 5*width/48 - 2*EDGE_WIDTH/3 + 5*width/48 + EDGE_WIDTH/3, height/2-EDGE_WIDTH); 
    energy_stations[5].set_estation(7*width/8 - EDGE_WIDTH + width/24, height/6); 
}

void set_guards(obstacle guards[NDOGS]) {
    // assuming NGUARDS = 5
    guards[0].set_obstacle(9*width/16 + 5*width/48 - 2*EDGE_WIDTH/3, EDGE_WIDTH + height/10 + 2*height/15, 2);//18
    guards[1].set_obstacle(width/8, 3*height/5, 2);//6
    guards[2].set_obstacle(width/8, EDGE_WIDTH + height/5, 2); //8
    guards[3].set_obstacle(width/4-EDGE_WIDTH, 4*EDGE_WIDTH/3 + 4*height/15, 2); //10
    guards[4].set_obstacle(width-EDGE_WIDTH, EDGE_WIDTH, 2); //3
}

void draw_key(unsigned char image[sheight][swidth][BYTES_PER_PIXEL], SDL_Renderer*& renderer) {
    int x = width/4, y = EDGE_WIDTH/3 + 4*height/15;
    colorRect(image, x, y, 1, 1, SILVER);
    SDL_Rect spot = {x*STRIDE, y*STRIDE, PARTICLE_WIDTH, PARTICLE_WIDTH};
    SDL_SetRenderDrawColor(renderer, SILVER[R], SILVER[G], SILVER[B], 0xFF);
    SDL_RenderFillRect(renderer, &spot);
}

int main() {
    unsigned char image[sheight][swidth][BYTES_PER_PIXEL];
    colorRect(image, 0, 0, width, height, GREEN_BG);
    makePaths(image);
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (!sdl_initialize(window, renderer)) {close(window, renderer); return -1;}
    TTF_Font* font = TTF_OpenFont("OpenSans-Bold.ttf", 1000);
    if (font == NULL) {printf("TTF_OpenFont: %s\n", TTF_GetError()); close(window, renderer); return -1;}
    bool quit = false;
    SDL_Event e;
    Particle p = *(new Particle(7*width/8-EDGE_WIDTH, EDGE_WIDTH, 100));
    goal g = *(new goal(width/2, 3*height/5 + EDGE_WIDTH));
    obstacle* dogs = new obstacle[NDOGS];
    obstacle* guards = new obstacle[NGUARDS];
    estation* energy_stations = new estation[NESTATIONS];
    set_dogs(dogs);
    set_guards(guards);
    set_stations(energy_stations);
    string updates = "";
    int* dog_update = new int[NDOGS];
    for(int i=0; i<NDOGS; i++) {
        srand((i+1)*NDOGS);
        dog_update[i] = (rand()%100); 
    }
     
    int* guard_update = new int[NGUARDS];
    for(int i=0; i<NGUARDS; i++) {
        srand((i+1)*NGUARDS);
        guard_update[i] = (rand()%100); 
    }

    Mix_Chunk* all_sound_effects = initialize_music();
 
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
            p.handleEvent(e, image, renderer, all_sound_effects);
        }
        SDL_SetRenderDrawColor( renderer, GREEN_BG[R], GREEN_BG[G], GREEN_BG[B], 0xFF );
        SDL_RenderClear(renderer);
        if (!load_game_screen(window, renderer)) {quit = true;}
        for(int i=0; i<NDOGS; i++) dog_update[i]++;
        for(int i=0; i<NGUARDS; i++) guard_update[i]++;
        if (!p.now_has_key()) draw_key(image, renderer);
        p.draw_particle(renderer);
        for(int i=0; i<NDOGS; i++) dogs[i].add_show_obstacle(image, renderer, &all_sound_effects[4]);
        for(int i=0; i<NGUARDS; i++) guards[i].add_show_obstacle(image, renderer, &all_sound_effects[4]);
        for(int i=0; i<NDOGS; i++) if (dog_update[i]%10 == 0) dogs[i].move(image, p, &all_sound_effects[4]);
        for(int i=0; i<NGUARDS; i++) if (guard_update[i]%15 == 0) guards[i].move(image, p, &all_sound_effects[4]);
        for(int i=0; i<NESTATIONS; i++) energy_stations[i].add_show_estation(image, renderer);
        g.add_show_goal(image, renderer);
        p.show_energy(renderer, font);
        SDL_RenderPresent(renderer);
        if (p.lost()) {
            lost_message(renderer, font, &all_sound_effects[3]);
            quit = true;
            SDL_RenderPresent(renderer);
            SDL_Delay(2000);
        }
        if (p.won()) {
            win_message(renderer, font, &all_sound_effects[2]);
            quit = true;
            SDL_RenderPresent(renderer);
            SDL_Delay(2000);
        }
    }
    close(window, renderer, font, all_sound_effects);
    return 0;
}