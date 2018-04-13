#include <algorithm>
#include <cmath>
#include "subjectSprite.h"
#include "smartSprite.h"
#include "gamedata.h"
#include "renderContext.h"

SubjectSprite::SubjectSprite( const std::string& name) :
  MultiSprite(name), 
  colliders()
{ }

SubjectSprite::SubjectSprite(const SubjectSprite& s) :
  MultiSprite(s), 
  colliders( s.colliders )
  { }

void SubjectSprite::detach( SmartSprite* o ) {
  std::list<SmartSprite*>::iterator ptr = colliders.begin();
  while ( ptr != colliders.end() ) {
    if ( *ptr == o ) {
      ptr = colliders.erase(ptr);
      return;
    }
    ++ptr;
  }
}

void SubjectSprite::update(Uint32 ticks) { 
  MultiSprite::update(ticks);
  for(SmartSprite* ptr : colliders){
    (*ptr).setPlayerPos( getPosition() );
    ++ptr;
  }
}

void SubjectSprite::sort(const Vector2f& playerPosition){
  colliders.sort( [&playerPosition](SmartSprite* lhs, 
                                   SmartSprite* rhs) { 

/*
      std::cout << playerPosition << std::endl;
      std::cout << "lhs is " << lhs->getPosition() << " and rhs is " << rhs->getPosition() << std::endl;
      std::cout << lhs->getScaledWidth()/2 << " and other lhs is " << lhs->getScaledHeight()/2 << std::endl;
      std::cout << rhs->getScaledWidth()/2 << " and other rhs is " << rhs->getScaledHeight()/2 << std::endl;
      std::cout <<  hypot(lhs->getPosition()[0] + lhs->getScaledWidth()/2 - playerPosition[0] + 16,
                        lhs->getPosition()[1] + lhs->getScaledWidth()/2 - playerPosition[1] + 16) 
                  << " and " <<
                  hypot(rhs->getPosition()[0] + rhs->getScaledWidth()/2 - playerPosition[0] + 16, 
                        rhs->getPosition()[1] + rhs->getScaledHeight()/2 - playerPosition[1] + 16)
                  << std::endl;
*/
                return
                  hypot(lhs->getPosition()[0] + lhs->getScaledWidth()/2 - playerPosition[0] + 16,
                        lhs->getPosition()[1] + lhs->getScaledHeight()/2 - playerPosition[1] + 16) 
                  <
                  hypot(rhs->getPosition()[0] + rhs->getScaledWidth()/2 - playerPosition[0] + 16, 
                        rhs->getPosition()[1] + rhs->getScaledHeight()/2 - playerPosition[1] + 16)
                ;}
                
  );

}
