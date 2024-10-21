#include "../include/GameController.h"
#include "../include/Renderer.h"
#include "../include/SpriteAnim.h"
#include "../include/SpriteSheet.h"
#include "../include/BasicStructs.h"
#include "../include/TTFont.h"
#include "../include/Timing.h"

GameController::GameController()
{
    m_sdlEvent = {};
}

GameController::~GameController()
{
}

bool GameController::checkCollision(Rect rect1, Rect rect2) {
    return !(rect1.X2 < rect2.X1 ||  // Rect1 is to the left of Rect2
        rect1.X1 > rect2.X2 ||      // Rect1 is to the right of Rect2
        rect1.Y2 < rect2.Y1 ||      // Rect1 is above Rect2
        rect1.Y1 > rect2.Y2);       // Rect1 is below Rect2
}

void GameController::RunGame()
{
    std::cout << "Initializing AssetController..." << std::endl;
    AssetController::Instance().Initialize(10000000); // Allocate 10MB
    std::cout << "AssetController initialized!" << std::endl;

    std::cout << "Initializing Renderer..." << std::endl;
    Renderer* r = &Renderer::Instance();
    Timing* t = &Timing::Instance();
    r->Initialize(1920, 1080);
    std::cout << "Renderer initialized!" << std::endl;

    std::cout << "Initializing TTFont..." << std::endl;
    TTFont* font = new TTFont();
    font->Initialize(20);
    std::cout << "TTFont initialized!" << std::endl;

    // Other initialization steps
    Point ws = r->GetWindowSize();

    SpriteSheet* sheet[20];
    for (int i = 0; i < 10; i++)
    {
        SpriteSheet::Pool = new ObjectPool<SpriteSheet>();
        SpriteAnim::Pool = new ObjectPool<SpriteAnim>();
        sheet[i] = SpriteSheet::Pool->GetResource();
        sheet[i]->Load("assets/Textures/Warrior.tga");
        sheet[i]->SetSize(17, 6, 69, 44);
        sheet[i]->AddAnimation(EN_AN_IDLE, 0, 6, 0.1f);

        int randomInt = rand() % 1001;
        float randomFloat = 0.08 + (randomInt / 1000.0f) * (0.1 - 0.08);

        float normalizedSpeed = (randomFloat - 0.08f) / (0.1f - 0.08f);
        float proportionalValue = normalizedSpeed * (100.0f - 80.0f) + (80.0f);

        sheet[i]->AddAnimation(EN_AN_RUN, 6, 8, randomFloat);
        sheet[i]->AddAnimation(EN_AN_DEATH, 26, 36, 0.1f);
        sheet[i]->setSpeed(proportionalValue / 60.0);
        ofstream writeStream("resource.bin", ios::out | ios::binary);
        sheet[i]->Serialize(writeStream);
        writeStream.close();
    }

    for (int i = 10; i < 20; i++)
    {
        SpriteSheet::Pool = new ObjectPool<SpriteSheet>();
        SpriteAnim::Pool = new ObjectPool<SpriteAnim>();
        sheet[i] = SpriteSheet::Pool->GetResource();
        sheet[i]->Load("assets/Textures/Rock.tga");
        sheet[i]->SetSize(1, 4, 20, 20);       
                     
        ofstream writeStream("resource.bin", ios::out | ios::binary);
        sheet[i]->Serialize(writeStream);
        writeStream.close();
    }

    double x[10]{ 0 };
    double x2[10]{ 0 };
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    bool saved = false;
    int level = 1;

    bool alive[10];
    int count = 0;

    for (int i = 0; i < 10; i++)
        alive[i] = true;

    while (m_sdlEvent.type != SDL_QUIT)
    {
        Uint32 frameStart = SDL_GetTicks(); // Capture frame start time
        t->Tick();

        if (level == 1)
        {
            if (t->getCurrentTime() / 1000 >= 50)
            {
                if (!saved)
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

                    saved = true;
                }
            }

            SDL_PollEvent(&m_sdlEvent);
            r->SetDrawColor(Color(128, 128, 128, 0));
            r->ClearScreen();

            for (int i = 0; i < 10; i++)
            {
                x[i] += sheet[i]->getSpeed();

                r->RenderTexture(sheet[i], sheet[i]->Update(EN_AN_RUN), Rect(x[i], 10 + i * 100, 69 * 3 + x[i], 150 * i + 44 * 3));

                if (x[i] >= r->GetWindowSize().X)
                {
                    level = 2;

                    for (int i = 0; i < 10; i++)
                    {
                        x[i] = 0;

                        r->RenderTexture(sheet[i], sheet[i]->Update(EN_AN_RUN), Rect(x[i], 10 + i * 100, 69 * 3 + x[i], 150 * i + 44 * 3));                      
                    }

                    t->Reset();
                }
            }

            string display = "Frames Per Second: " + to_string(t->GetFPS());
            display += "        Game Time: " + to_string(t->getCurrentTime() / 1000);
            display += "        Auto Saved: ";

            if (t->getCurrentTime() / 1000 >= 5)
                display += "Yes";

            else
                display += "No";

            font->Write(r->GetRenderer(), display.c_str(), SDL_Color{ 0, 0, 255 }, SDL_Point{ 0, 0 });

            Uint32 frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < frameDelay)
            {
                SDL_Delay(frameDelay - frameTime); // Delay to cap at 60 FPS
            }

            SDL_RenderPresent(r->GetRenderer());
        }

        if (level == 2)
        {
            if (t->getCurrentTime() / 1000 >= 50)
            {
                if (!saved)
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

                    saved = true;
                }
            }

            SDL_PollEvent(&m_sdlEvent);
            r->SetDrawColor(Color(0, 128, 0, 0));
            r->ClearScreen();

            for (int i = 0; i < 10; i++)
            {
                if (alive[i] == true)
                {
                    x[i] += sheet[i]->getSpeed();

                    Rect playerRect = Rect(x[i], 10 + i * 100, 69 * 3 + x[i], 150 * i + 44 * 3);

                    r->RenderTexture(sheet[i], sheet[i]->Update(EN_AN_RUN), playerRect);

                    Rect rockRect = Rect(i * 100 + 50, x[i], 20 + i * 100 + 50, 20 + x[i]);
                    r->RenderTexture(sheet[i + 10], Rect(0, 0, 20, 20), rockRect);

                    if (checkCollision(playerRect, rockRect))
                    {
                        alive[i] = false;
                        count++;

                        if (count == 10)
                        {
                            r->ClearScreen();
                            SDL_RenderPresent(r->GetRenderer());

                            delete SpriteAnim::Pool;
                            delete SpriteSheet::Pool;

                            font->Shutdown();
                            r->Shutdown();
                            return;
                        }
                    }

                    if (x[i] >= r->GetWindowSize().X)
                    {
                        r->ClearScreen();
                        SDL_RenderPresent(r->GetRenderer());
                        delete SpriteAnim::Pool;
                        delete SpriteSheet::Pool;

                        font->Shutdown();
                        r->Shutdown();
                        return;
                    }

                }

                else
                {
                    if (x2[i] < 100)
                    {
                        r->RenderTexture(sheet[i], sheet[i]->Update(EN_AN_DEATH), Rect(x[i], 10 + i * 100, 69 * 3 + x[i], 150 * i + 44 * 3));
                        x2[i]++;
                    }                                  
                }
            }

            string display = "Frames Per Second: " + to_string(t->GetFPS());
            display += "        Game Time: " + to_string(t->getCurrentTime() / 1000);
            display += "        Auto Saved: ";

            if (t->getCurrentTime() / 1000 >= 5)
                display += "Yes";

            else
                display += "No";

            font->Write(r->GetRenderer(), display.c_str(), SDL_Color{ 0, 0, 255 }, SDL_Point{ 0, 0 });

            Uint32 frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < frameDelay)
            {
                SDL_Delay(frameDelay - frameTime); // Delay to cap at 60 FPS
            }

            SDL_RenderPresent(r->GetRenderer());
        }
    }

    delete SpriteAnim::Pool;
    delete SpriteSheet::Pool;

    font->Shutdown();
    r->Shutdown();
}