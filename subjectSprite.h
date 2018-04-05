#ifndef SUBJECTSPRITE__H
#define SUBJECTSPRITE__H

#include <string>
#include <list>
#include <cmath>
#include "multisprite.h"

class SmartSprite;

class SubjectSprite : public MultiSprite {
public:
  SubjectSprite(const std::string&);
  SubjectSprite(const SubjectSprite&);

  void update(Uint32 ticks);
  void attach( SmartSprite* o ) { colliders.push_back(o); } 
  void detach( SmartSprite* o );
	std::list<SmartSprite*>& getColliders() { return colliders; } 

protected:
  std::list<SmartSprite*> colliders;
  SubjectSprite& operator=(const SubjectSprite&);
};
#endif
