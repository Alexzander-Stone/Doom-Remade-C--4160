#include "hud.h"

Hud::Hud() : background("Hud"){}

void Hud::draw() const {
  background.draw();
}

void Hud::update() {
  background.setX();
}
