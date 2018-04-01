#include "enemy.h"
#include "gamedata.h"


Enemy::Enemy( const std::string& name) :
  enemy(name),
  maxVelocity(Gamedata::getInstance().getXmlInt(name + "/maxSpeed")),
  amtToIncreaseVelocity(Gamedata::getInstance().getXmlInt(name+ "/incSpeed")),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  x_fov(Gamedata::getInstance().getXmlInt(name + "/xFovStart")),
  y_fov(Gamedata::getInstance().getXmlInt(name + "/yFovStart")),
  theta(Gamedata::getInstance().getXmlInt(name + "/directionStart")),
  rotation_radius(Gamedata::getInstance().getXmlInt(name + "/rotationRadius")),
  current_state(NORMAL),
  bounce_timer(0),
  previous_x(0),
  previous_y(0)
{ }

void Enemy::stop() {
  // Momentum, slow down speed each tick.
  enemy.setVelocity( 
    Vector2f(
    Gamedata::getInstance().getXmlFloat(enemy.getName() + 
        "/momentumSlowdown") * enemy.getVelocityX(), 
        Gamedata::getInstance().getXmlFloat(enemy.getName() + 
        "/momentumSlowdown") * enemy.getVelocityY()
		) 
  );
}

// Use y_fov and x_fov to determine how diaggonal movement works.
// Vertical goes from x_fov = 1 (top) to x_fov = -1 (bottom).
// Horizontal goes form y_fov = 1 (left) to y_fov = -1 (right).
void Enemy::right() { 
  // Add to the current speed of the enemy.
  enemy.setVelocityX( enemy.getVelocityX() + amtToIncreaseVelocity * x_fov );
  enemy.setVelocityY( enemy.getVelocityY() + amtToIncreaseVelocity * y_fov );
} 
void Enemy::left()  { 
  // Add to the current speed of the enemy.
  enemy.setVelocityX( enemy.getVelocityX() - amtToIncreaseVelocity * x_fov );
  enemy.setVelocityY( enemy.getVelocityY() - amtToIncreaseVelocity * y_fov );
} 
void Enemy::up()    { 
  // Add to the current speed of the enemy.
  enemy.setVelocityX( enemy.getVelocityX() - amtToIncreaseVelocity * -y_fov );
  enemy.setVelocityY( enemy.getVelocityY() - amtToIncreaseVelocity * x_fov );
} 
void Enemy::down()  { 
  // Add to the current speed of the enemy.
  enemy.setVelocityX( enemy.getVelocityX() + amtToIncreaseVelocity * -y_fov );
  enemy.setVelocityY( enemy.getVelocityY() + amtToIncreaseVelocity * x_fov );
}

// When determining the vector, make sure to normalize it.
void Enemy::rotateLeft() {
    // Wrap the theta around when reaching -1.
    theta -= Gamedata::getInstance().getXmlInt(enemy.getName() + 
             "/thetaIncrement");
    if(theta < 0) {
      theta += 360;
    }
    float not_normalized_x= cos(theta * (3.14/180));
    float not_normalized_y = sin(theta * (3.14/180));
    
    // Check to see if the values are negative, need to preserve the negative if so.
    if(not_normalized_x >= 0){
      x_fov = rotation_radius * pow(not_normalized_x, 2);
    }
    else{
      x_fov = -1 * rotation_radius * pow(not_normalized_x, 2);
    }

    if(not_normalized_y >= 0){
      y_fov = rotation_radius * pow(not_normalized_y, 2);
    }
    else{
      y_fov = -1 * rotation_radius * pow(not_normalized_y, 2);
    }
}
void Enemy::rotateRight() {
    // Wrap the theta around when reaching 360.
    theta += Gamedata::getInstance().getXmlInt(enemy.getName() + 
             "/thetaIncrement");
    if(theta > 359) {
    	theta -= 360;
    }
    float not_normalized_x= cos(theta * (3.14/180));
    float not_normalized_y = sin(theta * (3.14/180));
    

    // Check to see if the values are negative, need to preserve the negative if so.
    if(not_normalized_x >= 0){
      x_fov = rotation_radius * pow(not_normalized_x, 2);
    }
    else{
      x_fov = -1 * rotation_radius * pow(not_normalized_x, 2);
    }

    if(not_normalized_y >= 0){
      y_fov = rotation_radius * pow(not_normalized_y, 2);
    }
    else{
      y_fov = -1 * rotation_radius * pow(not_normalized_y, 2);
    }
}

