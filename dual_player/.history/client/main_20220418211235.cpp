#include <stdio.h>
#include <cstdlib>
#include "utils.hpp"
#include <iostream>
#include <cstring>

using namespace std;

class Particle {
    // location of particle
    int x, y;//x, y

    // velocity of particle
    int vx, vy; 

    int energy; // energy of particle // e

    bool has_key; // h

    public:
    Particle() {}
    Particle(int x, int y, int energy, int vx=1, int vy=1) {
        this->x = x;
        this->y = y;
        this->vx = vx;
        this->vy = vy;
        this->energy = energy;
        has_key = false;
    }

    void draw_particle(SDL_Renderer*& renderer, const int color[3]) {
        SDL_Rect spot = {x*STRIDE, y*STRIDE, PARTICLE_WIDTH, PARTICLE_WIDTH};
        SDL_SetRenderDrawColor(renderer, color[R], color[G], color[B], 0xFF);
        SDL_RenderFillRect(renderer, &spot);
    }


    string update(int try_x, int try_y, unsigned char image[sheight][swidth][BYTES_PER_PIXEL], SDL_Renderer*& renderer) {
        ostringstream changes;
        if (onroad(try_x, try_y, image)) {
            x = try_x;
            y = try_y;
            energy -= 2; 
            changes << "x " << x << ' ' << "y " << y << ' ' << "e " << energy << ' ' << '\n';
        }
        else if (on_goal(try_x, try_y, image) && has_key) {
            x = try_x;
            y = try_y;
            energy = 1000;
            changes << "x " << x << ' ' << "y " << y << ' ' << "e " << energy << ' ' << '\n';
        }
        else if (on_key(try_x, try_y, image)) {
            has_key = true;
            colorRect(image, try_x, try_y, 1, 1, GREEN_BG);
            SDL_Rect spot = {try_x*STRIDE, try_y*STRIDE, PARTICLE_WIDTH, PARTICLE_WIDTH};
            SDL_SetRenderDrawColor(renderer, GREEN_BG[R], GREEN_BG[G], GREEN_BG[B], 0xFF);
            SDL_RenderFillRect(renderer, &spot);
            changes << "h "  << '\n';
        }
        else if (on_energy(try_x, try_y, image)) {
            x = try_x;
            y = try_y;
            if (energy + 12 <= 100) energy += 12;
            else if (energy < 100) energy = 100; // fully replenished
            changes << "x " << x << ' ' << "y " << y << ' ' << "e " << energy << ' ' << '\n';
        }
        else if (on_obstacle(try_x, try_y, image)) {
            int type = on_obstacle(try_x, try_y, image);
            x = try_x;
            y = try_y;
            if (type == 1) energy = 0; // dog
            else energy /= 2; // half energy due to guard
            changes << "x " << x << ' ' << "y " << y << ' ' << "e " << energy << ' ' << '\n';
        }
        if (changes.str() == "") return "\n";
        return changes.str();
    }

    void set_x(int x) {this->x = x;}

    void set_y(int y) {this->y = y;}

    void set_energy(int e) {energy = e;}

    bool now_has_key() {return has_key;}

    void set_has_key() {has_key = true;}

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

    string handleEvent( SDL_Event& e, unsigned char image[sheight][swidth][BYTES_PER_PIXEL], SDL_Renderer*& renderer) {
        printf("handling p2 events\n");
        if( e.type == SDL_KEYDOWN) {
            switch( e.key.keysym.sym ) {
                case SDLK_w: printf("up"); return update(x, y-vy, image, renderer); break; // up
                case SDLK_s: printf("down"); return update(x, y+vy, image, renderer); break; // down
                case SDLK_a: printf("left"); return update(x-vx, y, image, renderer); break; // left
                case SDLK_d: printf("right"); return update(x+vx, y, image, renderer); break; // right
            }
        }
        return "";
    }
};

