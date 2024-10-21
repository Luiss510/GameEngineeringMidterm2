#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "StandardIncludes.h"
#include "BasicStructs.h"
#include "SpriteSheet.h"

#include <SDL.h>

class Renderer;
class Timing;
class TTFont;

class GameController : public Singleton<GameController> {
public:
    // Constructors/Destructors
    GameController();
    virtual ~GameController();

    // Methods
    void RunGame();                  // Main game loop

private:
    // Members
    SDL_Event m_sdlEvent;
    Uint32 levelStartTime;
    Renderer* r;
    Timing* t;
    TTFont* font;
    SpriteSheet* sheet[10];
    bool autoSaveFlag;
};

#endif // GAME_CONTROLLER_H
