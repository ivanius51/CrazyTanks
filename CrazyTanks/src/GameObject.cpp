#include "GameObject.h"
//#include <GlobalVariables.h>
#include <cmath>
#include <Game.h>

template <typename T> int sgn(T val) {
    return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
}

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
POINT MapTile::GetPosition()
{
  return Position_;
}
void MapTile::SetPosition(POINT val)
{
  int MaxTiles = Game::instance().MaxTiles();
  if (!Game::instance().Tiles[val.x * MaxTiles + val.y])
  {
    Game::instance().Tiles[val.x * MaxTiles + val.y] = this;
    if (Game::instance().Tiles[Position_.x * MaxTiles + Position_.y])
      Game::instance().Tiles[Position_.x * MaxTiles + Position_.y] = nullptr;
    Position_ = val;
  }
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
void Graphical::DrawToBuffer()
{
  if (hBitmap_==0)
  {
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

Bullet::Bullet(HDC ahdc, COLORREF aLineColor, int x, int y, Tank* AShooter)
  :Graphical(ahdc,aLineColor, x, y)
{
  IsWalkable = true;
  IsMovable = true;
  Shooter = AShooter;
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
  //Rectangle(ahdc,14,10,17,22);
  int width = 3 / 2;
  int height = 12 / 2;

  Rectangle(ahdc,16 - width - (Direction.x * height),16 - width - (Direction.y * height),16 + width + (Direction.x * height),16 + width + (Direction.y * height));

  SelectObject(ahdc,penOld);
}
void Bullet::Draw()
{
  DrawToBuffer();
  int TileSize = Game::instance().TileSize();
  Game::instance().DrawBitmap(TileSize*Position_.x + Offset.x, TileSize*Position_.y + Offset.y, hBitmap_, true);
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
    if (((NewPosition.x>=0)&&(NewPosition.x<MaxTiles))&&((NewPosition.y>=0)&&(NewPosition.y<MaxTiles)))
    {
      MapTile* NextTile = Game::instance().Tiles[NewPosition.x * MaxTiles + NewPosition.y];
      Offset.x += Direction.x;
      Offset.y += Direction.y;
      if ((NextTile) || (NextTile && NextTile->IsWalkable))
      {
        //check and call self destroy
        //ClearBuffer();

        //bullet collision with tank
        if (NextTile && dynamic_cast<Tank*>(NextTile))
        {
          //Enemy bullet
          if (Shooter==Game::instance().Player && NextTile!=this)
          {
            //InAnimation = false;//if need stop bullet on enemy tank
            delete Game::instance().Tiles[NewPosition.x * MaxTiles + NewPosition.y];
            Game::instance().Tiles[NewPosition.x * MaxTiles + NewPosition.y] = nullptr;
            Game::instance().Score++;
          }
          else
          //Player bullet
          if (NextTile==Game::instance().Player &&
              //check player is outway animation
              !(((Game::instance().Player->Direction.x==Direction.y)||(Game::instance().Player->Direction.y==Direction.x)) &&
                Game::instance().Player->InAnimation && ((abs(Game::instance().Player->Offset.x)+abs(Game::instance().Player->Offset.y))>TileSize/3)))
          {
            InAnimation = false;
            Game::instance().Lives --;
            if (Game::instance().Lives<=0)
              Game::instance().stop();
          }
        }
        else
        //bullet collision with other
          InAnimation = false;
      }
    }
    else
      InAnimation = false;
  }
}

Tank::~Tank()
{
  if (bullet)
    delete bullet;
}
Tank::Tank(HDC ahdc, COLORREF aLineColor, int x, int y)
  :Graphical(ahdc,aLineColor, x, y)
{
  IsWalkable = false;
  IsMovable = true;
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

  //Draw in Direction, but magic numbers still here
  DrawDirection_ = Direction;

  int width = 6;
  int height = 12;
  Rectangle(ahdc,16 - width*((Direction.x < 0) ? 1 : 2),16 - width*((Direction.y < 0) ? 1 : 2),16 + width*((Direction.x > 0) ? 1 : 2),16 + width*((Direction.y > 0) ? 1 : 2));
  /*//base
  //0,-1
  Rectangle(ahdc,4,10,28,28);
  //0,1
  Rectangle(ahdc,4,4,28,20);
  //left
  MoveToEx(ahdc,8,10,nullptr);
  LineTo(ahdc,8,28);
  //right
  MoveToEx(ahdc,23,10,nullptr);
  LineTo(ahdc,23,28);
  //top hole
  Arc(ahdc,11,15,20,24,11,15,11,15);*/
  //weapon
  width = 2;
  if (Direction.x == 0)
    Rectangle(ahdc,14,16 - height*((Direction.y < 0) ? 0 : -1),17,16 + height*((Direction.y > 0) ? 0 : -1));
  else
    Rectangle(ahdc,16 - height*((Direction.x < 0) ? 0 : -1),14,16 + height*((Direction.x > 0) ? 0 : -1),17);

  SelectObject(ahdc,penOld);
}
void Tank::Draw()
{
  DrawToBuffer();
  int TileSize = Game::instance().TileSize();
  Game::instance().DrawBitmap(hdc,TileSize*Position_.x + Offset.x, TileSize*Position_.y + Offset.y, hBitmap_, true);
  if (bullet)
    bullet->Draw();
}

void Tank::Update()
{
  if (DrawDirection_.x!=Direction.x || DrawDirection_.y!=Direction.y)
    ClearBuffer();

  if (this != Game::instance().Player)
  {
    POINT Position = Game::instance().Player->GetPosition();
    if ((Position.x==Position_.x)||(Position.y==Position_.y))
    {
      Direction.x = sgn(Position.x-Position_.x);
      Direction.y = sgn(Position.y-Position_.y);
      this->Shoot();
    }
  }

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
      InAnimation = false;
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
        InAnimation = false;
        Offset.x = 1;
        Offset.y = 1;
      }
    }
  }
  if (bullet)
  {
    bullet->Update();
    if (!bullet->InAnimation)
    {
      delete bullet;
      bullet = nullptr;
    }
  }
}

void Tank::Shoot()
{
  int TileSize = Game::instance().TileSize();
  int MaxTiles = Game::instance().MaxTiles();
  if (bullet && !bullet->InAnimation)
  {
    delete bullet;
  }
  if (!bullet || !bullet->InAnimation)
  {
    bullet = new Bullet(hdc,LineColor,GetPosition().x+Direction.x,GetPosition().y+Direction.y, this);
    bullet->Offset.y = -Direction.y*(TileSize/2);
    bullet->Offset.x = -Direction.x*(TileSize/2);
    bullet->Direction = Direction;
    bullet->InAnimation = true;
  }
}
