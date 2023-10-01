#include <deque>
#include <vector>
// for unique_ptr
#include <memory>

struct Point
{
public:
  float x;
  float y;
  Point() {}
  Point(float x, float y)
  {
    this->x = x;
    this->y = y;
  }

  // arithmetic operators
  Point operator+(const Point &p) const
  {
    return Point(x + p.x, y + p.y);
  }
  Point operator-(const Point &p) const
  {
    return Point(x - p.x, y - p.y);
  }
  Point operator*(const float &f) const
  {
    return Point(x * f, y * f);
  }
  Point &operator+=(const Point &p)
  {
    x += p.x;
    y += p.y;
    return *this;
  }
  Point &operator-=(const Point &p)
  {
    x -= p.x;
    y -= p.y;
    return *this;
  }
  Point &operator*=(const float &f)
  {
    x *= f;
    y *= f;
    return *this;
  }
};

struct QuadPoint
{
public:
  Point p0;
  Point p1;
  Point p2;
  Point p3;
  QuadPoint() {}
  QuadPoint(Point p0, Point p1, Point p2, Point p3)
  {
    this->p0 = p0;
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
  }
};

class SplineManouver
{
public:
  virtual QuadPoint getNextQuadPoint() = 0;
  virtual bool isFinished() = 0;
  virtual void reset() {}
  virtual ~SplineManouver() {}
};

typedef std::deque<Point> Points;

extern void addPreStartPoint(Points &points);
extern void addPostEndPoint(Points &points);

class SplineManouverFromPoints : public SplineManouver
{
protected:
  std::unique_ptr<Points> _pPoints;
  Points::const_iterator _currentPoint;

public:
  SplineManouverFromPoints(std::unique_ptr<Points> pPoints);
  void reset() override;
  QuadPoint getNextQuadPoint() override;
  bool isFinished() override;
};

class RawManouver
{
public:
  virtual bool isFinished() = 0;
  virtual Point getNextPoint() = 0;
  virtual void reset() {}
  virtual ~RawManouver() {}
};

class RepeatRawManouver : public RawManouver
{
protected:
  std::unique_ptr<RawManouver> _pManouver;

public:
  RepeatRawManouver(std::unique_ptr<RawManouver> pManouver);
  bool isFinished() override;
  Point getNextPoint() override;
  void reset() override;
};

class TransformRawManouver : public RawManouver
{
protected:
  bool isFinished() override;
  Point getNextPoint() override;
  Point transformPoint(Point p);
  std::unique_ptr<RawManouver> _pManouver;
  float _scale;
  float _rotate;
  Point _translate;
  bool _flipH;
  bool _flipV;

public:
  TransformRawManouver(
    std::unique_ptr<RawManouver> pManouver,
    float scale,
    float rotate,
    Point translate = Point(0, 0),
    bool flipH = false,
    bool flipV = false
  );
};

typedef std::vector<std::unique_ptr<RawManouver> > Manouvers;

class RawManouverSequence : public RawManouver
{
protected:
  std::unique_ptr<Manouvers> _pManouvers;
  Manouvers::iterator _currentManouver;

public:
  RawManouverSequence(std::unique_ptr<Manouvers> pManouvers);
  bool isFinished() override;
  Point getNextPoint() override;
  void reset() override;
};

class PointwiseAddRawManouver : public RawManouver
{
protected:
  std::unique_ptr<Manouvers> _pManouvers;
  std::vector<Manouvers::iterator> _iterators;

public:
  PointwiseAddRawManouver(std::unique_ptr<Manouvers> pManouvers);
  bool isFinished() override;
  Point getNextPoint() override;
  void reset() override;
};

class RawManouverFromSplineManouver : public RawManouver
{
protected:
  std::unique_ptr<SplineManouver> _pSplineManouver;
  bool _SplineInputFinished = false;
  float _t = 0;
  QuadPoint _lastQuad;
  float _fractionPartOfT;
  float _rate;

  void advanceT();
  void partialReset();

public:
  void reset() override;
  RawManouverFromSplineManouver(
    std::unique_ptr<SplineManouver> pSplineManouver,
    float rate
  );
  bool isFinished() override;
  Point getNextPoint() override;
};

class linearRawManouver : public RawManouver
{
protected:
  Point _p1;
  float _t = 0;
  float _rate;
  Point _totalDelta;

  void advanceT();

public:
  linearRawManouver(Point p1, Point p2, int numberOfSteps);
  bool isFinished() override;
  Point getNextPoint() override;
};
