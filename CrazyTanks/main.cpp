//Code Blocks: Project Build Options Linker settings Other linker options: add -lgdi32
#define _WIN32_WINNT 0x0501
#define WINVER 0x0410

#include <chrono>
#include <thread>

#include <iostream>

#include <windows.h>

#include <GameObject.h>
#include <Game.h>

using namespace std;

int main()
{
  const int FPS = 60;
  const int FRAME_DELAY = 1000 / FPS;

  Game::instance().Init(GetConsoleWindow(),10);

  while(Game::instance().running())
  {
    int startTime = GetTickCount();

    Game::instance().InputHandler();
    Game::instance().Update();
    Game::instance().Draw();

    int elapsedTime = GetTickCount() - startTime;

    if (FRAME_DELAY > elapsedTime)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY - elapsedTime));
    }
  }
  Game::instance().Free();
  return 0;
}
