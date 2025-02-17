#include "Player.h"
#include "Bullet.h"
#include "Utility.h"
#include "Obstacle.h"
#include "Item.h"
#include "Timer.h"

std::vector<Player*> players;

Player::Player(std::string id): 
    Obj(), 
    _playerID(id),
    _dirX(0), _dirY(-1),
    _rotVel(0), _deg(0), 
    _moveVel(0), 
    _moveSlowDown(0),
    _playerType(GunPlayer),
    _state(alive),
    _bombEquipped(false),
    _hp(500),
    _defend(1),
    helmet(NULL),
    helmetUp(NULL),
    gun(NULL),
    bomb(NULL),
    bodyArmor(NULL),
    _shootTime(0),
    _ammo(-1),
    _isHitted(0),
    _hittedMask(NULL)
{
    loadTexture(typeToString(_playerType));
    _hittedMask = new Item;
    _hittedMask->loadTexture("ShotMask");
    _hittedMask->SetPosition(_playerX, _playerY);
    BloodStrip[0].loadTexture("BloodStripBackground");
    GunBulletStrip[0].loadTexture("GunBulletStripBackground");
    // BloodStrip[1].loadTexture("BloodStripRed");
}
Player::~Player() {
    free();   
    for (int i = 0;i < 2; i++) BloodStrip[i].free();
    _hittedMask->free();
    _hittedMask = NULL;
    helmet->free(), gun->free(), bomb->free();
    helmet = NULL, helmetUp = NULL, gun = NULL, bomb = NULL, bodyArmor = NULL;
}

// set player initial postion
void Player::setInitialPosition(int x, int y) {
    _posX = x;
    _posY = y;
    _playerSize = getWidth() / 2;
    _playerX = _posX + _playerSize;
    _playerY = _posY + getHeight() - _playerSize;
    _rotCenter.x = _playerSize; _rotCenter.y = getHeight() - _playerSize;
}

// player handle key input
void Player::handleKeyInput(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        //Adjust the velocity
        switch (e.key.keysym.sym) {
            case SDLK_UP:{
                _moveVel = 4 - _moveSlowDown;
                loadedSound.playSound(1, "footstep", -1); 
                break;
            }
            case SDLK_DOWN:{
                _moveVel = -4 + _moveSlowDown;
                loadedSound.playSound(1, "footstep", -1); 
                break;
            }
            case SDLK_LEFT: _rotVel = -5; break;
            case SDLK_RIGHT: _rotVel = +5; break;
            case SDLK_RETURN: fire(); _shootTime += 1; break;
        }
    }
    // If a key was released
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        //Adjust the velocity
        switch (e.key.keysym.sym) {
            case SDLK_UP: _moveVel = 0;Mix_Pause(1); break;
            case SDLK_DOWN: _moveVel = 0;Mix_Pause(1); break;
            case SDLK_LEFT: _rotVel = 0; break;
            case SDLK_RIGHT: _rotVel = 0; break;
            case SDLK_RETURN: _shootTime = 0; break;
        }
    }
}

// player move
void Player::move() {
    _posX += _dirX * _moveVel;
    _playerX += _dirX * _moveVel;
    // check collide with wall left and right

    if (collideWall() || collideOtherPlayer() || collideObstacle()) {
        _playerX -= _dirX * _moveVel;
        _posX -= _dirX * _moveVel;
    }

    _posY += _dirY * _moveVel;
    _playerY += _dirY * _moveVel;

    // check collide with wall up and down
    if (collideWall() || collideOtherPlayer() || collideObstacle()) {
        _posY -= _dirY * _moveVel;
        _playerY -= _dirY * _moveVel;
    }
}

// set player rotate
void Player::rotate() {
    _deg += _rotVel;
    _dirX = sin(_deg * PI / 180);
    _dirY = -cos(_deg * PI / 180);
}

// check player collide with wall
bool Player::collideWall() {
    return _playerX - _playerSize < 0 || _playerX + _playerSize > LEVEL_WIDTH || _playerY - _playerSize < 0 || _playerY + _playerSize > LEVEL_HEIGHT;
}

void Player::pickItem() {
    for (int i = 0; i < items.size(); i++) {
        if (sqrt(pow(_playerX - items[i]->getItemX(), 2) + pow(_playerY - items[i]->getItemY(), 2)) < _playerSize + items[i]->getWidth() / 2) {
            items[i]->isPicked(this);
            // todo remove item from items
        }
    }
}

