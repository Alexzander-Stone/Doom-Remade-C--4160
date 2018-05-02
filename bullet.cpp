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
  Sprite bulletCollision("BulletCollision");
  bulletCollision.setPosition(getPosition() + Vector2f(getScaledWidth()/2, getScaledHeight()/2));

  // Enemy collision, large sprite.
  if(sprite->getName() == "Pinkie"){
    Sprite objectToHit("BulletCollision");
    objectToHit.setScale(2);
    return strategy->execute(objectToHit, bulletCollision); 
  }
  else // Walls and player.
    return strategy->execute(*sprite, bulletCollision); 

}
