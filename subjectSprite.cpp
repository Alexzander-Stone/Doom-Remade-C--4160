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
