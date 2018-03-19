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

    //Tiles[0].push_back(new Tank(hdc, RGB(0, 128, 0), 5, 9));

    Tiles = new MapTile*[MAXTILES_*MAXTILES_];

    for(int i = 0; i < MAXTILES_; ++i)
      for(int j = 0; j < MAXTILES_; ++j)
      //create walls
        if ((rand()%100) < 20)
          Tiles[i * MAXTILES_ + j] = new Wall(hdc, RGB(0, 0, 128), i, j);
        else
          Tiles[i * MAXTILES_ + j] = nullptr;

    RenderMap();

    Player = new Tank(hdc, RGB(0, 128, 0), 5, 9);
    Tiles[9 * MAXTILES_ + 5] = Player;

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
  system("cls");
  for(int i = 0; i < MAXTILES_; ++i)
        for(int j = 0; j < MAXTILES_; ++j)
          if (Tiles[i * MAXTILES_ + j])
            delete Tiles[i * MAXTILES_ + j];
  //Tiles.clear();
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
    HDC bmpDc = CreateCompatibleDC(hdc);
    mapLayer = CreateCompatibleBitmap(hdc,320,322);
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(bmpDc,mapLayer));
    if (hOldBitmap)
    {
      BitBlt(hdc,0,0,320,322,hdc,0,0,BLACKNESS);
      HPEN winPen = CreatePen(PS_SOLID,1,RGB(128,128,128));
      HGDIOBJ penOld = SelectObject(bmpDc,winPen);
      HBRUSH winBrush = CreateSolidBrush(RGB(0,0,0));
      HGDIOBJ brushOld = SelectObject(bmpDc,winBrush);
      Rectangle(bmpDc,0,0,320,322);
      //SelectObject(bmpDc,brushOld);
      SelectObject(bmpDc,penOld);

      for(int i = 0; i < MAXTILES_; ++i)
        for(int j = 0; j < MAXTILES_; ++j)
          if (Tiles[i * MAXTILES_ + j] && dynamic_cast<Wall*>(Tiles[i * MAXTILES_ + j]))
          {
            dynamic_cast<Wall*>(Tiles[i * MAXTILES_ + j])->Draw();
            DrawBitmap(bmpDc,TILESIZE_*i + 1, TILESIZE_*j + 1,dynamic_cast<Wall*>(Tiles[i * MAXTILES_ + j])->GethBitmap());
          }
      SelectObject(bmpDc,hOldBitmap);
    }
    DeleteDC(bmpDc);
  }
}

void Game::PreRender()
{
  system("cls");
  //BitBlt(hdc,0,0,320,340,hdc,0,0,BLACKNESS);//WHITENESS
  DrawBitmap(0,0,mapLayer);
}

void Game::Render()
{
  for(int i = 0; i < MAXTILES_; ++i)
    for(int j = 0; j < MAXTILES_; ++j)
      if (Tiles[i * MAXTILES_ + j] && Tiles[i * MAXTILES_ + j]->IsMovable)
        Tiles[i * MAXTILES_ + j]->Draw();
}

void Game::PostRender()
{
  //BitBlt(hdc,0,322,320,18,hdc,0,0,BLACKNESS);
  BitBlt(hdc,0,322,320,18,hdc,0,0,WHITENESS);
  INT32 Time=GetTickCount()-StartTime;
  int Sec = (Time / 1000)%60;
  int Min = ((Time / 60000) % 60);
  std::string strTime = "Time "+SSTR(Min)+":"+SSTR(Sec);
  TextOut(hdc,0,321,strTime.c_str(),strTime.length());

  SIZE textsize;
  int indentx = 2;
  std::string strLifes = "Score:"+SSTR(Score)+" Lives:"+SSTR(Lives);//+to_string(Lives);
  GetTextExtentPoint32(hdc,strLifes.c_str(),strLifes.length(),&textsize);
  indentx += textsize.cx;
  TextOut(hdc,320-indentx,321,strLifes.c_str(),strLifes.length());
  TextOut(hdc,340,340,"0",1);//test
}

void Game::Draw()
{
  frameCount++;
  PreRender();
  Render();
  PostRender();
}

void Game::SetConsoleWindowSize(int x, int y)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    if (h == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Unable to get stdout handle.");

    // If either dimension is greater than the largest console window we can have,
    // there is no point in attempting the change.
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
        // window size needs to be adjusted before the buffer size can be reduced.
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
  cursorInfo.bVisible = showFlag; // set the cursor visibility
  SetConsoleCursorInfo(hOut, &cursorInfo);
}

void Game::DrawBitmap(int x, int y, HBITMAP hBitmap, bool transparent)
{
  DrawBitmap(hdc,x,y,hBitmap, transparent);
}

void Game::DrawBitmap(HDC hdc, int x, int y, HBITMAP hBitmap, bool transparent)
{
  HBITMAP hOldbm;
  HDC hMemDC;
  BITMAP bm;
  POINT ptSize, ptOrg;

  // —оздаем контекст пам€ти, совместимый
  // с контекстом отображени€
  hMemDC = CreateCompatibleDC(hdc);

  // ¬ыбираем изображение bitmap в контекст пам€ти
  hOldbm = static_cast<HBITMAP>(SelectObject(hMemDC, hBitmap));

  // ≈сли не было ошибок, продолжаем работу
  if (hOldbm)
  {
    // ƒл€ контекста пам€ти устанавливаем тот же
    // режим отображени€, что используетс€ в
    // контексте отображени€
    SetMapMode(hMemDC, GetMapMode(hdc));

    // ќпредел€ем размеры изображени€
    GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);

    ptSize.x = bm.bmWidth;  // ширина
    ptSize.y = bm.bmHeight; // высота

    // ѕреобразуем координаты устройства в логические
    // дл€ устройства вывода
    DPtoLP(hdc, &ptSize, 1);

    ptOrg.x = 0;
    ptOrg.y = 0;

    // ѕреобразуем координаты устройства в логические
    // дл€ контекста пам€ти
    DPtoLP(hMemDC, &ptOrg, 1);

    // –исуем изображение bitmap
    if (!transparent)
      BitBlt(hdc, x, y, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, SRCCOPY);
    else
      BitBlt(hdc, x, y, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, SRCPAINT);
    //TransparentBlt(hdc, x, y, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, ptSize.x, ptSize.y, RGB(0,0,0));
    // ¬осстанавливаем контекст пам€ти
    SelectObject(hMemDC, hOldbm);
  }

  // ”дал€ем контекст пам€ти
  DeleteDC(hMemDC);
}