// check player collide with other player
bool Player::collideOtherPlayer() {
    for (int i = 0;i < players.size(); i++) {
        if (_playerID != players[i]->_playerID && sqrt(pow(_playerX - players[i]->_playerX, 2) + pow(_playerY - players[i]->_playerY, 2)) < _playerSize + players[i]->_playerSize) return 1;
    }
    return 0;
}

bool Player::collideObstacle() {
    for (int i = 0; i < obstacles.size(); i++) {
        if (obstacles[i]->_type < Box) {
            if (obstacles[i]->_type == Tree) {
                if (getDis(_playerX, _playerY, obstacles[i]->_obstacleX, obstacles[i]->_obstacleY) < _playerSize + obstacles[i]->getWidth() / 6) return 1;
            }
            else {
                if (getDis(_playerX, _playerY, obstacles[i]->_obstacleX, obstacles[i]->_obstacleY) < _playerSize + obstacles[i]->getWidth() / 2) return 1;
            }
        }
        else {
            // if(abs(_playerX - obstacles[i]->_obstacleX) < _playerSize + _objWidth / 2 && abs(_playerY - obstacles[i]->_obstacleY) < _playerSize + _objHeight / 2) return 1;
            if (abs(_playerX - obstacles[i]->_obstacleX) < _playerSize + obstacles[i]->getWidth() / 2 && abs(_playerY - obstacles[i]->_obstacleY) < _playerSize + obstacles[i]->getHeight() / 2) return 1;
        }
    }
    return 0;
}


void Player::fire() {
    if(!_bombEquipped) {
        Bullet* bullet = new Bullet(this, (ItemType)_playerType);
        _ammo--;
        if(_ammo == 0) { 
            _playerType = GunPlayer; 
            gun = NULL;
            changeSkin(GunPlayer);
        }
        else {
            switch(_playerType) {
                case GunPlayer: {
                    loadedSound.playSound(0, "DefaultGunShot", 0);
                    break;
                }
                case MachineGunPlayer: {
                    loadedSound.playSound(0, "MachineGunShot", 0);
                    break;
                }
                case ShotGunPlayer: {
                    Player* tmp = new Player(*this);
                    tmp->_deg += 5;
                    tmp->_dirX = sin(tmp->_deg * PI / 180);
                    tmp->_dirY = -cos(tmp->_deg * PI / 180);
                    Bullet* bullet2 = new Bullet(tmp, (ItemType)_playerType);
                    tmp->_deg -= 10;
                    tmp->_dirX = sin(tmp->_deg * PI / 180);
                    tmp->_dirY = -cos(tmp->_deg * PI / 180);
                    Bullet* bullet3 = new Bullet(tmp, (ItemType)_playerType);
                    loadedSound.playSound(0, "ShotGunShot", 0);
                    bullets.push_back(bullet2);
                    bullets.push_back(bullet3);
                    break;
                }
                case FireGunPlayer: {
                    loadedSound.playSound(0, "FireGunShot", 0);
                    break;
                }
                case AK47Player: {
                    loadedSound.playSound(0, "MachineGunShot", 0);
                    break;
                }
                default: break;
            }
        }
        // bullets.emplace_back(this, (GunType)_playerType);
        bullets.push_back(bullet);
    }
    else {
        loadedSound.playSound(0, "FireInTheHole", 0);
        Bullet* bullet = new Bullet(this, Bomb);
        bullets.push_back(bullet);
    }
}

void Player::isShooted(Bullet* bullet) {
    loadedSound.playSound(3, "Hurt", 0);
    _hp -= bullet->lethality * _defend;
    _isHitted = true;
    // todo hurt animation
    if (_hp <= 0) {
        // todo death
        _state = dead;
        loadedSound.playSound(3, "shriek", 0);
    }
}

void Player::changeSkin(PlayerType newPlayerType) {
    loadTexture(typeToString(newPlayerType));
    _playerType = newPlayerType;
    _playerSize = getWidth() / 2;
    _rotCenter.x = _playerSize; _rotCenter.y = getHeight() - _playerSize;
    // set equip window
    if (gun == NULL) gun = new Item;
    gun->loadTexture(typeToString((ItemType)newPlayerType));
    gun->resize(60, 60);
}

void Player::changeHelmet(ItemType newHelmet) {
    if (helmetUp == NULL) helmetUp = new Item;
    helmetUp->loadTexture(typeToString(newHelmet) + "Up"); 
    helmetUp->resize(50, 50); 

    if (helmet == NULL) helmet = new Item;
    helmet->loadTexture(typeToString(newHelmet));
    helmet->resize(60, 60);
}

void Player::changeBodyArmor(ItemType newBodyArmor) {
    if (bodyArmor == NULL) bodyArmor = new Item;
    bodyArmor->loadTexture(typeToString(newBodyArmor));
    bodyArmor->resize(60, 60);
}

