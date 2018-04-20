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
  Vector2f pos = getPosition();
  Sprite::update(ticks);
  distance += ( hypot(getX()-pos[0], getY()-pos[1]) );
  if (distance > maxDistance) tooFar = true;
}

bool Bullet::checkCollision( const Drawable* sprite) const{
  return strategy->execute(*sprite, *this); 
}
