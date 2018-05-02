#include "living.h"
#include <iostream>

Living::Living() : 
health(100)
{

}


void Living::dealDamage(int n)
{
  health -= n;
}
