#ifndef COMMAND_H
#define COMMAND_H

#include <GameObject.h>

class Command
{
  public:
    virtual ~Command();
    virtual void execute() = 0;
    virtual void execute(Tank& tank) = 0;
  protected:

  private:
};

class FireCommand : public Command
{
  public:
      virtual void execute(Tank& tank)
      {
        tank.Shoot();
      };
};

class MoveForward : public Command
{
  public:
      virtual void execute(Graphical& actor)
      {
        if (!actor.InAnimation)
          actor.InAnimation = true;
      }
};


#endif // COMMAND_H
