#include "GameObject.h"
//#include <GlobalVariables.h>
#include <cmath>
#include <Game.h>


GameObject::GameObject()
{

}
/*
GameObject::~GameObject()
{

}
*/
MapTile::MapTile(int x, int y)
{
  Position_.x = x;
  Position_.y = y;
}

Graphical::Graphical(HDC ahdc, COLORREF aLineColor, int x, int y)
  :MapTile(x,y)
{
  IsEmpty = false;
  IsMovable = false;
  IsWalkable = true;
  hdc = ahdc;
  LineColor = aLineColor;

  Direction.x = 0;
  Direction.y = -1;
  Offset.x = 1;
  Offset.y = 1;
}

Graphical::~Graphical()
{
  if (hBitmap_!=0)
  {
    DeleteObject(hBitmap_);
    hBitmap_ = 0;
  }
}

Wall::Wall(HDC ahdc, COLORREF aLineColor, int x, int y)
  :Graphical(ahdc,aLineColor,x ,y)
{
  IsWalkable = false;
}
void Wall::DrawTo(HDC ahdc)
{
  BitBlt(ahdc,0,0,32,32,ahdc,0,0,BLACKNESS);

  HPEN winPen = CreatePen(PS_SOLID,1,RGB(128,128,128));
  HGDIOBJ penOld = SelectObject(ahdc,winPen);
  HBRUSH winBrush = CreateSolidBrush(LineColor);
  HGDIOBJ brushOld = SelectObject(ahdc,winBrush);

  //base
  Rectangle(ahdc,0,0,32,32);

  SelectObject(ahdc,brushOld);
  SelectObject(ahdc,penOld);
}
void Wall::Draw()
{
  if (hBitmap_==0)
  {
    //Draw Wall to bitmap
    HDC bmpDc = CreateCompatibleDC(hdc);
    hBitmap_ = CreateCompatibleBitmap(hdc,32,32);
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(bmpDc,hBitmap_));
    if (hOldBitmap)
    {
      DrawTo(bmpDc);
      SelectObject(bmpDc,hOldBitmap);
    }
    DeleteDC(bmpDc);
  }
  int TileSize = Game::instance().TileSize();
  Game::instance().DrawBitmap(TileSize*Position_.x + 1, TileSize*Position_.y + 1, hBitmap_);
}

Bullet::Bullet(HDC ahdc, COLORREF aLineColor, int x, int y)
  :Graphical(ahdc,aLineColor, x, y)
{
  IsWalkable = true;
  IsMovable = true;
}
void Bullet::DrawToBuffer()
{
  ClearBuffer();
  Draw();
}
void Bullet::ClearBuffer()
{
  if (hBitmap_!=0)
  {
    DeleteObject(hBitmap_);
    hBitmap_ = 0;
  }
}
void Bullet::DrawTo(HDC ahdc)
{
  BitBlt(ahdc,0,0,32,32,ahdc,0,0,BLACKNESS);

  HPEN winPen = CreatePen(PS_SOLID,1,LineColor);
  HGDIOBJ penOld = SelectObject(ahdc,winPen);

  //weapon
  Rectangle(ahdc,14,10,17,22);

  SelectObject(ahdc,penOld);
}
void Bullet::Draw()
{
  if (hBitmap_==0)
  {
    //Draw Bullet to bitmap
    HDC bmpDc = CreateCompatibleDC(hdc);
    hBitmap_ = CreateCompatibleBitmap(hdc,32,32);
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(bmpDc,hBitmap_));
    if (hOldBitmap)
    {
      DrawTo(bmpDc);
      SelectObject(bmpDc,hOldBitmap);
    }
    DeleteDC(bmpDc);
  }
  else
  {
    int TileSize = Game::instance().TileSize();
    Game::instance().DrawBitmap(TileSize*Position_.x + Offset.x, TileSize*Position_.y + Offset.y, hBitmap_, true);
  }
}

void Bullet::Update()
{
  int TileSize = Game::instance().TileSize();
  int MaxTiles = Game::instance().MaxTiles();
  if (InAnimation)
  {
    POINT NewPosition = GetPosition();
    NewPosition.x += (Offset.x+Direction.x*TileSize/2)/TileSize;
    NewPosition.y += (Offset.y+Direction.y*TileSize/2)/TileSize;
    MapTile* NextTile = Game::instance().Tiles[NewPosition.x * MaxTiles + NewPosition.y];
    /*if ((abs(Offset.x)>=TileSize-1) || (abs(Offset.y)>=TileSize-1))
    {
      InAnimation = !InAnimation;
      Offset.x = 1;
      Offset.y = 1;

      if (((NewPosition.x>=0)&&(NewPosition.x<MaxTiles))&&((NewPosition.y>=0)&&(NewPosition.y<MaxTiles))&&(!NextTile || NextTile->IsWalkable))
        SetPosition(NewPosition);
    }
    else*/
    {
      Offset.x += Direction.x;
      Offset.y += Direction.y;
      if (((!(((NewPosition.x>=0)&&(NewPosition.x<MaxTiles))&&((NewPosition.y>=0)&&(NewPosition.y<MaxTiles))) || (NextTile) || (NextTile && NextTile->IsWalkable))))
      {
        InAnimation = !InAnimation;
      }
    }
  }
}

