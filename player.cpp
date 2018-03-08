#include "player.h"
#include "gamedata.h"


Player::Player( const std::string& name) :
  player(name),
  initialVelocity(player.getVelocity()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  x_fov(1),
  y_fov(0),
  theta(0),
  rotation_radius(1)
{ }

void Player::stop() { 
  player.setVelocity( Vector2f(0, 0) );
}

void Player::right() { 
  if ( player.getX() + x_fov < worldWidth-getScaledWidth()) {
    player.setVelocityX(initialVelocity[0] * x_fov);
    player.setVelocityY(initialVelocity[1] * y_fov);
  }
} 
void Player::left()  { 
  if ( player.getX() + x_fov > 0) {
    player.setVelocityX(-initialVelocity[0] * x_fov);
    player.setVelocityY(-initialVelocity[1] * y_fov);
  }
} 
void Player::up()    { 
  if ( player.getY() + y_fov > 0) {
    player.setVelocityY( -initialVelocity[1] * x_fov);
    player.setVelocityX( -initialVelocity[0] * y_fov);
  }
} 
void Player::down()  { 
  if ( player.getY() + y_fov < worldHeight-getScaledHeight()) {
    player.setVelocityY( initialVelocity[1] * x_fov);
    player.setVelocityX( initialVelocity[0] * y_fov);
  }
}
void Player::rotateRight() {
    // Wrap the theta around when reaching -1.
    theta -= 2;
    if(theta < 0) {
        theta += 360;
    }
    x_fov = rotation_radius*cos(theta * (3.14159265/180));
    y_fov = rotation_radius*sin(theta * (3.14159265/180));
    
    // DEBUG.
    std::cout << "X_fov is " << x_fov << " and Y_fov is " << y_fov << std::endl;
}
void Player::rotateLeft() {
    // Wrap the theta around when reaching 360.
    theta += 2;
    if(theta >= 360) {
        theta -= 359;
    }
    x_fov = rotation_radius*cos(theta * (3.14159265/180));
    y_fov = rotation_radius*sin(theta * (3.14159265/180));

    // DEBUG.
    std::cout << "X_fov is " << x_fov << " and Y_fov is " << y_fov << std::endl;
}

void Player::update(Uint32 ticks) { 
  player.update(ticks);
  stop();
}
