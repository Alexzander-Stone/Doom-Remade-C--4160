#include "hud.h"
#include "viewport.h"
#include "renderContext.h"
#include "ioMod.h"
#include "clock.h"
#include <sstream>

Hud::Hud() : 
background("Hud"),
doom_head("Hud/DoomFace"),
screen_ratio((((Gamedata::getInstance().getXmlFloat("view/width") + (Gamedata::getInstance().getXmlFloat("view/height") )) / 
	    ( Gamedata::getInstance().getXmlFloat(background.getName() + "/xRes") + Gamedata::getInstance().getXmlFloat(background.getName() + "/yRes"))))),
active(true)
{
  background.setScale(screen_ratio);
  doom_head.setScale(screen_ratio);
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
		Gamedata::getInstance().getXmlInt("view/height") - Gamedata::getInstance().getXmlInt("font/size") * 6);
  
  IoMod::getInstance().
      writeText("Use left/right arrow to rotate.", 
		Gamedata::getInstance().getXmlInt("view/width") - Gamedata::getInstance().getXmlInt("font/size") * 13, 
		Gamedata::getInstance().getXmlInt("view/height") - Gamedata::getInstance().getXmlInt("font/size") * 4);

  IoMod::getInstance().
      writeText("F1 to remove hud.", 
		Gamedata::getInstance().getXmlInt("view/width") - Gamedata::getInstance().getXmlInt("font/size") * 13, 
		Gamedata::getInstance().getXmlInt("view/height") - Gamedata::getInstance().getXmlInt("font/size") * 2);
}

void Hud::update(Uint32 ticks) {
  background.setX(Viewport::getInstance().getX());
  background.setY(Viewport::getInstance().getY() + Gamedata::getInstance().getXmlInt("view/height") - background.getScaledHeight());
  doom_head.setX(Viewport::getInstance().getX() + background.getScaledWidth()/2 - doom_head.getScaledWidth()/2);
  doom_head.setY(Viewport::getInstance().getY() + Gamedata::getInstance().getXmlInt("view/height") - background.getScaledHeight());
  
  doom_head.update(ticks);
}
