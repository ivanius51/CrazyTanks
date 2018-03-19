#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <sstream>

#define SSTR( x ) static_cast< std::ostringstream & >( std::ostringstream() << std::dec << x ).str()

#include <windows.h>

class GameObject
{
  public:
    GameObject();
    //virtual ~GameObject();

    virtual void Draw() = 0;
    virtual void DrawTo(HDC ahdc) = 0;
    virtual void DrawToBuffer() = 0;
    virtual void ClearBuffer() = 0;
    virtual void Update() = 0;

    bool IsEmpty = true;

    UINT UpdateDelay = 0;

  protected:
    UINT UpdateTime_;
};

class MapTile : public GameObject
{
  public:
    MapTile(int x, int y);

    bool IsMovable = false;
    bool IsWalkable = true;

    POINT GetPosition();
    void SetPosition(POINT val);
  protected:
    POINT Position_;
  private:

};

class Graphical : public MapTile
{
  public:
    Graphical(HDC ahdc, COLORREF aLineColor, int x, int y);
    ~Graphical();

    bool InAnimation = false;

    void DrawToBuffer();

    POINT Direction;
    POINT Offset;

    HBITMAP GethBitmap() { return hBitmap_; }
  protected:
    HBITMAP hBitmap_ = 0;
    POINT DrawDirection_;
    COLORREF LineColor;
    HDC hdc;
};

class Wall : public Graphical
{
  public:
    Wall(HDC ahdc, COLORREF aLineColor, int x, int y);
    void Draw() override;
    void DrawTo(HDC ahdc) override;
    //void DrawToBuffer(){;}
    void ClearBuffer(){;}
    void Update(){;}
};

class Tank;

class Bullet : public Graphical
{
  public:
    Bullet(HDC ahdc, COLORREF aLineColor, int x, int y, Tank* AShooter);
    void Draw() override;
    void DrawTo(HDC ahdc) override;
    //void DrawToBuffer() override;
    void ClearBuffer() override;
    void Update() override;
  private:
    Tank* Shooter;
};

class Tank : public Graphical
{
  public:
    Tank(HDC ahdc, COLORREF aLineColor, int x, int y);
    ~Tank();
    void Draw() override;
    void DrawTo(HDC ahdc) override;
    //void DrawToBuffer() override;
    void ClearBuffer() override;
    void Shoot();
    void Update();

    UINT ShootDelay;
  protected:
    UINT ShootTime_;
    Bullet* bullet;
};
#endif // GAMEOBJECT_H
