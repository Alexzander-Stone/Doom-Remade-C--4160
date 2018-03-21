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
  rotation_radius(Gamedata::getInstance().getXmlInt(name + "/rotationRadius")),
  current_state(NORMAL),
  bounce_timer(0),
	previous_x(0),
	previous_y(0)
{ }

void Player::stop() {
  // Momentum, slow down speed each tick.
  player.setVelocity( 
    Vector2f(
        Gamedata::getInstance().getXmlFloat(player.getName() + 
        "/momentumSlowdown") * player.getVelocityX(), 
        Gamedata::getInstance().getXmlFloat(player.getName() + 
        "/momentumSlowdown") * player.getVelocityY()
		) 
  );
}

// Use y_fov and x_fov to determine how diaggonal movement works.
// Vertical goes from x_fov = 1 (top) to x_fov = -1 (bottom).
// Horizontal goes form y_fov = 1 (left) to y_fov = -1 (right).
void Player::right() { 
  // Add to the current speed of the player.
  player.setVelocityX( player.getVelocityX() + amtToIncreaseVelocity * x_fov );
  player.setVelocityY( player.getVelocityY() + amtToIncreaseVelocity * y_fov );
} 
void Player::left()  { 
  // Add to the current speed of the player.
  player.setVelocityX( player.getVelocityX() - amtToIncreaseVelocity * x_fov );
  player.setVelocityY( player.getVelocityY() - amtToIncreaseVelocity * y_fov );
} 
void Player::up()    { 
  // Add to the current speed of the player.
  player.setVelocityX( player.getVelocityX() - amtToIncreaseVelocity * -y_fov );
  player.setVelocityY( player.getVelocityY() - amtToIncreaseVelocity * x_fov );
} 
void Player::down()  { 
  // Add to the current speed of the player.
  player.setVelocityX( player.getVelocityX() + amtToIncreaseVelocity * -y_fov );
  player.setVelocityY( player.getVelocityY() + amtToIncreaseVelocity * x_fov );
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

void Player::update(Uint32 ticks) {
  // Bouncing timer when colliding with wall.
	if( current_state == BOUNCE && bounce_timer >= static_cast<Uint32>( 5 * Gamedata::getInstance().getXmlInt("period") ) ) 
  {
    amtToIncreaseVelocity = Gamedata::getInstance().getXmlInt(player.getName()+ "/incSpeed");
    current_state = NORMAL; 
  }
	else if ( current_state == BOUNCE ){
		bounce_timer += ticks;
	}

	previous_y = player.getY();
	previous_x = player.getX();
  player.update(ticks);
  stop();
}

void Player::collisionDetected(){
		// Check to see if the object is within the left/right and top/bottom range
		// of the object. This will determine where to place the object.
		// Detach the collision object once the player is in a valid state (outside
		// the wall).
		
		// Keep track of which wall the player encountered.
		bool xFinished = false;			
		double momentumX = getMomentumVelocityX();
		double momentumY = getMomentumVelocityY();

		std::list<SmartSprite*>::iterator it = player.getObservers().begin();
		while( it != player.getObservers().end() )
		{
			int collision_obj_x = (*it)->getX();
			int collision_obj_y = (*it)->getY();
			double currentX = player.getX();
			double currentY = player.getY();
						
			// Momentum direction determines which direction to move the player in response 
			// to the collision. 
			if( x_fov != 0 && y_fov != 0 ) {
				while ( ( currentX < collision_obj_x + (*it)->getScaledWidth() + 2 && 
	     				    currentX + getScaledWidth() > collision_obj_x - 2) 
								&& 
								( currentY < collision_obj_y + (*it)->getScaledHeight() + 2 &&
									currentY + getScaledHeight() > collision_obj_y - 2 )
							) {
					currentX = currentX + .01 * (-momentumX);
					currentY = currentY + .01 * (-momentumY);
				}
				player.setX(currentX);
				player.setY(currentY);
			}
			else if( y_fov == 0) {
				while ( currentY < collision_obj_y + (*it)->getScaledHeight() + 2 &&
								currentY + getScaledHeight() > collision_obj_y - 2 ) {
					currentY = currentY - .01*momentumY;
				}
				player.setY(currentY);
			}		
			else {
				while ( currentX < collision_obj_x + (*it)->getScaledWidth() + 2 && 
	     				  currentX + getScaledWidth() > collision_obj_x - 2 ) {
					currentX = currentX - .01*momentumX;
				}
				player.setX(currentX);
			}
			if( !( currentX < collision_obj_x + (*it)->getScaledWidth() + 2 && 
	     				    currentX + getScaledWidth() > collision_obj_x - 2) )
				xFinished = true;
			else
				xFinished = false;

			it++;			
		}
		player.getObservers().erase( player.getObservers().begin(), 
																 player.getObservers().end() 
															 );

		// Use the direction that the momentum is traveling towards.
    // Bounce player back towards opposite direction.
    // Change state of player to BOUNCE mode.
    if ( current_state == NORMAL ){
    	current_state = BOUNCE;
    	bounce_timer = 0;  
    	amtToIncreaseVelocity = Gamedata::getInstance().getXmlInt(player.getName() + 
															"/Bounce/incSpeed");
			float bounceVelX = player.getVelocityX() *  
											 Gamedata::getInstance().getXmlFloat(player.getName() + "/Bounce/changeVel");
			float bounceVelY = player.getVelocityY() * 
											 Gamedata::getInstance().getXmlFloat(player.getName() + "/Bounce/changeVel");
			
			// Player has hit a horizontal wall. Otherwise, vertical wall.
			if( xFinished == false) {
				bounceVelY *= -1;
			}
			else {
				bounceVelX *= -1;
			}
    	player.setVelocityX( bounceVelX );   
    	player.setVelocityY( bounceVelY );
    }   

		// Redraw player.
		player.draw();
}

// Determine the x value of the player's momentum.
double Player::getMomentumVelocityX() const {
	double deltaX = player.getX() - previous_x;
	double absoluteDeltaX = abs( player.getX() ) + abs( previous_x );
	double absoluteDeltaY = abs( player.getY() ) + abs( previous_y );
	double result = ( absoluteDeltaX ) / (absoluteDeltaX + absoluteDeltaY);
	if(deltaX < 0)
		result *= -1;

  return result;
}

// Determine the y value of the player's momentum.
double Player::getMomentumVelocityY() const {
	double deltaY = player.getY() - previous_y;
	double absoluteDeltaX = abs( player.getX() ) + abs( previous_x );
	double absoluteDeltaY = abs( player.getY() ) + abs( previous_y );
	double result = ( absoluteDeltaY ) / (absoluteDeltaX + absoluteDeltaY);
	if(deltaY < 0)
		result *= -1;

	return result;
}
