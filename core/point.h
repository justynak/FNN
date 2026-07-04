#ifndef FNN_CORE_POINT_H
#define FNN_CORE_POINT_H

namespace fnn {

struct Point2 {
    double x, y;
};

struct Point3 {
    double x, y, z;
};

inline Point3 operator+(Point3 a, Point3 b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Point3 operator*(double s, Point3 p)
{
    return {s * p.x, s * p.y, s * p.z};
}

} // namespace fnn

#endif // FNN_CORE_POINT_H
