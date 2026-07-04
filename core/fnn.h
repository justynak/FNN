#ifndef FNN_CORE_FNN_H
#define FNN_CORE_FNN_H

#include <cmath>
#include <limits>
#include <vector>

#include "core/embedding.h"

namespace fnn {

// Thresholds of the Kennel-Brown-Abarbanel false-nearest-neighbors criteria
// (the defaults are the values from the original 1992 paper).
struct FnnParams {
    double r_tol = 15.0; // ratio test: extension distance vs neighbor distance
    double a_tol = 2.0;  // size test: extended distance vs attractor size
};

// Fraction of points in the `dim`-dimensional delay embedding whose nearest
// neighbor is false, i.e. flies apart when the embedding is extended to
// dim+1. A point is false if either criterion fires:
//   1. |x_{i+dim*lag} - x_{nn+dim*lag}| / R_dim > r_tol
//   2. R_{dim+1} / attractor_size > a_tol   (keeps noise from ever embedding)
// Only points whose (dim+1)-th coordinate exists participate. Neighbor
// search is brute force O(N^2) — fine for the ~10^3..10^4 points used here.
inline double false_neighbor_fraction(const std::vector<double> &signal,
                                      int dim, int lag,
                                      const FnnParams &prm = {})
{
    const int count = delay_vector_count(int(signal.size()), dim + 1, lag);
    if (count < 2)
        return 0.0;

    // Attractor size: standard deviation of the signal.
    double mean = 0.0;
    for (const double v : signal)
        mean += v;
    mean /= signal.size();
    double var = 0.0;
    for (const double v : signal)
        var += (v - mean) * (v - mean);
    const double attractor_size = std::sqrt(var / signal.size());
    if (attractor_size == 0.0)
        return 0.0; // constant signal: every neighbor is a true neighbor

    int false_count = 0;
    for (int i = 0; i < count; ++i) {
        int nn = -1;
        double best = std::numeric_limits<double>::max();
        for (int j = 0; j < count; ++j) {
            if (j == i)
                continue;
            double d2 = 0.0;
            for (int k = 0; k < dim && d2 < best; ++k) {
                const double diff = signal[i + k * lag] - signal[j + k * lag];
                d2 += diff * diff;
            }
            if (d2 < best) {
                best = d2;
                nn = j;
            }
        }

        const double r = std::sqrt(best);
        const double ext = std::fabs(signal[i + dim * lag] - signal[nn + dim * lag]);
        // r == 0 means duplicate points (e.g. exactly periodic signals):
        // the ratio is undefined, so any separation at all marks it false.
        const bool ratio_false = r > 0.0 ? ext / r > prm.r_tol : ext > 0.0;
        const bool size_false = std::sqrt(r * r + ext * ext) / attractor_size > prm.a_tol;
        if (ratio_false || size_false)
            ++false_count;
    }
    return double(false_count) / count;
}

// False-neighbor fraction for every dimension 1..max_dim; element [d-1]
// holds the fraction at dimension d. The embedding dimension m is the
// smallest d where the fraction drops to ~0.
inline std::vector<double> fnn_curve(const std::vector<double> &signal,
                                     int max_dim, int lag,
                                     const FnnParams &prm = {})
{
    std::vector<double> curve;
    curve.reserve(max_dim);
    for (int dim = 1; dim <= max_dim; ++dim)
        curve.push_back(false_neighbor_fraction(signal, dim, lag, prm));
    return curve;
}

} // namespace fnn

#endif // FNN_CORE_FNN_H
