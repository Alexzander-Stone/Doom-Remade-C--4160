#include "living.h"
#include "clock.h"
#include <iostream>

Living::Living() : 
  health(100),
  alive(true)
{

}

bool Living::getAlive(){
  return alive;
}

void Living::update()
{
  // Death has occurred.
  if(health <= 0)
  {
    alive = false;
  }
}

void Living::dealDamage(int n)
{
  health -= n;
}
