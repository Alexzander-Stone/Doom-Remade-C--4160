#include "subjectSprite.h"
#include "smartSprite.h"
#include "gamedata.h"
#include "renderContext.h"
#include <algorithm>

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

      std::cout <<  hypot(lhs->getPosition()[0] - playerPosition[0],
                        lhs->getPosition()[1] - playerPosition[1]) 
                  << " and " <<
                  hypot(rhs->getPosition()[0] - playerPosition[0], 
                        rhs->getPosition()[1] - playerPosition[1])
                  << std::endl;

                return
                  hypot(lhs->getPosition()[0] - playerPosition[0],
                        lhs->getPosition()[1] - playerPosition[1]) 
                  <
                  hypot(rhs->getPosition()[0] - playerPosition[0], 
                        rhs->getPosition()[1] - playerPosition[1])
                ;}
                
  );

}
