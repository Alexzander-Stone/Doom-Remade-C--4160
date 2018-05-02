#include "hud.h"
#include "viewport.h"
#include "renderContext.h"
#include "ioMod.h"
#include "clock.h"
#include <sstream>

Hud::Hud() : 
  renderer(RenderContext::getInstance()->getRenderer()),
  background("Hud"),
  doom_head("Hud/DoomFace"),
  shells(),
  screen_ratio((((Gamedata::getInstance().getXmlFloat("view/width") + (Gamedata::getInstance().getXmlFloat("view/height") )) / 
        ( Gamedata::getInstance().getXmlFloat(background.getName() + "/xRes") + Gamedata::getInstance().getXmlFloat(background.getName() + "/yRes"))))),
  active(true),
  ending(false),
  player(nullptr)
{
  background.setScale(screen_ratio);
  doom_head.setScale(screen_ratio);
  
}

Hud::Hud(Player* p) : 
  renderer(RenderContext::getInstance()->getRenderer()),
  background("Hud"),
  doom_head("Hud/DoomFace"),
  shells(),
  screen_ratio((((Gamedata::getInstance().getXmlFloat("view/width") + (Gamedata::getInstance().getXmlFloat("view/height") )) / 
        ( Gamedata::getInstance().getXmlFloat(background.getName() + "/xRes") + Gamedata::getInstance().getXmlFloat(background.getName() + "/yRes"))))),
  active(true),
  ending(false),
  player(p)
{
  background.setScale(screen_ratio);
  doom_head.setScale(screen_ratio);
  shells.reserve(Gamedata::getInstance().getXmlInt( Gamedata::getInstance().getXmlStr(player->getName()+"/bullet") + "/ammo"));
  for( int i = 0; i < Gamedata::getInstance().getXmlInt( Gamedata::getInstance().getXmlStr(player->getName()+"/bullet") + "/ammo"); i++ ) {
    Sprite* newShell =  new Sprite("Hud/Shell");
    newShell->setScale(1);
    newShell->setPosition( Vector2f(210 - newShell->getScaledWidth()/2 + 50 * i, Gamedata::getInstance().getXmlInt("view/height") - 50 - newShell->getScaledHeight()/2) );
    std::cout << newShell->getX() << " and " << newShell->getY() << std::endl;
    shells.push_back(newShell);
  }
}

Hud::Hud(const Hud& h) : 
  renderer(RenderContext::getInstance()->getRenderer()),
  background("Hud"),
  doom_head("Hud/DoomFace"),
  shells(),
  screen_ratio((((Gamedata::getInstance().getXmlFloat("view/width") + (Gamedata::getInstance().getXmlFloat("view/height") )) / 
        ( Gamedata::getInstance().getXmlFloat(background.getName() + "/xRes") + Gamedata::getInstance().getXmlFloat(background.getName() + "/yRes"))))),
  active(true),
  ending(false),
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
    newShell->setScale(.5);
    newShell->setPosition( Vector2f(210 - newShell->getScaledWidth()/2 + 50 * i, Gamedata::getInstance().getXmlInt("view/height") - 50 - newShell->getScaledHeight()) );
    std::cout << newShell->getX() << " and " << newShell->getY() << std::endl;
    shells.push_back(newShell);
  }
}

void Hud::draw() const {
  background.draw();
  doom_head.draw();

  if(ending == false) {
    // Draw FPS.
    SDL_SetRenderDrawColor( renderer, 208, 9, 0, 255 );

    std::stringstream fpsStream;
    fpsStream << Clock::getInstance().getFps();
    string fpsCounter = "FPS: " + fpsStream.str();
    IoMod::getInstance().
        writeText(fpsCounter, Gamedata::getInstance().getXmlInt("view/width") - 100, 0);
    // Draw name.
    IoMod::getInstance().
        writeText("Alexzander Stone", 
      Gamedata::getInstance().getXmlInt("font/size") * 9, 
      Gamedata::getInstance().getXmlInt("view/height") - background.getScaledHeight()/2);

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
  else { // Ending is true;
    // Menu for text to sit on.
    SDL_Rect menu;
    menu.x = Gamedata::getInstance().getXmlFloat("view/width") * .1;
    menu.y = Gamedata::getInstance().getXmlFloat("view/height") * .1;
    menu.w = Gamedata::getInstance().getXmlFloat("view/width") * .8;
    menu.h = Gamedata::getInstance().getXmlFloat("view/height") * .65;

    SDL_SetRenderDrawColor( renderer, 200, 200, 200, 255 );
    SDL_RenderDrawRect(renderer, &menu);

    SDL_SetRenderDrawColor( renderer, 200, 200, 200, 200 );
    SDL_RenderFillRect(renderer, &menu);

    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 50 );
    if(ending == 1) { // Player has won.
      IoMod::getInstance().
          writeText(" You defeated the monster! ", 
            Gamedata::getInstance().getXmlFloat("view/width") * .5 - Gamedata::getInstance().getXmlFloat("font/size")*6 - 10, 
            Gamedata::getInstance().getXmlFloat("view/height") * .2, 
            SDL_Color({255, 255, 255, 255})
          );
    }
    else {  // Enemy has won
      IoMod::getInstance().
          writeText(" Death awaits you... ", 
            Gamedata::getInstance().getXmlFloat("view/width") * .5 - Gamedata::getInstance().getXmlFloat("font/size")*2 - 10, 
            Gamedata::getInstance().getXmlFloat("view/height") * .2, 
            SDL_Color({255, 255, 255, 255})
          );
    }
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
