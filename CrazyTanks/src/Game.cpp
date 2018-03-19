#include "Game.h"
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>

#include <stdlib.h>

#include <windows.h>
#include <conio.h>

Game::Init(HWND handle, int maxtiles)
{
  if (!isRunning_)
  {
    isRunning_ = false;

    //Set a handle
    hwnd = handle;
    //Get a handle to device context
    hdc = GetDC(hwnd);

    MAXTILES_ = maxtiles;

    SetConsoleWindowSize(40,28);
    ShowConsoleCursor(false);
    system("cls");
    SetBkMode(hdc, TRANSPARENT);

    Score = 0;
    Lives = 3;

    frameCount = 1;
    updateCount = 1;

    Tiles = new MapTile*[MAXTILES_*MAXTILES_];

    bufferDC = CreateCompatibleDC(hdc);
    buffer = CreateCompatibleBitmap(bufferDC,320,340);
    SelectObject(bufferDC,buffer);
    BitBlt(bufferDC,0,0,320,340,bufferDC,0,0,BLACKNESS);

    for(int i = 0; i < MAXTILES_; ++i)
      for(int j = 0; j < MAXTILES_; ++j)
      {
        int random = (rand()%100);
        //create walls
        if (random < 20)
          Tiles[i * MAXTILES_ + j] = new Wall(hdc, RGB(0, 0, 128), i, j);
        else
          //create enemys
        if (random < 25)
        {
          Tiles[i * MAXTILES_ + j] = new Tank(hdc, RGB(128, 0, 0), i, j);
          Tiles[i * MAXTILES_ + j]->UpdateDelay = 1250 + rand()%750;
          dynamic_cast<Tank*>(Tiles[i * MAXTILES_ + j])->ShootDelay = 250;// + rand()%750;
        }
        else
          Tiles[i * MAXTILES_ + j] = nullptr;
      }

    RenderMap();

    Player = new Tank(hdc, RGB(0, 128, 0), 5, 9);
    Tiles[5 * MAXTILES_ + 9] = Player;

    isRunning_ = true;
    StartTime=GetTickCount();
  }
}

void Game::Free()
{
  if (hwnd!=0 && hdc!=0)
  {
    BitBlt(hdc,0,0,320,322,hdc,0,0,BLACKNESS);
    ReleaseDC(hwnd, hdc);
  }
  if (bufferDC)
  {
    DeleteDC(bufferDC);
    bufferDC = 0;
  }
  if (mapLayerDC)
  {
    DeleteDC(mapLayerDC);
    mapLayerDC = 0;
  }
  if (buffer)
  {
    DeleteObject(buffer);
    buffer = 0;
  }
  if (mapLayer)
  {
    DeleteObject(mapLayer);
    mapLayer = 0;
  }
  system("cls");
  for(int i = 0; i < MAXTILES_; ++i)
        for(int j = 0; j < MAXTILES_; ++j)
          if (Tiles[i * MAXTILES_ + j])
            delete Tiles[i * MAXTILES_ + j];
}

void Game::InputHandler()
{
  if (kbhit()!=0)
  {
    KeyInput = getch();
    if (KeyInput==0 || KeyInput==0xE0) KeyInput=getch();
    //esc
    if(KeyInput == 0x1B) isRunning_=false;
    //space
    if((KeyInput == 32)||(GetAsyncKeyState(VK_SPACE) & 0x80000000))
      Player->Shoot();
    if (!Player->InAnimation)
    {
      //left
      if(KeyInput == 75)
      {
        Player->Direction.y=0;
        if (Player->Direction.x == -1)
          Player->InAnimation = true;
        else
          Player->Direction.x = -1;
          //Player->InAnimation = true;
      }
      //right
      if(KeyInput == 77)
      {
        Player->Direction.y=0;
        if (Player->Direction.x == 1)
          Player->InAnimation = true;
        else
          Player->Direction.x = 1;
          //Player->InAnimation = true;
      }
      //up
      if(KeyInput == 72)
      {
        Player->Direction.x = 0;
        if (Player->Direction.y == -1)
          Player->InAnimation = true;
        else
          Player->Direction.y=-1;
          //Player->InAnimation = true;
      }
      //down
      if(KeyInput == 80)
      {
        Player->Direction.x = 0;
        if (Player->Direction.y == 1)
          Player->InAnimation = true;
        else
          Player->Direction.y = 1;
          //Player->InAnimation = true;
      }
    }
  }
}

