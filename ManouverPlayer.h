#ifndef __ManouverPlayer_h__
#define __ManouverPlayer_h__

#include "manouver.h"

class ManouverPlayer
{
public:
  virtual void play(std::unique_ptr<RawManouver> pManouver) = 0;
  virtual void stop() = 0;
  virtual void run() = 0;
  virtual bool isRunning() = 0;
  virtual void delay(int ms) = 0;
  virtual ~ManouverPlayer() {}
};

#endif // __ManouverPlayer_h__
