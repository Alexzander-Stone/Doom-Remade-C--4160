#include "enemy.h"
#include "gamedata.h"

Enemy::Enemy( const std::string& name, const Vector2f& pos) :
  WallCollidable(name), // Pass parent constructor the name.
  current_state(NORMAL),
  maxVelocity(Gamedata::getInstance().getXmlInt(name + "/maxSpeed")),
  amtToIncreaseVelocity(Gamedata::getInstance().getXmlInt(name+ "/incSpeed")),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  playerPos(pos)
{ }

void Enemy::draw() const{
  WallCollidable::draw();
  getSpriteInfo()->draw();
}

// Slow down velocity each tick.
void Enemy::stop() {
  getSpriteInfo()->setVelocity( 
    Vector2f(
	Gamedata::getInstance().getXmlFloat(getName() + 
        "/momentumSlowdown") * getSpriteInfo()->getVelocityX(), 
        Gamedata::getInstance().getXmlFloat(getName() + 
        "/momentumSlowdown") * getSpriteInfo()->getVelocityY()
    ) 
  );
}

// Use y_fov and x_fov to determine how diagonal movement works.
// Vertical goes from x_fov = 1 (top) to x_fov = -1 (bottom).
// Horizontal goes form y_fov = 1 (left) to y_fov = -1 (right).
void Enemy::up()    { 
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() - amtToIncreaseVelocity * getYFov() );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() - amtToIncreaseVelocity * getXFov() );
} 



void Enemy::update(Uint32 ticks) {
  // Bouncing timer when colliding with wall.
  // Can bounce when traveling above the bounce speed.
  if( getState() == 1 && getBounceTimer() >= static_cast<Uint32>( 10 * Gamedata::getInstance().getXmlInt("period") ) 
      && 
      ( fabs(getSpriteInfo()->getVelocityX() ) +fabs(getSpriteInfo()->getVelocityY()) > Gamedata::getInstance().getXmlInt(getName() + "/bounceSpeedRequirement") ) 
    ) 
  {
    setState(0); 
  }
  else if ( getState() == 1 ){
    setBounceTimer(ticks);
  }

  // Use player's position/angle to determine which way to rotate the enemy.
  // Tie this to tick rate in future.
  float deltaX = playerPos[0] - getSpriteInfo()->getX();
  float deltaY = playerPos[1] - getSpriteInfo()->getY();
  float angleBetweenVectors = atan2(deltaY, deltaX) * 57.2958 + 180;
  
  deltaX = getPreviousX() - getSpriteInfo()->getX();
  deltaY = getPreviousY() - getSpriteInfo()->getY();
  float tempTheta = atan2(deltaY, deltaX) * 57.2958 + 180;
  
  float leftRotCheck = angleBetweenVectors - tempTheta;
  float rightRotCheck = tempTheta - angleBetweenVectors;
  if(leftRotCheck < 0)
    leftRotCheck += 360; 
  if(rightRotCheck < 0)
    rightRotCheck += 360;

  if(rightRotCheck > leftRotCheck){
    rotateRight();
  }
  else if(rightRotCheck < leftRotCheck){
    rotateLeft();
  }

  // Shoot projectile at player if they are within range.
  if(current_state == NORMAL && hypot(playerPos[0]-getSpriteInfo()->getX(), playerPos[1] - getSpriteInfo()->getY()) < Gamedata::getInstance().getXmlInt(getName()+"/attackDistance"))
  {
    current_state = ATTACK;
    amtToIncreaseVelocity = 0;
    std::cout << "My leg!" << std::endl;
  }
  // Continue shooting at player until they exit range.
  else if(current_state == ATTACK && hypot(playerPos[0]-getSpriteInfo()->getX(), playerPos[1] - getSpriteInfo()->getY()) < Gamedata::getInstance().getXmlInt(getName()+"/attackDistance")) {
  shoot();  
  }
  // Player has left the range of the enemy, begin walking towards the player
  // again.
  else if(current_state == ATTACK) {
    current_state = NORMAL;
    amtToIncreaseVelocity = Gamedata::getInstance().getXmlInt(getName() + "/incSpeed");

    std::cout << "fancey nancey" << std::endl;
  }

  up();  

  setPreviousY(getSpriteInfo()->getY());
  setPreviousX(getSpriteInfo()->getX());
  getSpriteInfo()->update(ticks);

  // Shooting.
  WallCollidable::update(ticks);

  stop();
}

void Enemy::shoot(){
  WallCollidable::shoot();
}
