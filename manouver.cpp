#include "manouver.h"
#include <cassert>
#include <cmath>

// Catmull-Rom Spline interpolation between p1 and p2 for previous point p0 and next point p3
static Point spline(const QuadPoint &quad, float t)
{
  float t2 = t * t;
  float t3 = t2 * t;

  float b1 = .5 * (-t3 + 2 * t2 - t);
  float b2 = .5 * (3 * t3 - 5 * t2 + 2);
  float b3 = .5 * (-3 * t3 + 4 * t2 + t);
  float b4 = .5 * (t3 - t2);

  return quad.p0 * b1 + quad.p1 * b2 + quad.p2 * b3 + quad.p3 * b4;
}

void addPreStartPoint(Points &points)
{
  assert(points.size() > 1);
  Point p0 = points.front();
  Point p1 = points.at(1);
  Point prevP = p0 - (p1 - p0);
  points.push_front(prevP);
}

void addPostEndPoint(Points &points)
{
  assert(points.size() > 1);
  Point p0 = points.back();
  Point p1 = points.at(points.size() - 2);
  Point postP = p0 - (p1 - p0);
  points.push_back(postP);
}

SplineManouverFromPoints::SplineManouverFromPoints(Points &points) : _points(points)
{
  _currentPoint = _points.begin();
}

void SplineManouverFromPoints::reset()
{
  _currentPoint = _points.begin();
}

bool SplineManouverFromPoints::isFinished()
{
  return _currentPoint + 4 == _points.end();
}

QuadPoint SplineManouverFromPoints::getNextQuadPoint()
{
  QuadPoint quad = QuadPoint(*_currentPoint, *(_currentPoint + 1), *(_currentPoint + 2), *(_currentPoint + 3));
  _currentPoint++;
  return quad;
}

void RepeatRawManouver::reset()
{
  _manouver.reset();
}

bool RepeatRawManouver::isFinished()
{
  return false;
}

Point RepeatRawManouver::getNextPoint()
{
  if (_manouver.isFinished())
  {
    _manouver.reset();
  }
  return _manouver.getNextPoint();
}

TransformRawManouver::TransformRawManouver(RawManouver &manouver, float scale, float rotate, Point translate, bool flipH, bool flipV) : _manouver(manouver), _scale(scale), _rotate(rotate), _translate(translate), _flipH(flipH), _flipV(flipV) {}

Point TransformRawManouver::transformPoint(Point p)
{
  if (_flipH)
  {
    p.x = -p.x;
  }
  if (_flipV)
  {
    p.y = -p.y;
  }
  float x = p.x * _scale;
  float y = p.y * _scale;
  float x1 = x * cos(_rotate) - y * sin(_rotate);
  float y1 = x * sin(_rotate) + y * cos(_rotate);
  return Point(x1 + _translate.x, y1 + _translate.y);
}

bool TransformRawManouver::isFinished()
{
  return _manouver.isFinished();
}

Point TransformRawManouver::getNextPoint()
{
  Point point = _manouver.getNextPoint();
  return transformPoint(point);
}

bool RawManouverSequence::isFinished()
{
  return _currentManouver == _manouvers.end();
}

Point RawManouverSequence::getNextPoint()
{
  if (_currentManouver == _manouvers.end())
  {
    return Point();
  }
  Point point = (*_currentManouver)->getNextPoint();
  if ((*_currentManouver)->isFinished())
  {
    _currentManouver++;
  }
  return point;
}

void RawManouverSequence::reset()
{
  _currentManouver = _manouvers.begin();
  for (Manouvers::iterator it = _manouvers.begin(); it != _manouvers.end(); it++)
  {
    (*it)->reset();
  }
}

PointwiseAddRawManouver::PointwiseAddRawManouver(Manouvers &manouvers) : _manouvers(manouvers) {};

bool PointwiseAddRawManouver::isFinished()
{
  for (Manouvers::iterator it = _manouvers.begin(); it != _manouvers.end(); it++)
  {
    if ((*it)->isFinished())
    {
      return true;
    }
  }
}

Point PointwiseAddRawManouver::getNextPoint()
{
  Point sum = Point(0, 0);
  for (Manouvers::iterator it = _manouvers.begin(); it != _manouvers.end(); it++)
  {
    sum += (*it)->getNextPoint();
  }
  return sum;
}

void PointwiseAddRawManouver::reset()
{
  for (Manouvers::iterator it = _manouvers.begin(); it != _manouvers.end(); it++)
  {
    (*it)->reset();
  }
}

constexpr float epsilon = 0.0001;

void RawManouverFromSplineManouver::advanceT()
{
  float oldT = _t;
  _t += _rate;
  if (floor(_t + epsilon) != floor(oldT + epsilon))
  {
    if (_splineManouver.isFinished())
    {
      _SplineInputFinished = true;
    }
    else
    {
      _lastQuad = _splineManouver.getNextQuadPoint();
    }
  }
  _fractionPartOfT = _t - floor(_t + epsilon);
}

void RawManouverFromSplineManouver::partialReset()
{
  _SplineInputFinished = false;
  _t = 0;
  _lastQuad = _splineManouver.getNextQuadPoint();
  _fractionPartOfT = 0;
}

void RawManouverFromSplineManouver::reset()
{
  _splineManouver.reset();
  partialReset();
}

RawManouverFromSplineManouver::RawManouverFromSplineManouver(SplineManouver &splineManouver, float rate) : _splineManouver(splineManouver), _rate(rate)
{
  partialReset();
}

bool RawManouverFromSplineManouver::isFinished()
{
  if (!_SplineInputFinished)
  {
    return false;
  }
  return _fractionPartOfT + epsilon > 1.0 + _rate / 2.0;
}

Point RawManouverFromSplineManouver::getNextPoint()
{
  advanceT();
  return spline(_lastQuad, _fractionPartOfT);
}

void linearRawManouver::advanceT()
{
  float oldT = _t;
  _t += _rate;
}

linearRawManouver::linearRawManouver(Point p1, Point p2, int numberOfSteps)
    : _p1(p1)
{
  _rate = 1.0 / numberOfSteps;
  _totalDelta = p2 - p1;
}

bool linearRawManouver::isFinished()
{
  return _t + epsilon > 1.0;
}

Point linearRawManouver::getNextPoint()
{
  advanceT();
  return _p1 + _totalDelta * _t;
}
