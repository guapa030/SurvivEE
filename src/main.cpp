#include "global.h"
#include "main.h"
#include "Obj.h"
#include "Player.h"
#include "Bullet.h"
#include "LoadMedia.h"
#include "button.h"
#include "Item.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
int LEVEL_WIDTH = 1600;
int LEVEL_HEIGHT = 1200;

// set Game State
GameState gameState;
// background, loadingmenu
Obj background, loadingmenu;
// camera
SDL_Rect camera = {0 , 0, SCREEN_WIDTH, SCREEN_HEIGHT};
// tree
std::string itemName[12] = {"MachineGun", "AK47", "Bomb", "Gun", "ShotGun", "SubMachineGun",
                    "Bandage", "BodyArmor1", "BodyArmor2", "Helmet1", "Helmet2", "LifeBox"};
// title
Obj title;
// button
Button* startButton;

int main(int argc, char* args[]) {
    try {
        init();
        loadMedia();
    }
    catch (const char* message) {
        printf("Error: %s\n", message);
    }
    for (int i = 0;i < 12; i++) {
        Item* tmp = new Item(itemName[i], 300.0, i * 60);
        items.push_back(tmp);
    }
    //Event handler
    SDL_Event e;
    // initail game State
    gameState = Menu;
    while (gameState != Quit) {
        switch (gameState) {
            case Menu: menu(e); break;
            case Loading: gameLoad(e); break;
            case Playing: playing(e); break;
            default: 
                gameState = Quit;
                break;
        }
    }
    // close game
    close();

}

void menu(SDL_Event& e) {
    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 ) {
        //User requests quit
        if (startButton->get_triggered() == true) { gameState = Loading; break; }
        else if (e.type == SDL_QUIT) { gameState = Quit; break; }
        startButton->handleEvent(&e); 
        // tutorial.handleEvent(&e); option.handleEvent(&e);
    }
    //Clear screen
    SDL_SetRenderDrawColor( gRenderer, 182, 196, 182, 0 );
    SDL_RenderClear( gRenderer );

    // Render background
    background.render(0, 0);

    // Render Title
    title.render((SCREEN_WIDTH - title.getWidth()) / 2 , SCREEN_HEIGHT / 5);

    // Render Button
    startButton->update(); 
    startButton->resize(152, 42);
    // tutorial.update(); option.update();

    //Update screen
    SDL_RenderPresent( gRenderer );
}

void gameLoad(SDL_Event& e) {
    while( SDL_PollEvent( &e ) != 0 ) {
        //User requests quit
        if (e.key.keysym.sym == SDLK_SPACE) { 
            for (int i = 0;i < 2; i++) {
                Player* player = new Player(std::to_string(i));
                players.push_back(player);
            }
            // resize
            background.resize(LEVEL_WIDTH, LEVEL_HEIGHT);
            // set player place
            players[0]->setInitialPosition(SCREEN_WIDTH * 3 / 4, SCREEN_HEIGHT / 4);
            players[1]->setInitialPosition(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);
            gameState = Playing;
            break;
            }
        else if (e.type == SDL_QUIT) { gameState = Quit; break; }
    }
    //Clear screen
    SDL_SetRenderDrawColor( gRenderer, 182, 196, 182, 0 );
    SDL_RenderClear( gRenderer );

    // Render loadingmenu
    loadingmenu.render(0, 0);
    loadingmenu.resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    //Update screen
    SDL_RenderPresent( gRenderer );
}

void playing(SDL_Event& e) {
    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 ) {
        //User requests quit
        if(e.type == SDL_QUIT) { gameState = Quit; break; }
        players[0]->handleKeyInput(e);
    }

    //Clear screen
    SDL_RenderClear( gRenderer );
    SDL_SetRenderDrawColor( gRenderer, 182, 196, 182, 100 );
    // Render background
    background.render(0, 0, 0.0, NULL, &camera);
    //Render update
    for (int i = 0;i < players.size(); i++) players[i]->update(camera);
    for (int i = 0;i < bullets.size(); i++) bullets[i]->update(camera);

    //Center the camera over the Player
    camera.x = players[0]->getPlayerX() - SCREEN_WIDTH / 2;
    camera.y = players[0]->getPlayerY() - SCREEN_HEIGHT / 2;

    //Keep the camera in bounds
    if( camera.x < 0 ) {camera.x = 0;}
    if( camera.y < 0 ) {camera.y = 0;}
    if( camera.x > LEVEL_WIDTH - camera.w ) {camera.x = LEVEL_WIDTH - camera.w;}
    if( camera.y > LEVEL_HEIGHT - camera.h ) {camera.y = LEVEL_HEIGHT - camera.h;}
    // Render test item
    for (int i = 0;i < items.size(); i++) {
        items[i]->update(camera);
    }
    //Update screen
    SDL_RenderPresent( gRenderer );
}

void init() {
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) throw SDL_GetError();
    //Set texture filtering to linear
    if (!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" )) printf( "Warning: Linear texture filtering not enabled!" );
    // Create Window
    gWindow = SDL_CreateWindow( "SurvivEE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if (gWindow == NULL) throw SDL_GetError();
    // Create vsynced renderer for window
    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if (gRenderer == NULL) throw SDL_GetError();
    // Initialize renderer color
    SDL_SetRenderDrawColor( gRenderer, 182, 196, 182, 100 );
    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init( imgFlags ) & imgFlags)) throw IMG_GetError();
    // Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) throw Mix_GetError();
}

void loadMedia() {
    // Load all texture
    loadedTexture.loadAllTexture();
    // Load all sound
    loadedSound.loadAllSound();

    background.loadTexture("Grass");
    title.loadTexture("title");
    loadingmenu.loadTexture("loadingmenu");
    startButton = new Button(Start);
	// players[0].loadTexture("GunPlayer");
    // players[1].loadTexture("MachineGunPlayer");
}

void close() {
    // free loadedTextures
    loadedTexture.free();
    // free loadedSound
    loadedSound.free();
	//Free loaded player
	for (int i = 0;i < players.size(); i++) players[i]->free();
	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	
	//Quit SDL subsystems
	IMG_Quit();
    Mix_Quit();
	SDL_Quit();
}

