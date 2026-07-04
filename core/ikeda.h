#ifndef FNN_CORE_IKEDA_H
#define FNN_CORE_IKEDA_H

#include <cmath>
#include <vector>

#include "core/point.h"

namespace fnn {

// Ikeda map:  z = c - d / (1 + x^2 + y^2)
//             x' = a + b*(x*cos z - y*sin z)
//             y' =     b*(x*sin z + y*cos z)
// i.e. rotate (x, y) by the state-dependent angle z, scale by b, shift by a.
// Since the rotation preserves the norm, r' <= a + b*r, so for b < 1 every
// orbit ends up inside the ball of radius a / (1 - b).
// Defaults are the parameters used by the FNN app's attractor plot.
struct IkedaParams {
    double a = 1.0;
    double b = 0.9;
    double c = 0.1;
    double d = 8.0;
};

inline double ikeda_phase(Point2 p, const IkedaParams &prm = {})
{
    return prm.c - prm.d / (p.x * p.x + p.y * p.y + 1.0);
}

inline Point2 ikeda_step(Point2 p, const IkedaParams &prm = {})
{
    const double z = ikeda_phase(p, prm);
    return {prm.a + prm.b * (p.x * std::cos(z) - p.y * std::sin(z)),
            prm.b * (p.x * std::sin(z) + p.y * std::cos(z))};
}

// Iterates the map `count` times from `init`; the returned orbit holds the
// iterates only (init itself is not included).
inline std::vector<Point2> ikeda_orbit(Point2 init, int count,
                                       const IkedaParams &prm = {})
{
    std::vector<Point2> orbit;
    orbit.reserve(count);
    Point2 p = init;
    for (int i = 0; i < count; ++i) {
        p = ikeda_step(p, prm);
        orbit.push_back(p);
    }
    return orbit;
}

} // namespace fnn

#endif // FNN_CORE_IKEDA_H
