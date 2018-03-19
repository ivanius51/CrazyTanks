#ifndef GAME_H
#define GAME_H

#include <vector>

#include <windows.h>

#include <GameObject.h>

class Game
{
  public:
    //singleton
    static Game& instance()
    {
      static Game *instance = new Game();
      return *instance;
    }
    Init(HWND handle, int maxtiles);
    void Free();

    HDC hdc = 0;
    HWND hwnd = 0;

    HBITMAP buffer = 0;
    HDC bufferDC = 0;
    HBITMAP mapLayer = 0;
    HDC mapLayerDC = 0;

    unsigned short int MaxTiles(){return MAXTILES_;}
    unsigned short int TileSize(){return TILESIZE_;}

    bool running() {return isRunning_;}
    void stop() {isRunning_ = false;}

    MapTile** Tiles;

    Tank *Player = nullptr;
    unsigned short int Score = 0;
    unsigned short int Lives = 3;

    int KeyInput = 0;

    void InputHandler();
    void Update();
    void Draw();

    void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBitmap, bool transparent = false);
    void DrawBitmap(int x, int y, HBITMAP hBitmap, bool transparent = false);

    void ShowConsoleCursor(bool showFlag);
    void SetConsoleWindowSize(int x, int y);

  protected:


  private:
    Game() {}

    unsigned int StartTime;

    unsigned short int MAXTILES_ = 10;
    unsigned short int TILESIZE_ = 32;

    unsigned int frameCount;
    unsigned int updateCount;
    bool isRunning_;

    void RenderMap();

    void PreRender();//map from bitmap layer
    void Render();//objects layer
    void PostRender();//GUI layer
};

#endif // GAME_H
