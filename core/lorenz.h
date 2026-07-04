#ifndef FNN_CORE_LORENZ_H
#define FNN_CORE_LORENZ_H

#include <vector>

#include "core/point.h"

namespace fnn {

// Lorenz system:  dx/dt = sigma*(y - x)
//                 dy/dt = x*(rho - z) - y
//                 dz/dt = x*y - beta*z
// Defaults are the classic chaotic parameters.
struct LorenzParams {
    double sigma = 10.0;
    double rho = 28.0;
    double beta = 8.0 / 3.0;
};

inline Point3 lorenz_deriv(Point3 p, const LorenzParams &prm = {})
{
    return {prm.sigma * (p.y - p.x),
            p.x * (prm.rho - p.z) - p.y,
            p.x * p.y - prm.beta * p.z};
}

inline Point3 lorenz_euler_step(Point3 p, double dt, const LorenzParams &prm = {})
{
    const Point3 d = lorenz_deriv(p, prm);
    return {p.x + d.x * dt, p.y + d.y * dt, p.z + d.z * dt};
}

// Integrates with forward Euler; good enough for plotting the attractor.
// The returned orbit holds the integration steps only (init is not included).
inline std::vector<Point3> lorenz_orbit(Point3 init, double dt, int count,
                                        const LorenzParams &prm = {})
{
    std::vector<Point3> orbit;
    orbit.reserve(count);
    Point3 p = init;
    for (int i = 0; i < count; ++i) {
        p = lorenz_euler_step(p, dt, prm);
        orbit.push_back(p);
    }
    return orbit;
}

} // namespace fnn

#endif // FNN_CORE_LORENZ_H