bool show_energy(SDL_Renderer*& renderer, TTF_Font*& font, Particle p1, Particle p2) {
        std::ostringstream oss;
        oss << "Energy: " << "   " << p1.get_energy() << "    "<< p2.get_energy() <<"; Key " << "   " << p1.now_has_key() <<"    " << p2.now_has_key();
        std::string mystr = oss.str();
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
                SDL_Rect blue_e = {(width/8 + EDGE_WIDTH)*STRIDE, 0, PARTICLE_WIDTH, PARTICLE_WIDTH};
                SDL_SetRenderDrawColor(renderer, BLUE[R], BLUE[G], BLUE[B], 0xFF);
                SDL_RenderFillRect(renderer, &blue_e);
                SDL_Rect red_e = {(width/4)*STRIDE - 3*EDGE_WIDTH*STRIDE/4, 0, PARTICLE_WIDTH, PARTICLE_WIDTH};
                SDL_SetRenderDrawColor(renderer, BLACK[R], BLACK[G], BLACK[B], 0xFF);
                SDL_RenderFillRect(renderer, &red_e);
                SDL_Rect blue_k = {(width/4 + 8*EDGE_WIDTH)*STRIDE, 0, PARTICLE_WIDTH, PARTICLE_WIDTH};
                SDL_SetRenderDrawColor(renderer, BLUE[R], BLUE[G], BLUE[B], 0xFF);
                SDL_RenderFillRect(renderer, &blue_k); 
                SDL_Rect black_k = {(width/2-2*EDGE_WIDTH)*STRIDE, 0, PARTICLE_WIDTH, PARTICLE_WIDTH};
                SDL_SetRenderDrawColor(renderer, BLACK[R], BLACK[G], BLACK[B], 0xFF);
                SDL_RenderFillRect(renderer, &black_k); 
                SDL_DestroyTexture(mTexture);
                mTexture = NULL;
            }
            //Get rid of old surface
            SDL_FreeSurface( textSurface );
            
            return true;
        }
    }

class goal {
    int x, y; // position of goal

    public:
    goal() {}
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
    int x, y; // position of obstacle // x, y
    int vx, vy; // velocities of obstacle
    int dir; // d
    int moves; // m
    int disappear; // p
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

    void set_moves (int m) {moves = m;}

    void set_disappear(int d) {disappear = d;}

    void set_direction(int d) {dir = d;}

    void set_x(int a) {x =a;}
    void set_y(int a) {y =a;}
    
