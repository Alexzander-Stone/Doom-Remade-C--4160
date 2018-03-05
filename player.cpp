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
    x_location(Gamedata::getInstance().getXmlInt("Player/xLocation")),
    y_location(Gamedata::getInstance().getXmlInt("Player/yLocation")),
    x_fov(0),
    y_fov(0),
    theta(0),
    rotation_rad(Gamedata::getInstance().getXmlInt("Player/radius")),
    renderer(RenderContext::getInstance()->getRenderer())
{
}

Player::Player(const string& c) :
    name(c),
    x_location(Gamedata::getInstance().getXmlInt(name + "/xLocation")),
    y_location(Gamedata::getInstance().getXmlInt(name + "/yLocation")),
    x_fov(Gamedata::getInstance().getXmlFloat(name + "/xRotation")),
    y_fov(Gamedata::getInstance().getXmlFloat(name + "/yRotation")),
    theta(Gamedata::getInstance().getXmlFloat(name + "/theta")),
    rotation_rad(Gamedata::getInstance().getXmlInt(name + "/radius")),
    renderer(RenderContext::getInstance()->getRenderer())
{
}

Player::Player(const Player& p) :
    name(p.name),
    x_location(p.x_location),
    y_location(p.y_location),
    x_fov(p.x_fov),
    y_fov(p.y_fov),
    theta(p.theta),
    rotation_rad(p.rotation_rad),
    renderer(p.renderer)
{
}

Player& Player::operator=(const Player& rhs)
{
    name = rhs.name;
    x_location = rhs.x_location;
    y_location = rhs.y_location;
    x_fov = rhs.x_fov;
    y_fov = rhs.y_fov;
    theta = rhs.theta;
    rotation_rad = rhs.rotation_rad;

    renderer = rhs.renderer;
    return *this;
}

// Translations
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
    x_location += cos( theta * (3.14159265/180) );
    y_location += sin( theta * (3.14159265/180) );
}

// Rotations
void Player::rotateLeft()
{
    // Wrap the theta around when reaching 360.
    theta++;
    if(theta >= 360)
        theta -= 360;
    x_fov = rotation_rad*cos( theta * (3.14159265/180) );
    y_fov = rotation_rad*sin( theta * (3.14159265/180) );
}
void Player::rotateRight()
{
    // Wrap the theta around when reaching -1.
    theta--;
    if(theta < 0)
        theta += 360;
    x_fov = rotation_rad*cos( theta * (3.14159265/180) );
    y_fov = rotation_rad*sin( theta * (3.14159265/180) );
}

void Player::draw()
{
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderDrawLine(renderer, 
                     x_location, y_location, 
                     x_location + x_fov, y_location + y_fov);
}
