#ifndef BULLET_H
#define BULLET_H
#include "Utility.h"
#include "Obj.h"
#include "Timer.h"

class Player; 

enum BulletState {
    flying, shooted, stay
};

class Bullet: public Obj {
    private:
        std::string _fromID;
        std::string _type;
        double _bulletX, _bulletY; // center position
        double _dirX, _dirY;
        double _moveVel;
        double _deg;
        double _bulletSize;
        double _distance; // longest flying distance
        BulletState _state; // whether it shooted something
        void _move();
        void _collideWall();
        void _collideOtherPlayer();
        void _collideObstacle();
        void _endDistance();
        int _explosionDelay;
    public:
        int lethality; // hurting rate
        Bullet() {};
        Bullet(Player*, ItemType);
        ~Bullet(); 
        void update();
        // void update2(SDL_Rect& camera);
        void renderL(SDL_Rect&);
        void renderR(SDL_Rect&);
};

extern std::vector<Bullet*> bullets;

#endif