void Game::Update()
{
  for(int i = 0; i < MAXTILES_; ++i)
    for(int j = 0; j < MAXTILES_; ++j)
      if (Tiles[i * MAXTILES_ + j])
      {
        (Tiles[i * MAXTILES_ + j])->Update();
      }
  updateCount++;
}

void Game::RenderMap()
{
  if (!mapLayer)
  {
    mapLayerDC = CreateCompatibleDC(hdc);
    mapLayer = CreateCompatibleBitmap(hdc,320,322);
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(mapLayerDC,mapLayer));
    if (hOldBitmap)
    {
      BitBlt(hdc,0,0,320,322,hdc,0,0,BLACKNESS);
      HPEN winPen = CreatePen(PS_SOLID,1,RGB(128,128,128));
      HGDIOBJ penOld = SelectObject(mapLayerDC,winPen);
      HBRUSH winBrush = CreateSolidBrush(RGB(0,0,0));
      HGDIOBJ brushOld = SelectObject(mapLayerDC,winBrush);
      Rectangle(mapLayerDC,0,0,320,322);
      //SelectObject(bmpDc,brushOld);
      SelectObject(mapLayerDC,penOld);

      for(int i = 0; i < MAXTILES_; ++i)
        for(int j = 0; j < MAXTILES_; ++j)
          if (Tiles[i * MAXTILES_ + j] && dynamic_cast<Wall*>(Tiles[i * MAXTILES_ + j]))
          {
            dynamic_cast<Wall*>(Tiles[i * MAXTILES_ + j])->Draw();
            DrawBitmap(mapLayerDC,TILESIZE_*i + 1, TILESIZE_*j + 1,dynamic_cast<Wall*>(Tiles[i * MAXTILES_ + j])->GethBitmap());
          }
      SelectObject(mapLayerDC,hOldBitmap);
    }
    //DeleteDC(mapLayerDC);
    //mapLayerDC = 0;
  }
}

void Game::PreRender()
{
  system("cls");
  //BitBlt(hdc,0,0,320,340,hdc,0,0,BLACKNESS);//WHITENESS
  DrawBitmap(hdc,0,0,mapLayer);
}

void Game::Render()
{
  for(int i = 0; i < MAXTILES_; ++i)
    for(int j = 0; j < MAXTILES_; ++j)
      if (Tiles[i * MAXTILES_ + j] && Tiles[i * MAXTILES_ + j]->IsMovable)
        Tiles[i * MAXTILES_ + j]->Draw();
}

/*
void Game::Render()
{
  for(int i = 0; i < MAXTILES_; ++i)
    for(int j = 0; j < MAXTILES_; ++j)
      if (Tiles[i * MAXTILES_ + j] && dynamic_cast<Tank*>(Tiles[i * MAXTILES_ + j]))
      {
        Tiles[i * MAXTILES_ + j]->DrawToBuffer();
        DrawBitmap(hdc,TILESIZE_*i + dynamic_cast<Tank*>(Tiles[i * MAXTILES_ + j])->Offset.x, TILESIZE_*j + dynamic_cast<Tank*>(Tiles[i * MAXTILES_ + j])->Offset.y,dynamic_cast<Tank*>(Tiles[i * MAXTILES_ + j])->GethBitmap(), true);
      }
  //Player->Draw();
  //BitBlt(hdc,0,0,320,322,bufferDC,0,0,SRCPAINT);
}*/

