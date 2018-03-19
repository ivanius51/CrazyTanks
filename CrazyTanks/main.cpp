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

  //COLORREF WHITE = RGB(255,255,255);
  //COLORREF DARK_GREEN = RGB(0, 128, 0);

  //HBITMAP hBitmap = static_cast<HBITMAP>(LoadImage(GetModuleHandle(NULL), "mathjoke.bmp", IMAGE_BITMAP, 0,0, LR_LOADFROMFILE));
  //Graphical *tank;
  //tank = new Tank(Game::instance().hdc, DARK_GREEN);
  //tank->Draw();
  //Draw Tank from Bitmap
  //int x = 0;

  //cout << (Game::instance().Tiles[9 * Game::instance().MaxTiles() + 5]->GetPosition().x);
  //tank->Draw();
  //Game::instance().DrawBitmap(x,0,tank->hBitmap);
  //x++;

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
  //delete tank;
  //delete game;
  Game::instance().Free();
  return 0;
}
