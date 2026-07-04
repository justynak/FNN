#ifndef FNN_CORE_INTEGRATE_H
#define FNN_CORE_INTEGRATE_H

#include "core/point.h"

namespace fnn {

// Fixed-step integrators for dp/dt = deriv(p), generic over the system so
// their convergence order can be verified against ODEs with closed-form
// solutions (global error ~ dt for Euler, ~ dt^4 for RK4).

template <class Deriv>
inline Point3 euler_step(Point3 p, double dt, Deriv deriv)
{
    return p + dt * deriv(p);
}

template <class Deriv>
inline Point3 rk4_step(Point3 p, double dt, Deriv deriv)
{
    const Point3 k1 = deriv(p);
    const Point3 k2 = deriv(p + 0.5 * dt * k1);
    const Point3 k3 = deriv(p + 0.5 * dt * k2);
    const Point3 k4 = deriv(p + dt * k3);
    return p + (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}

} // namespace fnn

#endif // FNN_CORE_INTEGRATE_H
