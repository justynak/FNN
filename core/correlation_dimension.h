#ifndef FNN_CORE_CORRELATION_DIMENSION_H
#define FNN_CORE_CORRELATION_DIMENSION_H

#include <algorithm>
#include <cmath>
#include <vector>

namespace fnn {

// Grassberger-Procaccia correlation integral: C(r) is the fraction of
// distinct point pairs closer than r in the max-norm. Pairs closer than
// `theiler` in time are excluded (Theiler window), so temporal correlation
// along a trajectory doesn't masquerade as geometric structure. A negative
// window is treated as 0.
//
// One O(N^2) pass serves all radii, in any order: each pair bumps the
// count of every radius that covers it.
inline std::vector<double> correlation_integrals(
    const std::vector<std::vector<double>> &points,
    const std::vector<double> &radii, const int theiler = 0)
{
    const int n = int(points.size());
    const int window = std::max(theiler, 0);
    std::vector<long long> counts(radii.size(), 0);
    long long pairs = 0;
    for (int i = 0; i < n; ++i)
        for (int j = i + 1 + window; j < n; ++j) {
            double d = 0.0;
            for (size_t k = 0; k < points[i].size(); ++k)
                d = std::max(d, std::fabs(points[i][k] - points[j][k]));
            ++pairs;
            for (size_t r = 0; r < radii.size(); ++r)
                if (d < radii[r])
                    ++counts[r];
        }

    std::vector<double> c(radii.size(), 0.0);
    for (size_t r = 0; r < radii.size(); ++r)
        c[r] = pairs > 0 ? double(counts[r]) / double(pairs) : 0.0;
    return c;
}

// D2 estimate: least-squares slope of log C(r) vs log r over `n_radii`
// log-spaced radii in [r_min, r_max]. Choosing the scaling region is a
// judgment call that depends on data size and noise level, so it is the
// caller's explicit input — this function does not try to guess it.
// Radii with no pairs yet are skipped; returns 0 if fewer than two radii
// have data or the arguments are degenerate (n_radii < 2, r_min <= 0, or
// r_max <= r_min).
inline double correlation_dimension(
    const std::vector<std::vector<double>> &points,
    const double r_min, const double r_max, const int n_radii = 12, const int theiler = 0)
{
    if (n_radii < 2 || r_min <= 0.0 || r_max <= r_min)
        return 0.0;

    std::vector<double> radii(n_radii);
    const double step = std::log(r_max / r_min) / (n_radii - 1);
    for (int i = 0; i < n_radii; ++i)
        radii[i] = r_min * std::exp(i * step);

    const auto c = correlation_integrals(points, radii, theiler);

    double sx = 0.0, sy = 0.0, sxx = 0.0, sxy = 0.0;
    int used = 0;
    for (int i = 0; i < n_radii; ++i) {
        if (c[i] <= 0.0)
            continue;
        const double lx = std::log(radii[i]);
        const double ly = std::log(c[i]);
        sx += lx;
        sy += ly;
        sxx += lx * lx;
        sxy += lx * ly;
        ++used;
    }
    if (used < 2)
        return 0.0;
    return (used * sxy - sx * sy) / (used * sxx - sx * sx);
}

} // namespace fnn

#endif // FNN_CORE_CORRELATION_DIMENSION_H
