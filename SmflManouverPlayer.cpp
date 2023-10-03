#include "SmflManouverPlayer.h"

static sf::Time timePerFrame = sf::seconds(1.f / 30.f);

SmflManouverPlayer::SmflManouverPlayer(sf::RenderWindow &window) : _window(window)
{
  _pointer.setRadius(2.f);
  _pointer.setOrigin(_pointer.getRadius(), _pointer.getRadius());
  setPointerVisibility(false);
  _window.draw(_pointer);
}

void SmflManouverPlayer::setPointerVisibility(bool isVisible)
{
  if (isVisible) {
    _pointer.setFillColor(sf::Color::Red);
  }
  else {
    _pointer.setFillColor(sf::Color::Transparent);
  }
}

bool SmflManouverPlayer::drawPointerAtNextPoint()
{
  if (_pManouver->isFinished())
  {
    setPointerVisibility(false);
    _pManouver.reset();
    return false;
  }
  Point nextPoint = _pManouver->getNextPoint();
  _pointer.setPosition(nextPoint.x, nextPoint.y);
  setPointerVisibility(true);
  _timer.restart();
  this->_timeTillNextUpdate = timePerFrame;
  return true;
}

void SmflManouverPlayer::play(std::unique_ptr<RawManouver> pManouver)
{
  _pManouver = std::move(pManouver);
  _isWaiting = false;
  drawPointerAtNextPoint();
}

void SmflManouverPlayer::stop()
{
  _pManouver.reset();
  setPointerVisibility(false);
}

void SmflManouverPlayer::run()
{
  if (_timer.getElapsedTime() > _timeTillNextUpdate)
  {
    if (_pManouver) {
      drawPointerAtNextPoint();
    }
    if (_isWaiting)
    {
      _isWaiting = false;
    }
  }
  sf::Event event;
  if (_window.pollEvent(event))
  {
    if (event.type == sf::Event::Closed)
      _window.close();
      return;
  } if (_window.isOpen()) {
    _window.display();
  }
}

void SmflManouverPlayer::delay(int ms)
{
  _isWaiting = true;
  _timeTillNextUpdate = sf::milliseconds(ms);
}

bool SmflManouverPlayer::isRunning()
{
  return (_pManouver != nullptr || _isWaiting);
}
