#ifndef FNN_CORE_LOGISTIC_H
#define FNN_CORE_LOGISTIC_H

#include <vector>

namespace fnn {

// Logistic map:  x' = r * x * (1 - x). For r in [0,4] the unit interval
// maps into itself; the period-doubling cascade into chaos happens as
// r -> 4.
inline double logistic_step(const double x, const double r)
{
    return r * x * (1.0 - x);
}

// Iterates the map `count` times from `x0`; the returned orbit holds the
// iterates only (x0 itself is not included).
inline std::vector<double> logistic_orbit(const double x0, const double r, const int count)
{
    std::vector<double> orbit;
    orbit.reserve(count);
    double x = x0;
    for (int i = 0; i < count; ++i) {
        x = logistic_step(x, r);
        orbit.push_back(x);
    }
    return orbit;
}

} // namespace fnn

#endif // FNN_CORE_LOGISTIC_H
