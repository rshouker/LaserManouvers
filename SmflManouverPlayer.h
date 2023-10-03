#ifndef __SmflManouverPlayer_h__
#define __SmflManouverPlayer_h__

#include <SFML/Graphics.hpp>
#include "ManouverPlayer.h"

class SmflManouverPlayer : public ManouverPlayer
{
protected:
  sf::RenderWindow &_window;
  sf::Clock _timer;
  sf::Time _timeTillNextUpdate;
  sf::CircleShape _pointer;
  bool _isWaiting = true;
  std::unique_ptr<RawManouver> _pManouver;
  bool drawPointerAtNextPoint();
  void setPointerVisibility(bool isVisible);
public:
  SmflManouverPlayer(sf::RenderWindow &window);
  void play(std::unique_ptr<RawManouver> pManouver) override;
  void stop() override;
  void run() override;
  bool isRunning() override;
  void delay(int ms) override;
};

#endif // __SmflManouverPlayer_h__
