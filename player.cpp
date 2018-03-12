#include "player.h"
#include "gamedata.h"


Player::Player( const std::string& name) :
  player(name),
  initialVelocity(player.getVelocity()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  x_fov(Gamedata::getInstance().getXmlInt(name + "/xFovStart")),
  y_fov(Gamedata::getInstance().getXmlInt(name + "/yFovStart")),
  theta(Gamedata::getInstance().getXmlInt(name + "/directionStart")),
  rotation_radius(Gamedata::getInstance().getXmlInt(name + "/rotationRadius"))
{ }

void Player::stop() {
  // Momentum, slow down speed each tick.
  player.setVelocity( 
    Vector2f(
        Gamedata::getInstance().getXmlFloat(player.getName() + 
        "/momentumSlowdown") * player.getVelocityX(), 
        Gamedata::getInstance().getXmlFloat(player.getName() + 
        "/momentumSlowdown") * player.getVelocityY()) 
    );
}

// Use y_fov and x_fov to determine how diaggonal movement works.
// Vertical goes from x_fov = 1 (top) to x_fov = -1 (bottom).
// Horizontal goes form y_fov = 1 (left) to y_fov = -1 (right).
void Player::right() { 
  player.setVelocityX(initialVelocity[0] * x_fov);
  player.setVelocityY(initialVelocity[1] * y_fov);
} 
void Player::left()  { 
  player.setVelocityX(-initialVelocity[0] * x_fov);
  player.setVelocityY(-initialVelocity[1] * y_fov);
} 
void Player::up()    { 
  player.setVelocityY( -initialVelocity[1] * x_fov);
  player.setVelocityX( -initialVelocity[0] * -y_fov);
} 
void Player::down()  { 
  player.setVelocityY( initialVelocity[1] * x_fov);
  player.setVelocityX( initialVelocity[0] * -y_fov);
}
void Player::rotateLeft() {
    // Wrap the theta around when reaching -1.
    theta -= Gamedata::getInstance().getXmlInt(player.getName() + 
             "/thetaIncrement");
    if(theta < 0) {
        theta += 360;
    }
    x_fov = rotation_radius*cos(theta * (3.14159265/180));
    y_fov = rotation_radius*sin(theta * (3.14159265/180));
    
    // DEBUG.
    std::cout << "X_fov is " << x_fov << " and Y_fov is " << y_fov << std::endl;
}
void Player::rotateRight() {
    // Wrap the theta around when reaching 360.
    theta += Gamedata::getInstance().getXmlInt(player.getName() + 
             "/thetaIncrement");
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

void Player::collisionDetected(){
    stop();
}
