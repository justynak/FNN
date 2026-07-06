#ifndef FNN_CORE_HENON_H
#define FNN_CORE_HENON_H

#include <vector>

#include "core/point.h"

namespace fnn {

// Hénon map:  x' = 1 - a*x^2 + y,  y' = b*x
// Defaults are the canonical chaotic parameters.
struct HenonParams {
    double a = 1.4;
    double b = 0.3;
};

inline Point2 henon_step(const Point2 p, const HenonParams &prm = {})
{
    return {1.0 - prm.a * p.x * p.x + p.y,
            prm.b * p.x};
}

// Exact analytic inverse (the map is invertible for b != 0).
inline Point2 henon_inverse_step(const Point2 p, const HenonParams &prm = {})
{
    const double x = p.y / prm.b;
    return {x, p.x - 1.0 + prm.a * x * x};
}

// Iterates the map `count` times from `init`; the returned orbit holds the
// iterates only (init itself is not included).
inline std::vector<Point2> henon_orbit(const Point2 init, const int count,
                                       const HenonParams &prm = {})
{
    std::vector<Point2> orbit;
    orbit.reserve(count);
    Point2 p = init;
    for (int i = 0; i < count; ++i) {
        p = henon_step(p, prm);
        orbit.push_back(p);
    }
    return orbit;
}

} // namespace fnn

#endif // FNN_CORE_HENON_H
