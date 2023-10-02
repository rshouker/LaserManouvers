#ifndef __Shapes_h__
#define __Shapes_h__

#include "manouver.h"

std::unique_ptr<RawManouver> createManouverFromPoints(
  int ticks,
  std::unique_ptr<Points> pPoints,
  bool addPreStartPoint,
  bool addPostEndPoint
);

std::unique_ptr<RawManouver> transformManouver(
  std::unique_ptr<RawManouver> pManouver,
  float scale,
  float rotate,
  Point translate = Point(0, 0),
  bool flipH = false,
  bool flipV = false
);

std::unique_ptr<RawManouver> repeatManouver(std::unique_ptr<RawManouver> pManouver);

std::unique_ptr<RawManouver> concatManouvers(
  std::unique_ptr<RawManouver> pManouver1,
  std::unique_ptr<RawManouver> pManouver2
);

std::unique_ptr<RawManouver> concatManouvers(
  std::unique_ptr<Manouvers> pManouvers
);

std::unique_ptr<RawManouver> pointwiseAddManouvers(
  std::unique_ptr<RawManouver> pManouver1,
  std::unique_ptr<RawManouver> pManouver2
);

std::unique_ptr<RawManouver> pointwiseAddManouvers(
    std::unique_ptr<Manouvers> pManouvers
);

std::unique_ptr<RawManouver> createLineManouver(int ticks, Point& startingPoint, Point& endingPoint);
std::unique_ptr<RawManouver> createCircleManouver(int ticks);
std::unique_ptr<RawManouver> createHeartManouver(int ticks);
std::unique_ptr<RawManouver> createArcManouver(
  int ticks,
  Point startingPoint,
  Point endingPoint,
  float curvature
);

/*
class CircleManouver : public RawManouverFromSplineManouver
{
public:
  CircleManouver(int ticks);
};

class HeartManouver : public RawManouverSequence
{
  Points _leftSide;
  RawManouver *_leftSideManouver;
  RawManouver *_rightSideManouver;
public:
  HeartManouver(int ticks);
};

class ArcManouver : public RawManouverFromSplineManouver
{
public:
  ArcManouver(int ticks, Point startingPoint, Point endingPoint, float curvature);
};
*/

#endif // __Shapes_h__
