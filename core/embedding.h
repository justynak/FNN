#ifndef FNN_CORE_EMBEDDING_H
#define FNN_CORE_EMBEDDING_H

#include <vector>

namespace fnn {

// Number of delay vectors of dimension `dim` with delay `lag` that fit in a
// signal of `n` samples (0 if none fit).
inline int delay_vector_count(int n, int dim, int lag)
{
    const int count = n - (dim - 1) * lag;
    return count > 0 ? count : 0;
}

// Takens delay embedding: reconstructs `dim`-dimensional state vectors from
// a scalar signal, y_i = (x_i, x_{i+lag}, ..., x_{i+(dim-1)*lag}).
inline std::vector<std::vector<double>>
delay_embedding(const std::vector<double> &signal, int dim, int lag)
{
    const int count = delay_vector_count(int(signal.size()), dim, lag);
    std::vector<std::vector<double>> vectors;
    vectors.reserve(count);
    for (int i = 0; i < count; ++i) {
        std::vector<double> y(dim);
        for (int j = 0; j < dim; ++j)
            y[j] = signal[i + j * lag];
        vectors.push_back(std::move(y));
    }
    return vectors;
}

} // namespace fnn

#endif // FNN_CORE_EMBEDDING_H
