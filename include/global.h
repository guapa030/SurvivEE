#ifndef GLOBAL_H
#define GLOBAL_H
#define PI 3.14159265
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
#include <map>
#include <time.h>

enum GameState {
    Menu,
    Loading,
    Playing,
    Pause,
    GameOver,
    Quit
};

// enum GunType {
//     Default, Gun, MachineGun, ShotGun, FireGun, AK47
// };

enum PlayerType {
    DefaultPlayer, GunPlayer, MachineGunPlayer, ShotGunPlayer, FireGunPlayer, AK47Player
};

enum ButtonType {
    Start, Tutorial, Option, Continue, Restart
};

enum ItemType {
    Default, Gun, MachineGun, ShotGun, FireGun, AK47, 
    Bomb, BodyArmor1, BodyArmor2, Helmet1, Helmet2,
    Bandage, LifeBox
};

enum ObstacleType {
    Tree, Rock1, Rock2, Rock3, Box, BrickWall
};

extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int LEVEL_WIDTH;
extern int LEVEL_HEIGHT;
extern ItemType itemName[12];
#endif