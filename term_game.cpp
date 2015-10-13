#include <iostream>
#include <curses.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "BufferToggle.h"
#include <vector>
#include <time.h>

using namespace std;

struct Position {
    Position() : _x(0), _y(0) {};
    Position(int x, int y) : _x(x), _y(y) {};
    int _x, _y;
};

struct Projectile {
    Projectile(Position pos, char dir, char sign) : _pos(pos), _dir(dir), _sign(sign) {};
    Position _pos;
    char _dir;
    char _sign;
};

class Player
{
    public:
        Player(Position termDim);
        void moveLeft();
        void moveRight();
        void moveUp();
        void moveDown();
        void fireWord();
        void timeStep();

        int getPosX();
        int getPosY();
        vector<Projectile*> getProjectiles();

    private:
        vector<Projectile*> _projs;
        Position _pos;
        char _dir;
};

Player::Player(Position termDim) {
    _dir = 'k';
    _pos._x = termDim._x / 2;
    _pos._y = termDim._y / 2;
}

void Player::timeStep() {
    for(int i = 0; i < _projs.size(); ++i) {
        switch(_projs[i]->_dir) {
            case 'h':
                --_projs[i]->_pos._x;
                break;
            case 'l':
                ++_projs[i]->_pos._x;
                break;
            case 'j':
                --_projs[i]->_pos._y;
                break;
            case 'k':
                ++_projs[i]->_pos._y;
                break;
            default:
                cout << "Projs direction error" << endl;
        }
    }
}
void Player::moveLeft()     { _pos._x -= 1; _dir = 'h'; }
void Player::moveRight()    { _pos._x += 1; _dir = 'l'; }
void Player::moveUp()       { _pos._y += 1; _dir = 'k'; }
void Player::moveDown()     { _pos._y -= 1; _dir = 'j'; }

void Player::fireWord() {
    _projs.push_back(new Projectile(_pos, _dir, 'w'));
}

int Player::getPosX()       { return _pos._x; }
int Player::getPosY()       { return _pos._y; }

vector<Projectile*> Player::getProjectiles() { return _projs; }

class Game
{
    public:
        Game();
        void draw(Player* player);
        void update(Player* player);
        Position getTermDim();
    private:
        Position _termDim;
        clock_t _startTime, _currentTime, _diffTime;
};

Game::Game() {
    _startTime = clock();
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    _termDim._x = w.ws_col;
    _termDim._y = w.ws_row; 
}

Position Game::getTermDim() { return _termDim; }

void Game::update(Player* player) {
    _currentTime = clock();
    _diffTime = (_currentTime - _startTime);
    if(_diffTime > 50) {
        _startTime = clock();
        player->timeStep();
    }
}


void Game::draw(Player* player) {
    for(int y = 0; y < _termDim._y; ++y) {
        cout << endl;
        for(int x = 0; x < _termDim._x; ++x) {
            vector<Projectile*> projs = player->getProjectiles();
            for(int proj = 0; proj < projs.size(); ++proj) {
                if(projs[proj]->_pos._x == x && projs[proj]->_pos._y == y) {
                    cout << projs[proj]->_sign;
                }
            }
            if(player->getPosX() == x && player->getPosY() == y) {
                cout << "#";
            }
            else {
                cout << " ";
            }
        }
    }
}


class Controller
{
    public:
        void handleInput(char input, Player* player);
};

void Controller::handleInput(char input, Player* player) {
    switch(input) {
        case 'h':
            player->moveLeft();
            break;
        case 'l':
            player->moveRight();
            break;
        case 'j':
            player->moveUp();
            break;
        case 'k':
            player->moveDown();
            break;
        case 'w':
            player->fireWord();
            break;
        default:
            cout << "Invalid input" << endl;
    }
}


int main()
{
    Game* game = new Game();
    Player* player = new Player(game->getTermDim());
    Controller* controller = new Controller();


    BufferToggle bt;
    bt.off();

    cout << "HOWTO: Move around with h, j, k and l." << endl;
    while(true) {
        controller->handleInput(getchar(), player);
        game->draw(player);
        game->update(player);
    }

    return 0;
}
