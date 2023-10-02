#include "shapes.h"
#include <cassert>
#include <cmath>

using namespace std;

unique_ptr<RawManouver> createManouverFromPoints(
  int ticks,
  unique_ptr<Points> pPoints,
  bool addPreStartPoint,
  bool addPostEndPoint
)
{
  if (addPreStartPoint)
  {
    ::addPreStartPoint(*pPoints);
  }
  if (addPostEndPoint)
  {
    ::addPostEndPoint(*pPoints);
  }
  float rate = ticks / (float)(pPoints->size() - 3);
  unique_ptr<SplineManouver> pSplineManouver = make_unique<SplineManouverFromPoints>(move(pPoints));
  unique_ptr<RawManouver> pRawManouver = make_unique<RawManouverFromSplineManouver>(rate, move(pSplineManouver));
  return pRawManouver;
}

unique_ptr<RawManouver> transformManouver(
  unique_ptr<RawManouver> pManouver,
  float scale,
  float rotate,
  Point translate,
  bool flipH,
  bool flipV
)
{
  unique_ptr<RawManouver> pTransformedManouver = make_unique<TransformRawManouver>(
    move(pManouver),
    scale,
    rotate,
    translate,
    flipH,
    flipV);
  return pTransformedManouver;
}

unique_ptr<RawManouver> repeatManouver(unique_ptr<RawManouver> pManouver)
{
  unique_ptr<RawManouver> pRepeatedManouver = make_unique<RepeatRawManouver>(move(pManouver));
  return pRepeatedManouver;
}

unique_ptr<RawManouver> concatManouvers(
  unique_ptr<RawManouver> pManouver1,
  unique_ptr<RawManouver> pManouver2
)
{
  unique_ptr<Manouvers> pManouvers = make_unique<Manouvers>();
  pManouvers->push_back(move(pManouver1));
  pManouvers->push_back(move(pManouver2));
  return concatManouvers(move(pManouvers));
}

unique_ptr<RawManouver> concatManouvers(unique_ptr<Manouvers> pManouvers)
{
  unique_ptr<RawManouver> pConcatenatedManouver = make_unique<RawManouverSequence>(move(pManouvers));
  return pConcatenatedManouver;
}

unique_ptr<RawManouver> pointwiseAddManouvers(
  unique_ptr<RawManouver> pManouver1,
  unique_ptr<RawManouver> pManouver2
)
{
  unique_ptr<Manouvers> pManouvers = make_unique<Manouvers>();
  pManouvers->push_back(move(pManouver1));
  pManouvers->push_back(move(pManouver2));
  return pointwiseAddManouvers(move(pManouvers));
}

unique_ptr<RawManouver> pointwiseAddManouvers(unique_ptr<Manouvers> pManouvers)
{
  unique_ptr<RawManouver> pPointwiseAddedManouver = make_unique<PointwiseAddRawManouver>(move(pManouvers));
  return pPointwiseAddedManouver;
}

unique_ptr<RawManouver> createLineManouver(int ticks, Point& p1, Point& p2)
{
  return make_unique<linearRawManouver>(ticks, p1, p2);
}

unique_ptr<RawManouver> createCircleManouver(int ticks)
{
  unique_ptr<Points> pPoints = make_unique<Points>();
  pPoints->push_back(Point(1, 0));
  pPoints->push_back(Point(0, 1));
  pPoints->push_back(Point(-1, 0));
  pPoints->push_back(Point(0, -1));
  return createManouverFromPoints(ticks, move(pPoints), true, true);
}

unique_ptr<RawManouver> createHeartManouver(int ticks)
{
  unique_ptr<Points> pLeftSidePoints = make_unique<Points>();
  pLeftSidePoints->push_back(Point(0, -1));
  pLeftSidePoints->push_back(Point(-0.5, -0.5));
  pLeftSidePoints->push_back(Point(-1, 0));
  pLeftSidePoints->push_back(Point(-0.5, 0.5));
  pLeftSidePoints->push_back(Point(0, 0));
  unique_ptr<Points> pRightSidePoints = make_unique<Points>();
  // make it a reverse copy of the left side
  for (Points::reverse_iterator it = pLeftSidePoints->rbegin(); it != pLeftSidePoints->rend(); it++)
  {
    pRightSidePoints->push_back(*it);
  }

  unique_ptr<RawManouver> pLeftSideManouver = createManouverFromPoints(ticks/2, move(pLeftSidePoints), true, true);
  unique_ptr<RawManouver> pMirroredRightSideManouver = createManouverFromPoints(ticks/2, move(pRightSidePoints), true, true);
  unique_ptr<RawManouver> pRightSideManouver = transformManouver(move(pMirroredRightSideManouver), 1, 0, Point(0, 0), true, false);
  return concatManouvers(move(pLeftSideManouver), move(pRightSideManouver));
}

std::unique_ptr<RawManouver> createArcManouver(
  int ticks,
  Point startingPoint,
  Point endingPoint,
  float curvature
)
{
  unique_ptr<Points> pPoints = make_unique<Points>();
  Point linearMidPoint = (startingPoint + endingPoint) * 0.5;
  // float radius = 1 / curvature;
  Point PerpendicularAtMidPoint = Point(-linearMidPoint.y, linearMidPoint.x);
  Point midPoint = linearMidPoint + PerpendicularAtMidPoint * curvature;

  pPoints->push_back(startingPoint);
  pPoints->push_back(midPoint);
  pPoints->push_back(endingPoint);
  return createManouverFromPoints(ticks, move(pPoints), true, true);
}
