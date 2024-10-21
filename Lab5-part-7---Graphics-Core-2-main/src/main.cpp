#include "../include/GameController.h"
#include "../include/SpriteAnim.h"
#include "../include/SpriteSheet.h"
#include "../include/BasicStructs.h"
// By Sannan Ali

int main() 
{
    srand(time(0));
    GameController::Instance().RunGame();
    return 0;
}