    void add_show_obstacle(unsigned char image[sheight][swidth][BYTES_PER_PIXEL], SDL_Renderer*& renderer) {
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

    string check_player_overlap(int x, int y, Particle& p1, Particle& p2) {
        ostringstream changes;
        if ((p2.get_x() == x && p2.get_y() == y) && (type == 2)) {
            p2.update_energy(p2.get_energy()/2);
            changes << "p e " << p2.get_energy() << ' ' <<'\n';
        }
        if (p1.get_x() == x && p1.get_y() == y) {
            if (type == 1) {
                p1.update_energy(0);
                p2.update_energy(1000);
                changes << "P e " << p1.get_energy() << ' ' <<'\n';
                changes << "p e " << p2.get_energy() << ' ' <<'\n';
            }
            else {p1.update_energy(p1.get_energy()/2);
                changes << "P e " << p1.get_energy() << ' ' <<'\n';
            }
        }
        else if (p2.get_x() == x && p2.get_y() == y) {
            if (type == 1) {
                p2.update_energy(0);
                p1.update_energy(1000);
                changes << "P e " << p1.get_energy() << ' ' <<'\n';
                changes << "p e " << p2.get_energy() << ' ' <<'\n';
            }
        }
        if (changes.str() == "") return "\n";
        return changes.str();
    }

    string can_move(int dir, unsigned char image[sheight][swidth][BYTES_PER_PIXEL], Particle& p1, Particle& p2) {
        ostringstream changes;
        switch(dir) {
            case 0: // up
                if (onroad(x, y-vy, image)) {
                    colorRect(image, x, y, 1, 1, ROADGRAY);
                    y -= vy;
                    changes << "y " << y <<  ' ' << '\n' << check_player_overlap(x,y,p1,p2);
                }
                else changes << '\n';
                break;
            case 2: // down
                if (onroad(x, y+vy, image)) {
                    colorRect(image, x, y, 1, 1, ROADGRAY);
                    y += vy;
                    changes << "y " << y << ' ' << '\n' << check_player_overlap(x,y,p1,p2);
                }
                else changes << '\n';
                break;
            case 3: // left
                if (onroad(x-vx, y, image)) {
                    colorRect(image, x, y, 1, 1, ROADGRAY);
                    x -= vx;
                    changes << "x " << x << ' ' << '\n' << check_player_overlap(x,y,p1,p2);
                }
                else changes << '\n';
                break;
            case 1: // right
                if (onroad(x+vx, y, image)) {
                    colorRect(image, x, y, 1, 1, ROADGRAY);
                    x += vx;
                    changes << "x " << x << ' ' << '\n' << check_player_overlap(x,y,p1,p2);
                }
                else changes << '\n';
                break;
            default:
                break;
        }
        return changes.str();
    }

    string move_actuate(unsigned char image[sheight][swidth][BYTES_PER_PIXEL], Particle& p1, Particle& p2) {
        // after choosing random direction, move from dir -> dir + 1 -> dir + 2 -> dir + 3 (all mod 4)
        // choose first which is on road
        ostringstream changes ;
        string out;
        int i = 0;
        while (i < 4) {
            out = can_move((dir+i)%4, image, p1, p2);
            if (out != "\n") {
                dir = (dir+i)%4;
                break;
            }
            i++;
        }
        changes << "d " << dir << ' '<< out;
        return changes.str();
    } 

    string move(unsigned char image[sheight][swidth][BYTES_PER_PIXEL], Particle& p1, Particle& p2) {
        if (moves % 3 == 0) {
            srand(moves);
            disappear = rand()%2;
        }
        moves++;
        if (moves % 10 == 0) { // randomize after 10 moves
            srand(moves);
            dir = rand()%4;
        }
        ostringstream changes;
        changes << "m " << moves << ' ' << "p " << disappear << ' ';
        changes << move_actuate(image, p1, p2);
        return changes.str();
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

class Game {
    unsigned char image[sheight][swidth][BYTES_PER_PIXEL];
    Particle p1, p2; // P, p
    goal g;
    obstacle* dogs; // D
    obstacle* guards; // G
    estation* energy_stations;
    
    public:
    Game() {
        p1 = *(new Particle(7*width/8-2*EDGE_WIDTH, EDGE_WIDTH, 100));
        p2 = *(new Particle(7*width/8, EDGE_WIDTH, 100));
        g = *(new goal(width/2, 3*height/5 + EDGE_WIDTH));
        dogs = new obstacle[NDOGS];
        guards = new obstacle[NGUARDS];
        energy_stations = new estation[NESTATIONS];
        colorRect(image, 0, 0, width, height, GREEN_BG);
        makePaths(image);
    }

    void Game_init() {
        set_dogs(dogs);
        set_guards(guards);
        set_stations(energy_stations);
    }

    string handleEvent(SDL_Event e, SDL_Renderer*& renderer) {return "p " + p2.handleEvent(e, image, renderer);}

    void draw_key(SDL_Renderer*& renderer) {
        if (p2.now_has_key()) return;
        int x = width/4, y = EDGE_WIDTH/3 + 4*height/15;
        colorRect(image, x, y, 1, 1, SILVER);
        SDL_Rect spot = {x*STRIDE, y*STRIDE, PARTICLE_WIDTH, PARTICLE_WIDTH};
        SDL_SetRenderDrawColor(renderer, SILVER[R], SILVER[G], SILVER[B], 0xFF);
        SDL_RenderFillRect(renderer, &spot);
    }

    void draw_particles(SDL_Renderer*& renderer, TTF_Font* font) {
        p1.draw_particle(renderer, BLUE);
        p2.draw_particle(renderer, BLACK);
        show_energy(renderer, font, p1, p2);
    }

    string show_move_obstacles_stations_goal(SDL_Renderer*& renderer) {
        ostringstream changes;
        for(int i=0; i<NDOGS; i++) dogs[i].add_show_obstacle(image, renderer);
        for(int i=0; i<NGUARDS; i++) guards[i].add_show_obstacle(image, renderer);
        for(int i=0; i<NESTATIONS; i++) energy_stations[i].add_show_estation(image, renderer);
        g.add_show_goal(image, renderer);
        return changes.str();
    }

    void check_report_result(SDL_Renderer*& renderer, bool& quit, TTF_Font* font) {
        if (p1.lost() || p2.won()) {
            win_message(renderer, font);
            quit = true;
            SDL_RenderPresent(renderer);
            SDL_Delay(2000);
        }
        if (p1.won() || p2.lost()) {
            lost_message(renderer, font);
            quit = true;
            SDL_RenderPresent(renderer);
            SDL_Delay(2000);
        }
    }

    void decode(char* data) {
        if (strlen(data) <= 4) return;
        int i = 0,  c = 0;
        while (data[i] != '\0') { // something remains to be seen in the packet
        printf("packet decode while loop \n");
            switch (data[i]) {
                case 'p': // update player 2
                    {i += 4; c = 0;
                    while (isdigit(data[i])) {
                        c = 10*c + (data[i]-'0');
                        i++;
                    }
                    p2.set_energy(c);
                    i += 2;
                    break;}
                case 'P':
                    {i += 2; c = 0;
                    while (data[i] != '\n') {
                        switch (data[i]) {
                            case 'x'
                        }
                        if (data[i] == 'x') {
                            i+= 2; c=0;
                            while (isdigit(data[i])) {
                                c = 10*c + (data[i]-'0');
                                i++;
                            }
                            p1.set_x(c);
                            i++;
                        }
                        if (data[i] == 'y') {
                            i+= 2; c = 0;
                            while (isdigit(data[i])) {
                                c = 10*c + (packet->data[i]-'0');
                                i++;
                            }
                            p1.set_y(c);
                            i++;
                        }
                        if (data[i] == 'e') {
                            i+= 2; c = 0;
                            while (isdigit(data[i])) {
                                c = 10*c + (data[i]-'0');
                                i++;
                            }
                            p1.set_energy(c);
                            i++;
                        }
                        if (data[i] == 'e') {
                            i+= 2;
                            c = 0;
                            while (isdigit(data[i])) {
                                c = 10*c + (data[i]-'0');
                                i++;
                            }
                            p1.set_energy(c);
                            i++;
                        }
                        if (packet->data[i] == 'h') {
                            i+= 2;
                            p1.set_has_key();
                        }
                    }
                    break;}
            }        
    }
};

int main() {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    TTF_Font* font = NULL;
    ENetAddress address;
    ENetHost* client;
    ENetEvent event;
    ENetPeer* peer = NULL;

    if (!sdl_initialize(window, renderer)) {close(window, renderer); return 1;}
    int mWindowID = SDL_GetWindowID( window );
    font = TTF_OpenFont("OpenSans-Bold.ttf", 1000);
    if (font == NULL) {printf("TTF_OpenFont: %s\n", TTF_GetError()); close(window, renderer); return 1;}

    if (enet_initialize() != 0) {
        printf("Error in initializing enet\n");
        close(window, renderer, font);
        return 1;
    }
    if (!enet_initialization(address, client, event, peer)) {close(window, renderer, font); enet_deinitialize(); return 1;}

    Game* game = new Game();
    game->Game_init(); // static changes
    bool window_focus = true;
    bool quit = false;
    SDL_Event e;
    bool show = true;
    string changes = ""; 
    SDL_SetRenderDrawColor( renderer, GREEN_BG[R], GREEN_BG[G], GREEN_BG[B], 0xFF );
    SDL_RenderClear(renderer);
    if (!load_game_screen(window, renderer)) {quit = true;} // static changes
    SDL_RenderPresent(renderer);
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
            else if (e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID) {
                if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) window_focus = true;
                else if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) window_focus = false;
            }
            changes += game->handleEvent(e, renderer); // dynamic done!
        }
        SDL_SetRenderDrawColor( renderer, GREEN_BG[R], GREEN_BG[G], GREEN_BG[B], 0xFF );
        SDL_RenderClear(renderer);
        if (!load_game_screen(window, renderer)) {quit = true;} // static changes
        // SDL_RenderPresent(renderer);
        while (enet_host_service(client, &event, 0) > 0) {
            switch(event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("New client connected from %x:%u.\n",  event.peer->address.host, event.peer->address.port);
                    peer = event.peer;
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    printf("received a packet from server data: %s \n", event.packet->data);
                    game->decode(event.packet->data);
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("disconnected.\n"); //, event.peer->data);
                    event.peer->data = NULL;
                    close(window, renderer, font, client, peer);
                    return 0;
                    break;
                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        } 
        game->draw_key(renderer); // static changes
        game->draw_particles(renderer, font); // static changes
        game->show_move_obstacles_stations_goal(renderer); // dynamic done!
        SDL_RenderPresent(renderer);
        game->check_report_result(renderer, quit, font); // dynamic
        printf("here client\n");

        const char* pack = changes.c_str();
        ENetPacket* packet = enet_packet_create(pack, strlen(pack)+1, ENET_PACKET_FLAG_RELIABLE );
        enet_peer_send(peer, 0, packet);
        enet_host_flush(client);
    }
    close(window, renderer, font, client, peer);
    return 0;
}