void Game::PostRender()
{
  //BitBlt(hdc,0,322,320,18,hdc,0,0,BLACKNESS);
  BitBlt(hdc,0,322,320,18,hdc,0,0,WHITENESS);
  INT32 Time=GetTickCount()-StartTime;
  int Sec = (Time / 1000)%60;
  int Min = ((Time / 60000) % 60);
  std::string strTime = "Time "+SSTR(Min)+":"+SSTR(Sec);
  TextOut(hdc,0,322,strTime.c_str(),strTime.length());

  SIZE textsize;
  int indentx = 2;
  std::string strLifes = "Score:"+SSTR(Score)+" Lives:"+SSTR(Lives);//+to_string(Lives);
  GetTextExtentPoint32(hdc,strLifes.c_str(),strLifes.length(),&textsize);
  indentx += textsize.cx;
  TextOut(hdc,320-indentx,322,strLifes.c_str(),strLifes.length());
  TextOut(hdc,340,340,"0",1);//test

  //BitBlt(hdc,0,0,320,340,bufferDC,0,0,SRCPAINT);
}

void Game::Draw()
{
  frameCount++;
  //must be all this draw to buffer
  PreRender();
  Render();
  PostRender();
  //and here draw from buffer for remove flikering
}

void Game::SetConsoleWindowSize(int x, int y)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    if (h == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Unable to get stdout handle.");

    {
        COORD largestSize = GetLargestConsoleWindowSize(h);
        if (x > largestSize.X)
            throw std::invalid_argument("The x dimension is too large.");
        if (y > largestSize.Y)
            throw std::invalid_argument("The y dimension is too large.");
    }


    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    if (!GetConsoleScreenBufferInfo(h, &bufferInfo))
        throw std::runtime_error("Unable to retrieve screen buffer info.");

    SMALL_RECT& winInfo = bufferInfo.srWindow;
    COORD windowSize = { winInfo.Right - winInfo.Left + 1, winInfo.Bottom - winInfo.Top + 1};

    if (windowSize.X > x || windowSize.Y > y)
    {
        SMALL_RECT info =
        {
            0,
            0,
            x < windowSize.X ? x-1 : windowSize.X-1,
            y < windowSize.Y ? y-1 : windowSize.Y-1
        };

        if (!SetConsoleWindowInfo(h, TRUE, &info))
            throw std::runtime_error("Unable to resize window before resizing buffer.");
    }

    COORD size = { x, y };
    if (!SetConsoleScreenBufferSize(h, size))
        throw std::runtime_error("Unable to resize screen buffer.");


    SMALL_RECT info = { 0, 0, x - 1, y - 1 };
    if (!SetConsoleWindowInfo(h, TRUE, &info))
        throw std::runtime_error("Unable to resize window after resizing buffer.");
}

void Game::ShowConsoleCursor(bool showFlag)
{
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_CURSOR_INFO     cursorInfo;

  GetConsoleCursorInfo(hOut, &cursorInfo);
  cursorInfo.bVisible = showFlag;
  SetConsoleCursorInfo(hOut, &cursorInfo);
}

void Game::DrawBitmap(int x, int y, HBITMAP hBitmap, bool transparent)
{
  DrawBitmap(hdc,x,y,hBitmap, transparent);
}

void Game::DrawBitmap(HDC hdc, int x, int y, HBITMAP hBitmap, bool transparent)
{
  if (hBitmap)
  {
    HBITMAP hOldbm;
    HDC hMemDC;
    BITMAP bm;
    POINT ptSize, ptOrg;

    hMemDC = CreateCompatibleDC(hdc);

    hOldbm = static_cast<HBITMAP>(SelectObject(hMemDC, hBitmap));

    if (hOldbm)
    {
      SetMapMode(hMemDC, GetMapMode(hdc));

      GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);

      ptSize.x = bm.bmWidth;
      ptSize.y = bm.bmHeight;

      DPtoLP(hdc, &ptSize, 1);

      ptOrg.x = 0;
      ptOrg.y = 0;

      DPtoLP(hMemDC, &ptOrg, 1);

      if (!transparent)
        BitBlt(hdc, x, y, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, SRCCOPY);
      else
        BitBlt(hdc, x, y, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, SRCPAINT);

      SelectObject(hMemDC, hOldbm);
    }

    DeleteDC(hMemDC);
  }
}
