#include "player.h"
#include "gamedata.h"


Player::Player( const std::string& name) :
  player(name),
  maxVelocity(Gamedata::getInstance().getXmlInt(name + "/maxSpeed")),
  amtToIncreaseVelocity(Gamedata::getInstance().getXmlInt(name+ "/incSpeed")),
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
  // Add to the current speed of the player.
  player.setVelocityX( player.getVelocityX() + amtToIncreaseVelocity * x_fov );
  player.setVelocityY( player.getVelocityY() + amtToIncreaseVelocity * y_fov );
  
  // Create a max speed
  if( player.getVelocityX() > maxVelocity * x_fov )
    player.setVelocityX( maxVelocity * x_fov );
  if( player.getVelocityY() > maxVelocity * y_fov )
    player.setVelocityY( maxVelocity * y_fov );
} 
void Player::left()  { 
  // Add to the current speed of the player.
  player.setVelocityX( player.getVelocityX() - amtToIncreaseVelocity * x_fov );
  player.setVelocityY( player.getVelocityY() - amtToIncreaseVelocity * y_fov );
  
  // Create a max speed
  if( player.getVelocityX() < -maxVelocity * x_fov )
    player.setVelocityX( -maxVelocity * x_fov );
  if( player.getVelocityY() < -maxVelocity * y_fov )
    player.setVelocityY( -maxVelocity * y_fov );
} 
void Player::up()    { 
  // Add to the current speed of the player.
  player.setVelocityX( player.getVelocityX() - amtToIncreaseVelocity * -y_fov );
  player.setVelocityY( player.getVelocityY() - amtToIncreaseVelocity * x_fov );
  
  // Create a max speed
  if( player.getVelocityX() > -maxVelocity * -y_fov )
    player.setVelocityX( -maxVelocity * -y_fov );
  if( player.getVelocityY() < -maxVelocity * x_fov )
    player.setVelocityY( -maxVelocity * x_fov );
} 
void Player::down()  { 
  // Add to the current speed of the player.
  player.setVelocityX( player.getVelocityX() + amtToIncreaseVelocity * -y_fov );
  player.setVelocityY( player.getVelocityY() + amtToIncreaseVelocity * x_fov );
  
  // Create a max speed
  if( player.getVelocityX() < maxVelocity * -y_fov )
    player.setVelocityX( maxVelocity * -y_fov );
  if( player.getVelocityY() > maxVelocity * x_fov )
    player.setVelocityY( maxVelocity * x_fov );
}

// When determining the vector, make sure to normalize it.
void Player::rotateLeft() {
    // Wrap the theta around when reaching -1.
    theta -= Gamedata::getInstance().getXmlInt(player.getName() + 
             "/thetaIncrement");
    if(theta < 0) {
        theta += 360;
    }
    double not_normalized_x= cos(theta * (3.14/180));
    double not_normalized_y = sin(theta * (3.14/180));
    
    // Check to see if the values are negative, need to preserve the negative if so.
    if(not_normalized_x >= 0)
	x_fov = rotation_radius * pow(not_normalized_x, 2);
    else
	x_fov = -1 * rotation_radius * pow(not_normalized_x, 2);
    if(not_normalized_y >= 0)
	y_fov = rotation_radius * pow(not_normalized_y, 2);
    else
	y_fov = -1 * rotation_radius * pow(not_normalized_y, 2);
}
void Player::rotateRight() {
    // Wrap the theta around when reaching 360.
    theta += Gamedata::getInstance().getXmlInt(player.getName() + 
             "/thetaIncrement");
    if(theta > 359) {
        theta -= 360;
    }
    double not_normalized_x= cos(theta * (3.14/180));
    double not_normalized_y = sin(theta * (3.14/180));
    

    // Check to see if the values are negative, need to preserve the negative if so.
    if(not_normalized_x >= 0)
	x_fov = rotation_radius * pow(not_normalized_x, 2);
    else
	x_fov = -1 * rotation_radius * pow(not_normalized_x, 2);

    if(not_normalized_y >= 0)
	y_fov = rotation_radius * pow(not_normalized_y, 2);
    else
	y_fov = -1 * rotation_radius * pow(not_normalized_y, 2);

}

void Player::update(Uint32 ticks) {
  player.update(ticks);
  stop();
}

void Player::collisionDetected(){
    // Bounce player back towards opposite direction.
    player.setVelocityX( player.getVelocityX() * -.9 );   
    player.setVelocityY( player.getVelocityY() * -.9 );
}