Tank::Tank(HDC ahdc, COLORREF aLineColor, int x, int y)
  :Graphical(ahdc,aLineColor, x, y)
{
  IsWalkable = false;
  IsMovable = true;
}
void Tank::DrawToBuffer()
{
  ClearBuffer();
  Draw();
}
void Tank::ClearBuffer()
{
  if (hBitmap_!=0)
  {
    DeleteObject(hBitmap_);
    hBitmap_ = 0;
  }
}
void Tank::DrawTo(HDC ahdc)
{
  BitBlt(ahdc,0,0,32,32,ahdc,0,0,BLACKNESS);

  HPEN winPen = CreatePen(PS_SOLID,1,LineColor);
  HGDIOBJ penOld = SelectObject(ahdc,winPen);

  //base
  Rectangle(ahdc,4,10,28,28);
  //left
  MoveToEx(ahdc,8,10,nullptr);
  LineTo(ahdc,8,28);
  //right
  MoveToEx(ahdc,23,10,nullptr);
  LineTo(ahdc,23,28);
  //weapon
  Rectangle(ahdc,14,4,17,16);
  //top hole
  Arc(ahdc,11,15,20,24,11,15,11,15);

  SelectObject(ahdc,penOld);
}
void Tank::Draw()
{
  if (hBitmap_==0)
  {
    //Draw Tank to bitmap
    HDC bmpDc = CreateCompatibleDC(hdc);
    hBitmap_ = CreateCompatibleBitmap(hdc,32,32);
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(bmpDc,hBitmap_));
    if (hOldBitmap)
    {
      DrawTo(bmpDc);
      SelectObject(bmpDc,hOldBitmap);
    }
    DeleteDC(bmpDc);
  }
  else
  {
    int TileSize = Game::instance().TileSize();
    Game::instance().DrawBitmap(TileSize*Position_.x + Offset.x, TileSize*Position_.y + Offset.y, hBitmap_, true);
  }
}

void Tank::Update()
{
  int TileSize = Game::instance().TileSize();
  int MaxTiles = Game::instance().MaxTiles();
  if (InAnimation)
  {
    POINT NewPosition = GetPosition();
    NewPosition.x += Direction.x;
    NewPosition.y += Direction.y;
    MapTile* NextTile = Game::instance().Tiles[NewPosition.x * MaxTiles + NewPosition.y];
    if ((abs(Offset.x)>=TileSize-1) || (abs(Offset.y)>=TileSize-1))
    {
      InAnimation = !InAnimation;
      Offset.x = 1;
      Offset.y = 1;

      if (((NewPosition.x>=0)&&(NewPosition.x<MaxTiles))&&((NewPosition.y>=0)&&(NewPosition.y<MaxTiles))&&(!NextTile || NextTile->IsWalkable))
        SetPosition(NewPosition);
    }
    else
    {
      Offset.x += Direction.x;
      Offset.y += Direction.y;
      if (((!(((NewPosition.x>=0)&&(NewPosition.x<MaxTiles))&&((NewPosition.y>=0)&&(NewPosition.y<MaxTiles))) || (NextTile) || (NextTile && NextTile->IsWalkable)))&&(abs(Offset.x)>=(TileSize/5) || abs(Offset.y)>=(TileSize/5)))
      {
        InAnimation = !InAnimation;
        Offset.x = 1;
        Offset.y = 1;
      }
    }
  }
}

void Tank::Shoot()
{
  int TileSize = Game::instance().TileSize();
  if (bullet && !bullet->InAnimation)
  {
    delete bullet;
  }
  if (!bullet || !bullet->InAnimation)
  {
    bullet = new Bullet(hdc,RGB(0,255,0),GetPosition().x+Direction.x,GetPosition().y+Direction.y);
    bullet->Offset.y = -Direction.y*(TileSize/2);
    bullet->Direction = Direction;
    bullet->InAnimation = true;
    Game::instance().Tiles[GetPosition().x+Direction.x * GetPosition().y+Direction.y] = bullet;
  }
}