void Enemy::update(Uint32 ticks) {
  // Bouncing timer when colliding with wall.
  if( current_state == BOUNCE && bounce_timer >= static_cast<Uint32>( 10 * Gamedata::getInstance().getXmlInt("period") ) 
      && 
      ( fabs(enemy.getVelocityX() ) +fabs(enemy.getVelocityY()) > 450 ) 
    ) {
    current_state = NORMAL; 
  }
  else if ( current_state == BOUNCE ){
    bounce_timer += ticks;
  }
  
  previous_y = enemy.getY();
  previous_x = enemy.getX();
  enemy.update(ticks);
  stop();
}

void Enemy::collisionDetected(){
  // Check to see if the object is within the left/right and top/bottom range
  // of the object. This will determine where to place the object.
  // Detach the collision object once the enemy is in a valid state (outside
  // the wall).
	
	
  // Keep track of which wall the enemy encountered.
  bool xFinished = false;			
  float momentumX = getMomentumVelocityX();
  float momentumY = getMomentumVelocityY();

  std::list<SmartSprite*>::iterator it = enemy.getObservers().begin();
  while( it != enemy.getObservers().end() ) {
    float collision_obj_x = (*it)->getX();
    float collision_obj_y = (*it)->getY();
    float currentX = enemy.getX();
    float currentY = enemy.getY();
    float currentIncrement = 0;
   
    // Momentum direction determines which direction to move the enemy in response 
    // to the collision. 
    if( (*it)->getName() == "Wall/Horizontal") {
      while ( currentY + currentIncrement <= collision_obj_y + (*it)->getScaledHeight() + 1.001 &&
		          currentY + currentIncrement + getScaledHeight() >= collision_obj_y - 1.001 ) {
	      currentIncrement += -momentumY;
      }
      currentY += currentIncrement;
      enemy.setY( currentY );
    }
    else if( (*it)->getName() == "Wall/Vertical" ) {
      while ( currentX + currentIncrement <= collision_obj_x + (*it)->getScaledWidth() + 1.001 && 
	            currentX + getScaledWidth() + currentIncrement >= collision_obj_x - 1.001 ) {
	      currentIncrement += -momentumX ;
      }
      currentX += currentIncrement;
      enemy.setX( currentX );
    }
    if( currentX <= collision_obj_x + (*it)->getScaledWidth() + 1.00001 && 
	      currentX + getScaledWidth() >= collision_obj_x - 1.00001 )
      xFinished = false;
    else
      xFinished = true;

    it++;			
  }
  enemy.getObservers().erase( enemy.getObservers().begin(), enemy.getObservers().end() );

  // Use the direction that the momentum is traveling towards.
  // Bounce enemy back towards opposite direction.
  // Change state of enemy to BOUNCE mode.
  if ( current_state == NORMAL ){
    current_state = BOUNCE;
    bounce_timer = 0;  
    float bounceVelX = enemy.getVelocityX() * Gamedata::getInstance().getXmlFloat(enemy.getName() + "/Bounce/changeVel");
    float bounceVelY = enemy.getVelocityY() * Gamedata::getInstance().getXmlFloat(enemy.getName() + "/Bounce/changeVel");
			
    // Enemy has hit a horizontal wall. Otherwise, vertical wall.
    if( xFinished == false) {	    
      bounceVelY *= -1;
    }
    else {
      bounceVelX *= -1;
    }
    enemy.setVelocityX( bounceVelX );   
    enemy.setVelocityY( bounceVelY );
  }   
  // Still decrement speed whenever running into wall.
  else{
    float slowVelX = enemy.getVelocityX() * Gamedata::getInstance().getXmlFloat(enemy.getName() + "/Bounce/changeVel");
    float slowVelY = enemy.getVelocityY() * Gamedata::getInstance().getXmlFloat(enemy.getName() + "/Bounce/changeVel");
			
    enemy.setVelocityX( slowVelX );   
    enemy.setVelocityY( slowVelY );
  }
}

// Determine the x value of the enemy's momentum.
float Enemy::getMomentumVelocityX() const {
  float deltaX = enemy.getX() - previous_x;
  float absoluteDeltaX = fabs( enemy.getX()  - previous_x );
  float absoluteDeltaY = fabs(  enemy.getY() - previous_y );
  float result = ( deltaX ) / (absoluteDeltaX + absoluteDeltaY);

  return result;
}

// Determine the y value of the enemy's momentum.
float Enemy::getMomentumVelocityY() const {
  float deltaY = enemy.getY()  - previous_y;
  float absoluteDeltaX = fabs( enemy.getX() - previous_x );
  float absoluteDeltaY = fabs( enemy.getY() - previous_y );
  float result = ( deltaY ) / (absoluteDeltaX + absoluteDeltaY);

  return result;
}
