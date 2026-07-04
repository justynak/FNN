#ifndef FNN_CORE_MUTUAL_INFORMATION_H
#define FNN_CORE_MUTUAL_INFORMATION_H

#include <algorithm>
#include <cmath>
#include <vector>

namespace fnn {

// Average mutual information (in bits) between a signal and its copy lagged
// by `lag` samples, estimated with a uniform histogram of `bins` bins over
// the signal's [min, max] range (Fraser & Swinney).
//
// The marginal distributions are taken from the (x_t, x_{t+lag}) pairs, not
// from the whole signal, so at lag 0 the result equals the entropy of the
// binned signal exactly. A constant, empty or too-short signal has zero
// entropy and yields 0.
inline double average_mutual_information(const std::vector<double> &signal,
                                         int lag, int bins)
{
    const int pairs = int(signal.size()) - lag;
    if (lag < 0 || bins <= 0 || pairs <= 0)
        return 0.0;

    const auto [lo_it, hi_it] = std::minmax_element(signal.begin(), signal.end());
    const double lo = *lo_it, hi = *hi_it;
    if (hi == lo)
        return 0.0;

    const auto bin_of = [&](double v) {
        const int i = int((v - lo) / (hi - lo) * bins);
        return i < bins ? i : bins - 1; // v == hi lands in the top bin
    };

    std::vector<long> joint(size_t(bins) * bins, 0);
    std::vector<long> px(bins, 0), py(bins, 0);
    for (int t = 0; t < pairs; ++t) {
        const int i = bin_of(signal[t]);
        const int j = bin_of(signal[t + lag]);
        ++joint[size_t(i) * bins + j];
        ++px[i];
        ++py[j];
    }

    // I = sum p(i,j) * log2( p(i,j) / (p(i)*p(j)) ), zero-count terms skipped;
    // with counts c and pair total n the log argument is c*n / (px*py).
    const double n = pairs;
    double mi = 0.0;
    for (int i = 0; i < bins; ++i)
        for (int j = 0; j < bins; ++j) {
            const long c = joint[size_t(i) * bins + j];
            if (c == 0)
                continue;
            mi += (c / n) * std::log2(c * n / (double(px[i]) * double(py[j])));
        }
    return mi;
}

// AMI for every lag 0..max_lag; index into the result with the lag.
inline std::vector<double> ami_curve(const std::vector<double> &signal,
                                     int max_lag, int bins)
{
    std::vector<double> curve;
    curve.reserve(max_lag + 1);
    for (int lag = 0; lag <= max_lag; ++lag)
        curve.push_back(average_mutual_information(signal, lag, bins));
    return curve;
}

// Index of the first strict local minimum (v[i] < both neighbours), or -1 if
// there is none. Applied to an AMI curve this is the standard choice of the
// embedding delay tau.
inline int first_local_minimum(const std::vector<double> &v)
{
    for (int i = 1; i + 1 < int(v.size()); ++i)
        if (v[i] < v[i - 1] && v[i] < v[i + 1])
            return i;
    return -1;
}

} // namespace fnn

#endif // FNN_CORE_MUTUAL_INFORMATION_H
