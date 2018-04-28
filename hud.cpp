#include "hud.h"
#include "viewport.h"
#include "renderContext.h"
#include "ioMod.h"
#include "clock.h"
#include <sstream>

Hud::Hud() : 
background("Hud"),
doom_head("Hud/DoomFace"),
shells(),
screen_ratio((((Gamedata::getInstance().getXmlFloat("view/width") + (Gamedata::getInstance().getXmlFloat("view/height") )) / 
	    ( Gamedata::getInstance().getXmlFloat(background.getName() + "/xRes") + Gamedata::getInstance().getXmlFloat(background.getName() + "/yRes"))))),
active(true),
player(nullptr)
{
  background.setScale(screen_ratio);
  doom_head.setScale(screen_ratio);
  
}

Hud::Hud(Player* p) : 
background("Hud"),
doom_head("Hud/DoomFace"),
shells(),
screen_ratio((((Gamedata::getInstance().getXmlFloat("view/width") + (Gamedata::getInstance().getXmlFloat("view/height") )) / 
	    ( Gamedata::getInstance().getXmlFloat(background.getName() + "/xRes") + Gamedata::getInstance().getXmlFloat(background.getName() + "/yRes"))))),
active(true),
player(p)
{
  background.setScale(screen_ratio);
  doom_head.setScale(screen_ratio);
  shells.reserve(Gamedata::getInstance().getXmlInt( Gamedata::getInstance().getXmlStr(player->getName()+"/bullet") + "/ammo"));
  for( int i = 0; i < Gamedata::getInstance().getXmlInt( Gamedata::getInstance().getXmlStr(player->getName()+"/bullet") + "/ammo"); i++ ) {
    Sprite* newShell =  new Sprite("Hud/Shell");
    newShell->setScale(5);
    newShell->setPosition( Vector2f(210 + 50 * i, Gamedata::getInstance().getXmlInt("view/height") - 50) );
    shells.push_back(newShell);
  }
}

Hud::Hud(const Hud& h) : 
background("Hud"),
doom_head("Hud/DoomFace"),
shells(),
screen_ratio((((Gamedata::getInstance().getXmlFloat("view/width") + (Gamedata::getInstance().getXmlFloat("view/height") )) / 
	    ( Gamedata::getInstance().getXmlFloat(background.getName() + "/xRes") + Gamedata::getInstance().getXmlFloat(background.getName() + "/yRes"))))),
active(true),
player(h.player)
{
  background.setScale(screen_ratio);
  doom_head.setScale(screen_ratio);
  auto itr = shells.begin();
  while (itr != shells.end());
  {
    delete *itr;
    itr = shells.erase(itr);
  }
}

Hud& Hud::operator=(const Hud& h){
  if(this == &h) return *this;
  player = h.player;
  return *this;
}

void Hud::setPlayer(Player* p){
  player = p;
  shells.reserve(Gamedata::getInstance().getXmlInt( Gamedata::getInstance().getXmlStr(player->getName()+"/bullet") + "/ammo"));
  for( int i = 0; i < Gamedata::getInstance().getXmlInt( Gamedata::getInstance().getXmlStr(player->getName()+"/bullet") + "/ammo"); i++ ) {
    Sprite* newShell =  new Sprite("Hud/Shell");
    newShell->setScale(5);
    newShell->setPosition( Vector2f(210 + 50 * i, Gamedata::getInstance().getXmlInt("view/height") - 50) );
    shells.push_back(newShell);
  }
}

void Hud::draw() const {
  background.draw();
  doom_head.draw();
  
  // Draw FPS.
  SDL_SetRenderDrawColor( RenderContext::getInstance()->getRenderer(), 208, 9, 0, 255 );

  std::stringstream fpsStream;
  fpsStream << Clock::getInstance().getFps();
  string fpsCounter = "FPS: " + fpsStream.str();
  IoMod::getInstance().
      writeText(fpsCounter, Gamedata::getInstance().getXmlInt("view/width") - 100, 0);
  // Draw name.
  IoMod::getInstance().
      writeText("Alexzander Stone", 
		Gamedata::getInstance().getXmlInt("font/size") * 9, 
		Gamedata::getInstance().getXmlInt("view/height") - Gamedata::getInstance().getXmlInt("font/size")/2 - background.getScaledHeight()/2);

  // Draw instructions.
  IoMod::getInstance().
      writeText("Use WASD to move.", 
		Gamedata::getInstance().getXmlInt("view/width") - Gamedata::getInstance().getXmlInt("font/size") * 13, 
		Gamedata::getInstance().getXmlInt("view/height") - Gamedata::getInstance().getXmlInt("font/size") * 4 - 20);
  
  IoMod::getInstance().
      writeText("Use left/right arrow to rotate.", 
		Gamedata::getInstance().getXmlInt("view/width") - Gamedata::getInstance().getXmlInt("font/size") * 13, 
		Gamedata::getInstance().getXmlInt("view/height") - Gamedata::getInstance().getXmlInt("font/size") * 3 - 15);

  IoMod::getInstance().
      writeText("Press left shift to shoot.", 
		Gamedata::getInstance().getXmlInt("view/width") - Gamedata::getInstance().getXmlInt("font/size") * 13, 
		Gamedata::getInstance().getXmlInt("view/height") - Gamedata::getInstance().getXmlInt("font/size") * 2 - 10);

  IoMod::getInstance().
      writeText("F1 to remove hud.", 
		Gamedata::getInstance().getXmlInt("view/width") - Gamedata::getInstance().getXmlInt("font/size") * 13, 
		Gamedata::getInstance().getXmlInt("view/height") - Gamedata::getInstance().getXmlInt("font/size") * 1 - 5);

  // Draw the number of projectiles available to the player.
  for( unsigned int i = 0; i < player->getFreeBulletCount(); i++ ) {
    shells[i]->draw();
  }
}

void Hud::update(Uint32 ticks) {
  background.setX(Viewport::getInstance().getX());
  background.setY(Viewport::getInstance().getY() + Gamedata::getInstance().getXmlInt("view/height") - background.getScaledHeight());
  doom_head.setX(Viewport::getInstance().getX() + background.getScaledWidth()/2 - doom_head.getScaledWidth()/2);
  doom_head.setY(Viewport::getInstance().getY() + Gamedata::getInstance().getXmlInt("view/height") - background.getScaledHeight());
  
  doom_head.update(ticks);
}

Hud::~Hud(){
  for(auto ptr : shells)
  {
    delete ptr;
  }
}
