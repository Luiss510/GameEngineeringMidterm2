#include "../include/GameController.h"
#include "../include/Renderer.h"
#include "../include/SpriteAnim.h"
#include "../include/SpriteSheet.h"
#include "../include/BasicStructs.h"
#include "../include/TTFont.h"
#include "../include/Timing.h"

GameController::GameController()
    : r(nullptr), t(nullptr), font(nullptr), autoSaveFlag(false), levelStartTime(0)
{
    memset(&m_sdlEvent, 0, sizeof(m_sdlEvent)); // Initialize the SDL event structure
}

GameController::~GameController()
{
}

void GameController::RunGame()
{
    AssetController::Instance().Initialize(10000000);  // AssetController Initialization - Allocated 10MB
    std::cout << "AssetController initialized!" << std::endl;

    r = &Renderer::Instance();                         // Renderer Initialization
    t = &Timing::Instance();
    r->Initialize(1920, 1080);
    std::cout << "Renderer initialized!" << std::endl;

    font = new TTFont();                               // Font Initialization
    font->Initialize(20);
    std::cout << "TTFont initialized!" << std::endl;

    Point ws = r->GetWindowSize();

    SpriteSheet* sheet[10];
    for (int i = 0; i < 10; i++)
    {
        SpriteSheet::Pool = new ObjectPool<SpriteSheet>();
        SpriteAnim::Pool = new ObjectPool<SpriteAnim>();
        sheet[i] = SpriteSheet::Pool->GetResource();
        sheet[i]->Load("assets/Textures/Warrior.tga");
        sheet[i]->SetSize(17, 6, 69, 44);
        sheet[i]->AddAnimation(EN_AN_RUN, 6, 8, 2.50f);
        int randomInt = rand() % 1001;
        float randomFloat = 0.08 + (randomInt / 1000.0f) * (0.1 - 0.08);
        float normalizedSpeed = (randomFloat - 0.08f) / (0.1f - 0.08f);
        float proportionalValue = normalizedSpeed * (100.0f - 80.0f) + (80.0f);
        sheet[i]->AddAnimation(EN_AN_RUN, 6, 8, randomFloat);
        sheet[i]->AddAnimation(EN_AN_DEATH, 24, 11, 0.1f);
        sheet[i]->setSpeed(proportionalValue / 60.0);
        ofstream writeStream("resource.bin", ios::out | ios::binary);
        sheet[i]->Serialize(writeStream);
        writeStream.close();
    }

    bool serialized = false;                           // Rendering logic for Level 1
    double x[10]{ 0 };
    int level = 1;
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    while (m_sdlEvent.type != SDL_QUIT)
    {
        Uint32 frameStart = SDL_GetTicks();            // Start time recorded
        t->Tick();

        if (level == 1)
        {
            if (t->CurrentTime() / 1000 >= 5)
            {
                if (!serialized)
                {
                    ofstream writeStream("Level1.bin", ios::out | ios::binary);

                    for (int i = 0; i < 10; i++)
                    {
                        sheet[i]->Serialize(writeStream);
                    }

                    writeStream.close();

                    ifstream readStream("Level1.bin", ios::in | ios::binary);

                    for (int i = 0; i < 10; i++)
                    {
                        sheet[i]->Deserialize(readStream);
                    }

                    readStream.close();
                    serialized = true;
                }
            }
        }

        SDL_PollEvent(&m_sdlEvent);
        r->SetDrawColorGrey(Color(125, 125, 125, 255));
        r->ClearScreen();

        for (int i = 0; i < 10; i++)
        {
            x[i] += sheet[i]->getSpeed();
            r->RenderTexture(sheet[i], sheet[i]->Update(EN_AN_RUN, t->GetDeltaTime()), Rect(x[i], 10 + i * 100, 69 * 3 + x[i], 150 * i + 44 * 3));
            if (x[i] >= r->GetWindowSize().X)
            {
                //currentLevel == GameLevel::LEVEL_TWO;
                level = 2;
            }
        }
        /*
        // Display HUD: FPS, Game Time, Auto-save Status side by side
        std::string fps = "Frames Per Second: " + std::to_string(t->GetFPS());
        int fpsWidth = font->GetTextWidth(fps.c_str());
        int gap = 30;
        SDL_Point fpsPosition = { 0, 0 };
        font->Write(r->GetRenderer(), fps.c_str(), SDL_Color{ 0, 0, 255 }, fpsPosition);
        // Game Time
        Uint32 currentTime = SDL_GetTicks(); // Get the current time in milliseconds
        Uint32 gameTimeElapsed = (currentTime - levelStartTime) / 1000; // Convert to seconds
        std::string gameTime = "Game Time: " + std::to_string(gameTimeElapsed) + " seconds";
        SDL_Point gameTimePosition = { fpsPosition.x + fpsWidth + gap, 0 }; // Place next to FPS label with a gap of 30 pixels
        font->Write(r->GetRenderer(), gameTime.c_str(), SDL_Color{ 0, 0, 255 }, gameTimePosition);
        int gameTimeWidth = font->GetTextWidth(gameTime.c_str());
        // Auto-Save
        std::string autoSaveStatus;
        if (autoSaveFlag)
        {
            Uint32 autoSaveElapsed = (currentTime - levelStartTime) / 1000;
            autoSaveStatus = "Status: Auto Saved: " + std::to_string(autoSaveElapsed) + " seconds ago";
        } else{
            autoSaveStatus = "Status: Not auto-saved yet";
        }
        SDL_Point autoSavePosition = { gameTimePosition.x + gameTimeWidth + gap, 0 }; // Place next to Game Time label with a gap of 30 pixels
        font->Write(r->GetRenderer(), autoSaveStatus.c_str(), SDL_Color{ 0, 0, 255 }, autoSavePosition);
        */

        string display = "Frames Per Second: " + to_string(t->GetFPS());
        display += "        Game Time: " + to_string(t->CurrentTime() / 1000);
        display += "        Auto Saved: ";

        if (t->CurrentTime() / 1000 >= 5)
        {
            display += "Yes";
        }
        else {
            display += "No";
        }
        font->Write(r->GetRenderer(), display.c_str(), SDL_Color{ 0, 0, 255 }, SDL_Point{ 0, 0 });

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay)
        {
            SDL_Delay(frameDelay - frameTime);
        }

        SDL_RenderPresent(r->GetRenderer());

        if (level == 2)
        {
            if (t->CurrentTime() / 1000 >= 5)
            {
                if (!serialized)
                {
                    ofstream writeStream("Level2.bin", ios::out | ios::binary);
                    for (int i = 0; i < 10; i++)
                    {
                        sheet[i]->Serialize(writeStream);
                    }
                    writeStream.close();
                    ifstream readStream("Level2.bin", ios::in | ios::binary);
                    for (int i = 0; i < 10; i++)
                    {
                        sheet[i]->Deserialize(readStream);
                    }
                    readStream.close();
                    serialized = true;
                }
            }

            SDL_PollEvent(&m_sdlEvent);
            r->SetDrawColorGreen(Color(0, 255, 0, 255));
            r->ClearScreen();
            for (int i = 0; i < 10; i++)
            {
                x[i] += sheet[i]->getSpeed();
                r->RenderTexture(sheet[i], sheet[i]->Update(EN_AN_RUN, t->GetDeltaTime()), Rect(x[i], 10 + i * 100, 69 * 3 + x[i], 150 * i + 44 * 3));
                if (x[i] >= r->GetWindowSize().X)
                {
                    level = 2;
                }
            }
            string display = "Frames Per Second: " + to_string(t->GetFPS());
            display += "        Game Time: " + to_string(t->CurrentTime() / 1000);
            display += "        Auto Saved: ";
            if (t->CurrentTime() / 1000 >= 5)
            {
                display += "Yes";
            }
            else
            {
                display += "No";
            }

            font->Write(r->GetRenderer(), display.c_str(), SDL_Color{ 0, 0, 255 }, SDL_Point{ 0, 0 });
            Uint32 frameTime = SDL_GetTicks() - frameStart;

            if (frameTime < frameDelay)
            {
                SDL_Delay(frameDelay - frameTime);
            }
            SDL_RenderPresent(r->GetRenderer());
        }
    }
    delete SpriteAnim::Pool;
    delete SpriteSheet::Pool;

    if (font) {
        font->Shutdown();
        delete font;
    }
    r->Shutdown();
}
        