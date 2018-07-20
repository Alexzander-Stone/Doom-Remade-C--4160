#include <iostream>
#include <cmath>
#include "bullet.h"
#include "collisionStrategy.h"

Bullet& Bullet::operator=(const Bullet& bul){
  if(this == &bul) return *this;
  delete strategy;
  strategy = new RectangularCollisionStrategy;
  return *this;
}

void Bullet::update(Uint32 ticks) { 
  Sprite::update(ticks);
}

bool Bullet::checkCollision( const Drawable* sprite) const{
	Sprite bulletCollider("BulletCollision");
  bulletCollider.setPosition(getPosition() + Vector2f(getScaledWidth()/2 - bulletCollider.getScaledWidth()/2, getScaledHeight()/2 - bulletCollider.getScaledHeight()/2));

  // Enemy collision, large sprite.
	// Since enemy sprite is intentionally large, instead of using it's width
	// and height we create a new, smaller sprite for the single collision detection. 
	// Then it is moved to the center of the enemy location. 
  if(sprite->getName() == "Pinkie"){
    Sprite minimizedEnemySprite("BulletCollision");
    minimizedEnemySprite.setPosition(sprite->getPosition());
    return strategy->execute(minimizedEnemySprite, bulletCollider); 
  }
  // Walls and player. Both of these are appropriate sizes, 
	// so minimization not needed.
	else {  
    return strategy->execute(*sprite, bulletCollider); 
  }
}
