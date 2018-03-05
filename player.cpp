#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include "player.h"
#include "gamedata.h"
#include "ioMod.h"
#include "gamedata.h"
#include "renderContext.h"

Player& Player::getInstance() {
  static Player player;
  return player;
}

Player::Player() :
    name("Player"),
    x_location(0),
    y_location(0),
    renderer(RenderContext::getInstance()->getRenderer())
{
}

Player::Player(const string& c) :
    name(c),
    x_location(Gamedata::getInstance().getXmlInt(name + "/xLocation")),
    y_location(Gamedata::getInstance().getXmlInt(name + "/yLocation")),
    renderer(RenderContext::getInstance()->getRenderer())
{
}

Player::Player(const Player& p) :
    name(p.name),
    x_location(p.x_location),
    y_location(p.y_location),
    renderer(p.renderer)
{
}

Player& Player::operator=(const Player& rhs)
{
    name = rhs.name;
    x_location = rhs.x_location;
    y_location = rhs.y_location;
    renderer = rhs.renderer;
    return *this;
}

void Player::horizontalLeftMove()
{
    x_location -= Gamedata::getInstance().getXmlInt(name + "/movementSpeed");
}
void Player::horizontalRightMove()
{
    x_location += Gamedata::getInstance().getXmlInt(name + "/movementSpeed");
}
void Player::verticalUpMove()
{
    y_location -= Gamedata::getInstance().getXmlInt(name + "/movementSpeed");
}
void Player::verticalDownMove()
{
    y_location += Gamedata::getInstance().getXmlInt(name + "/movementSpeed");
}

void Player::draw()
{
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderDrawLine(renderer, 
                     x_location, y_location, 
                     x_location + 20, y_location + 10);
  SDL_RenderDrawLine(renderer,
                     x_location + 20, y_location + 10,
                     x_location, y_location + 20);
}