// run rotate, move render 
// todo collectItem
// todo set State
void Player::update() {
    if (_state == dead) {
        // todo destroy self
        BloodStrip[1].resize(BloodStrip[0].getWidth() * (_hp / 500), BloodStrip[0].getHeight());
        free();
        return;
    }
    // update blood
    if(_hp > 100){
        BloodStrip[1].loadTexture("BloodStripWhite");
        BloodStrip[1].resize(BloodStrip[0].getWidth() * (_hp / 500), BloodStrip[0].getHeight());
    }
    else {
        BloodStrip[1].loadTexture("BloodStripRed");
        BloodStrip[1].resize(BloodStrip[0].getWidth() * (_hp / 500), BloodStrip[0].getHeight());
    }

    // shoot continuously
    if (_shootTime && (_playerType == MachineGunPlayer || _playerType == AK47Player)) {
        if ((_shootTime++) % 10 == 0) fire();
    }
    // update ammo remaining
    GunBulletStrip[1].loadTexture("GunBulletStrip");
    if(_ammo <= -1) GunBulletStrip[1].resize(GunBulletStrip[0].getWidth(), GunBulletStrip[0].getHeight());
    else GunBulletStrip[1].resize(GunBulletStrip[0].getWidth() * (_ammo / 30), GunBulletStrip[0].getHeight());

    switch(_playerType) {
        case GunPlayer: _moveSlowDown = 0; break;
        case MachineGunPlayer: _moveSlowDown = 1; break;
        case ShotGunPlayer: _moveSlowDown = 1.5; break;
        case FireGunPlayer: _moveSlowDown = 1.5; break;
        case AK47Player: _moveSlowDown = 1; break;
        default: _moveSlowDown = 0; break;
    }
    move();
    rotate();
    pickItem();
}

void Player::setammo(int a) {
    _ammo = a;
}

void Player::renderL(SDL_Rect& camera) {
    if (_state == dead) return;
    if(_posX - camera.x < SCREEN_WIDTH / 2 && _posY - camera.y < SCREEN_HEIGHT) {
        render(_posX - camera.x, _posY - camera.y, _deg, &_rotCenter);
        if (helmetUp != NULL) {
            helmetUp->SetPosition(_playerX, _playerY);
            helmetUp->SetAngle(_deg);
            helmetUp->renderL(camera);
        }
        if (_isHitted) {
            // _isHitted = 0;
            _hittedMask->SetPosition(_playerX, _playerY);
            _hittedMask->renderL(camera);
        }
    }
}

void Player::renderR(SDL_Rect& camera) {
    if (_state == dead) return;
    if(_posX - camera.x > 0) {
        render(_posX - camera.x + SCREEN_WIDTH / 2, _posY - camera.y, _deg, &_rotCenter);
        if (helmetUp != NULL) {
            helmetUp->SetPosition(_playerX, _playerY);
            helmetUp->SetAngle(_deg);
            helmetUp->renderR(camera);
        }
    }
    if (_isHitted) {
        _isHitted = 0;
        _hittedMask->SetPosition(_playerX, _playerY);
        _hittedMask->renderR(camera);
    }
}

double Player::getPlayerX() { return _playerX; }
double Player::getPlayerY() { return _playerY; }
double Player::getDeg() { return _deg; }

PlayerState Player::getState() {
    return _state;
}

Player2::Player2(std::string id): Player(id) {}

Player2::~Player2() {
    free();   
}

void Player2::handleKeyInput(SDL_Event& e) {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        //Adjust the velocity
        switch (e.key.keysym.sym) {
            case SDLK_r:{
                _moveVel = 4 - _moveSlowDown;
                loadedSound.playSound(2, "footstep", -1); 
                break;
            }
            case SDLK_f:{
                _moveVel = -4 + _moveSlowDown;
                loadedSound.playSound(2, "footstep", -1); 
                break;
            }
            case SDLK_d: _rotVel = -5; break;
            case SDLK_g: _rotVel = +5; break;
            case SDLK_z: fire(); _shootTime = 1 ; break;
        }
    }
    // If a key was released
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        //Adjust the velocity
        switch (e.key.keysym.sym) {
            case SDLK_r: _moveVel = 0; Mix_Pause(2); break;
            case SDLK_f: _moveVel = 0; Mix_Pause(2); break;
            case SDLK_d: _rotVel = 0; break;
            case SDLK_g: _rotVel = 0; break;
            case SDLK_z: _shootTime = 0 ; break;
        }
    }
}


