#ifndef COMMAND_H
#define COMMAND_H

#include <GameObject.h>

class Command
{
  public:
    virtual ~Command();
    virtual void execute() = 0;
    virtual void execute(GameObject& actor) = 0;
  protected:

  private:
};

class FireCommand : public Command
{
  public:
      virtual void execute() {  };
};

class MoveForward : public Command
{
  public:
      virtual void execute(GameObject& actor)
      {
        //actor.MoveForward();
      }
};


#endif // COMMAND_H
