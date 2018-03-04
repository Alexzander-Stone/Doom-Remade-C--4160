#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include "player.h"
#include "gamedata.h"
#include "ioMod.h"
#include "gamedata.h"

Player& Player::getInstance() {
  static Player player; 
  return player;
}

Player::Player() :
    name("Player"),
    x_location(0),
    y_location(0)
{
}

Player::Player(const string& c) :
    name(c),
    x_location(Gamedata::getInstance().getXmlInt(name + "/xLocation")),
    y_location(Gamedata::getInstance().getXmlInt(name + "/yLocation"))
{
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
