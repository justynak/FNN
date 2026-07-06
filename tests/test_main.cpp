// Unit tests for the Qt-free math core. No framework dependency: CHECK
// macros count failures and main() returns non-zero if any test failed.
//
// Oracles (chosen because they survive floating-point reordering, unlike
// exact trajectory values of a chaotic system):
//   - analytic fixed points of the single-step map / ODE right-hand side
//   - provable orbit bounds (trapping regions)
//   - exact analytic inverse of the Hénon map (round-trip identity)
//   - orbit/step consistency and determinism

#include <cmath>
#include <cstdio>
#include <random>

#include "core/correlation_dimension.h"
#include "core/embedding.h"
#include "core/fnn.h"
#include "core/henon.h"
#include "core/ikeda.h"
#include "core/logistic.h"
#include "core/lorenz.h"
#include "core/mutual_information.h"

static int g_checks = 0;
static int g_failures = 0;

#define CHECK(cond)                                                          \
    do {                                                                     \
        ++g_checks;                                                          \
        if (!(cond)) {                                                       \
            ++g_failures;                                                    \
            std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond);      \
        }                                                                    \
    } while (0)

#define CHECK_NEAR(a, b, tol)                                                \
    do {                                                                     \
        ++g_checks;                                                          \
        const double _a = (a), _b = (b);                                     \
        if (!(std::fabs(_a - _b) <= (tol))) {                                \
            ++g_failures;                                                    \
            std::printf("FAIL %s:%d  %s == %g, expected %g (tol %g)\n",      \
                        __FILE__, __LINE__, #a, _a, _b, (double)(tol));      \
        }                                                                    \
    } while (0)

using namespace fnn;

// ---------------------------------------------------------------- Hénon

static void test_henon_fixed_points()
{
    // Fixed points solve a*x^2 + (1-b)*x - 1 = 0, y = b*x. Both are
    // saddles, so we assert one application of the map, not orbit
    // invariance: rounding errors would be amplified out of the point.
    const HenonParams prm;
    const double disc = std::sqrt((1.0 - prm.b) * (1.0 - prm.b) + 4.0 * prm.a);
    for (const double sign : {1.0, -1.0}) {
        const double x = (-(1.0 - prm.b) + sign * disc) / (2.0 * prm.a);
        const Point2 fp{x, prm.b * x};
        const Point2 next = henon_step(fp, prm);
        CHECK_NEAR(next.x, fp.x, 1e-12);
        CHECK_NEAR(next.y, fp.y, 1e-12);
    }
}

static void test_henon_inverse_round_trip()
{
    // The inverse is analytic, so inverse(step(p)) must reproduce p to
    // near machine precision along the whole orbit.
    const HenonParams prm;
    Point2 p{1.2, 1.3};
    for (int i = 0; i < 1000; ++i) {
        const Point2 next = henon_step(p, prm);
        const Point2 back = henon_inverse_step(next, prm);
        CHECK_NEAR(back.x, p.x, 1e-12);
        CHECK_NEAR(back.y, p.y, 1e-9);
        p = next;
    }
}

static void test_henon_orbit_bounded()
{
    const auto orbit = henon_orbit({1.2, 1.3}, 10000);
    CHECK(orbit.size() == 10000);
    bool bounded = true;
    for (const Point2 &p : orbit)
        bounded = bounded && std::fabs(p.x) < 2.0 && std::fabs(p.y) < 1.0;
    CHECK(bounded);
}

static void test_henon_orbit_step_consistency()
{
    // Guards the orbit loop itself (off-by-one, state reuse): each entry
    // must be exactly one step from the previous one, starting at init.
    const Point2 init{1.2, 1.3};
    const auto orbit = henon_orbit(init, 100);
    Point2 p = init;
    bool consistent = true;
    for (const Point2 &q : orbit) {
        p = henon_step(p);
        consistent = consistent && p.x == q.x && p.y == q.y;
    }
    CHECK(consistent);
}

// ---------------------------------------------------------------- Ikeda

static void test_ikeda_phase()
{
    // At the origin the formula collapses to c - d.
    const IkedaParams prm;
    CHECK_NEAR(ikeda_phase({0.0, 0.0}, prm), prm.c - prm.d, 1e-15);
}

static void test_ikeda_trapping_ball()
{
    // The map is a norm-preserving rotation scaled by b plus a shift by a,
    // so r' <= a + b*r: any orbit starting with r < a/(1-b) stays there.
    const IkedaParams prm;
    const double radius = prm.a / (1.0 - prm.b);
    const auto orbit = ikeda_orbit({0.5, 0.8}, 10000, prm);
    CHECK(orbit.size() == 10000);
    bool inside = true;
    for (const Point2 &p : orbit)
        inside = inside && std::hypot(p.x, p.y) < radius;
    CHECK(inside);
}

static void test_ikeda_orbit_step_consistency()
{
    const Point2 init{0.5, 0.8};
    const auto orbit = ikeda_orbit(init, 100);
    Point2 p = init;
    bool consistent = true;
    for (const Point2 &q : orbit) {
        p = ikeda_step(p);
        consistent = consistent && p.x == q.x && p.y == q.y;
    }
    CHECK(consistent);
}

// --------------------------------------------------------------- Lorenz

static void test_lorenz_fixed_points()
{
    // Tests the right-hand side, not the integrator: the derivative must
    // vanish at the origin and at C± = (±s, ±s, rho-1), s = sqrt(beta*(rho-1)).
    const LorenzParams prm;
    const Point3 at_origin = lorenz_deriv({0.0, 0.0, 0.0}, prm);
    CHECK(at_origin.x == 0.0 && at_origin.y == 0.0 && at_origin.z == 0.0);

    const double s = std::sqrt(prm.beta * (prm.rho - 1.0));
    for (const double sign : {1.0, -1.0}) {
        const Point3 d = lorenz_deriv({sign * s, sign * s, prm.rho - 1.0}, prm);
        CHECK_NEAR(d.x, 0.0, 1e-12);
        CHECK_NEAR(d.y, 0.0, 1e-12);
        CHECK_NEAR(d.z, 0.0, 1e-12);
    }
}

static void test_lorenz_euler_step()
{
    // One Euler step must be exactly p + dt * deriv(p).
    const LorenzParams prm;
    const Point3 p{1.2, 1.3, 1.6};
    const double dt = 0.005;
    const Point3 d = lorenz_deriv(p, prm);
    const Point3 next = lorenz_euler_step(p, dt, prm);
    CHECK(next.x == p.x + d.x * dt);
    CHECK(next.y == p.y + d.y * dt);
    CHECK(next.z == p.z + d.z * dt);
}

static void test_lorenz_orbit_bounded()
{
    const auto orbit = lorenz_orbit({1.2, 1.3, 1.6}, 0.005, 100000);
    CHECK(orbit.size() == 100000);
    bool bounded = true;
    for (const Point3 &p : orbit)
        bounded = bounded &&
                  std::fabs(p.x) < 30.0 && std::fabs(p.y) < 40.0 &&
                  p.z > -5.0 && p.z < 60.0;
    CHECK(bounded);
}

// --------------------------------------------- Average mutual information

static std::vector<double> tile(const std::vector<double> &pattern, int copies,
                                int extra = 0)
{
    std::vector<double> signal;
    signal.reserve(pattern.size() * copies + extra);
    for (int c = 0; c < copies; ++c)
        signal.insert(signal.end(), pattern.begin(), pattern.end());
    for (int i = 0; i < extra; ++i)
        signal.push_back(pattern[i % pattern.size()]);
    return signal;
}

static std::vector<double> henon_x(int count)
{
    std::vector<double> x;
    x.reserve(count);
    for (const Point2 &p : henon_orbit({1.2, 1.3}, count))
        x.push_back(p.x);
    return x;
}

// Canonical Lorenz test signal: x-component of `steps` RK4 steps at dt=0.01
// from {1.2, 1.3, 1.6}, with the first 500 samples dropped as transient.
static std::vector<double> lorenz_x(int steps)
{
    std::vector<double> x;
    const auto orbit = lorenz_orbit({1.2, 1.3, 1.6}, 0.01, steps);
    for (size_t i = 500; i < orbit.size(); ++i)
        x.push_back(orbit[i].x);
    return x;
}

static void test_ami_debruijn_zero_at_lag1()
{
    // De Bruijn pattern 0,0,1,1 tiled, one extra element so the 4m pairs at
    // lag 1 hit (0,0),(0,1),(1,1),(1,0) exactly m times each: the joint
    // distribution factorizes exactly and AMI is 0 with no estimator bias.
    const auto signal = tile({0.0, 0.0, 1.0, 1.0}, 250, 1);
    CHECK_NEAR(average_mutual_information(signal, 1, 2), 0.0, 1e-12);
}

static void test_ami_periodic_returns_to_entropy()
{
    // At lag 0 and at the full period every sample pairs with itself, so
    // AMI equals the marginal entropy: exactly 1 bit for a balanced binary
    // signal.
    const auto signal = tile({0.0, 0.0, 1.0, 1.0}, 250, 4);
    CHECK_NEAR(average_mutual_information(signal, 0, 2), 1.0, 1e-12);
    CHECK_NEAR(average_mutual_information(signal, 4, 2), 1.0, 1e-12);
}

static void test_ami_lag0_equals_entropy()
{
    // On real (Hénon) data, AMI at lag 0 must reproduce the Shannon entropy
    // of the histogram, computed independently here.
    const int bins = 16;
    const auto signal = henon_x(5000);

    const double lo = *std::min_element(signal.begin(), signal.end());
    const double hi = *std::max_element(signal.begin(), signal.end());
    std::vector<long> hist(bins, 0);
    for (const double v : signal) {
        const int i = int((v - lo) / (hi - lo) * bins);
        ++hist[i < bins ? i : bins - 1];
    }
    double entropy = 0.0;
    for (const long c : hist)
        if (c > 0) {
            const double p = double(c) / signal.size();
            entropy -= p * std::log2(p);
        }

    CHECK_NEAR(average_mutual_information(signal, 0, bins), entropy, 1e-12);
}

static void test_ami_bounds_on_henon()
{
    // For any input: 0 <= I(tau) <= I(0).
    const auto signal = henon_x(5000);
    const auto curve = ami_curve(signal, 20, 16);
    bool in_bounds = true;
    for (const double v : curve)
        in_bounds = in_bounds && v >= -1e-12 && v <= curve[0] + 1e-12;
    CHECK(curve.size() == 21);
    CHECK(in_bounds);
}

static void test_ami_degenerate_inputs()
{
    const std::vector<double> constant(100, 1.0);
    CHECK(average_mutual_information(constant, 0, 16) == 0.0);
    CHECK(average_mutual_information(constant, 5, 16) == 0.0);
    CHECK(average_mutual_information({}, 1, 16) == 0.0);
    CHECK(average_mutual_information({1.0, 2.0}, 5, 16) == 0.0);
}

static void test_first_local_minimum()
{
    CHECK(first_local_minimum({3.0, 2.0, 1.0, 2.0, 3.0}) == 2);
    CHECK(first_local_minimum({5.0, 4.0, 3.0, 2.0, 1.0}) == -1);
    CHECK(first_local_minimum({1.0, 2.0, 1.0, 2.0, 1.0}) == 2);
    CHECK(first_local_minimum({}) == -1);
}

// ------------------------------------------------------------- Logistic

static void test_logistic_fixed_point()
{
    // The nonzero fixed point x* = 1 - 1/r is unstable for r > 3, so this
    // asserts a single map application, not orbit invariance.
    const double r = 3.5;
    const double fp = 1.0 - 1.0 / r;
    CHECK_NEAR(logistic_step(fp, r), fp, 1e-12);
}

static void test_logistic_period_two_cycle()
{
    // For r = 3.2 the attracting 2-cycle is analytic:
    // x± = (r+1 ± sqrt((r+1)(r-3))) / (2r). One step maps each onto the
    // other, and a generic orbit must converge onto the cycle.
    const double r = 3.2;
    const double disc = std::sqrt((r + 1.0) * (r - 3.0));
    const double hi = (r + 1.0 + disc) / (2.0 * r);
    const double lo = (r + 1.0 - disc) / (2.0 * r);
    CHECK_NEAR(logistic_step(hi, r), lo, 1e-12);
    CHECK_NEAR(logistic_step(lo, r), hi, 1e-12);

    const auto orbit = logistic_orbit(0.5, r, 1000);
    bool on_cycle = true;
    for (size_t i = 500; i < orbit.size(); ++i)
        on_cycle = on_cycle && (std::fabs(orbit[i] - hi) < 1e-6 ||
                                std::fabs(orbit[i] - lo) < 1e-6);
    CHECK(on_cycle);
}

static void test_logistic_unit_interval_invariant()
{
    // r = 4 maps [0,1] onto itself (fully chaotic regime).
    bool inside = true;
    for (const double x : logistic_orbit(0.3, 4.0, 10000))
        inside = inside && x >= 0.0 && x <= 1.0;
    CHECK(inside);
}

static void test_logistic_orbit_step_consistency()
{
    const auto orbit = logistic_orbit(0.5, 3.7, 100);
    double x = 0.5;
    bool consistent = true;
    for (const double q : orbit) {
        x = logistic_step(x, 3.7);
        consistent = consistent && x == q;
    }
    CHECK(consistent);
}

// ---------------------------------------------------------- Integrators

// dp/dt = (-x, -2y, -3z): closed-form solution (e^-t, e^-2t, e^-3t) from
// (1,1,1), used to measure the *order* of each integrator. The order is the
// oracle: exact trajectory values would depend on dt, but "halving dt at
// fixed final time T shrinks the global error 2^order times" is analytic.
// Two classic blunders this setup avoids: halving dt at fixed step count
// (compares states at different times), and measuring on a chaotic system
// over a long horizon (e^(lambda*T) growth swamps the dt^order scaling).
static Point3 linear_deriv(Point3 p)
{
    return {-p.x, -2.0 * p.y, -3.0 * p.z};
}

template <class Stepper>
static double linear_system_error(int steps, double T, Stepper step)
{
    const double dt = T / steps;
    Point3 p{1.0, 1.0, 1.0};
    for (int i = 0; i < steps; ++i)
        p = step(p, dt);
    const Point3 exact{std::exp(-T), std::exp(-2.0 * T), std::exp(-3.0 * T)};
    return std::max({std::fabs(p.x - exact.x), std::fabs(p.y - exact.y),
                     std::fabs(p.z - exact.z)});
}

static void test_rk4_is_fourth_order()
{
    const auto step = [](Point3 p, double dt) { return rk4_step(p, dt, linear_deriv); };
    const double ratio = linear_system_error(32, 2.0, step) /
                         linear_system_error(64, 2.0, step);
    CHECK(ratio > 14.0 && ratio < 18.0); // 2^4 = 16
}

static void test_euler_is_first_order()
{
    // Same measurement on Euler must give 2^1: this validates the
    // measurement itself, not just the new integrator.
    const auto step = [](Point3 p, double dt) { return euler_step(p, dt, linear_deriv); };
    const double ratio = linear_system_error(32, 2.0, step) /
                         linear_system_error(64, 2.0, step);
    CHECK(ratio > 1.8 && ratio < 2.3);
}

static Point3 lorenz_rk4_endpoint(int steps, double T)
{
    const double dt = T / steps;
    Point3 p{1.2, 1.3, 1.6};
    for (int i = 0; i < steps; ++i)
        p = lorenz_rk4_step(p, dt);
    return p;
}

static void test_rk4_lorenz_self_convergence()
{
    // Lorenz has no closed form; Richardson self-convergence over a short
    // horizon (T=0.5, before chaotic divergence dominates) must still show
    // the 2^4 ratio between successive dt-halvings.
    const Point3 a = lorenz_rk4_endpoint(50, 0.5);
    const Point3 b = lorenz_rk4_endpoint(100, 0.5);
    const Point3 c = lorenz_rk4_endpoint(200, 0.5);
    const double d1 = std::max({std::fabs(a.x - b.x), std::fabs(a.y - b.y),
                                std::fabs(a.z - b.z)});
    const double d2 = std::max({std::fabs(b.x - c.x), std::fabs(b.y - c.y),
                                std::fabs(b.z - c.z)});
    const double ratio = d1 / d2;
    CHECK(ratio > 12.0 && ratio < 20.0);
}

static void test_rk4_lorenz_fixed_point()
{
    // The derivative vanishes at C+ at every RK4 stage, so the step must
    // stay put (up to FP noise in computing C+ itself).
    const LorenzParams prm;
    const double s = std::sqrt(prm.beta * (prm.rho - 1.0));
    const Point3 fp{s, s, prm.rho - 1.0};
    const Point3 next = lorenz_rk4_step(fp, 0.01, prm);
    CHECK_NEAR(next.x, fp.x, 1e-10);
    CHECK_NEAR(next.y, fp.y, 1e-10);
    CHECK_NEAR(next.z, fp.z, 1e-10);
}

static void test_lorenz_orbit_uses_rk4()
{
    const Point3 init{1.2, 1.3, 1.6};
    const auto orbit = lorenz_orbit(init, 0.005, 100);
    Point3 p = init;
    bool consistent = true;
    for (const Point3 &q : orbit) {
        p = lorenz_rk4_step(p, 0.005);
        consistent = consistent && p.x == q.x && p.y == q.y && p.z == q.z;
    }
    CHECK(consistent);
}

// -------------------------------- Embedding and false nearest neighbors

static void test_delay_embedding_indexing()
{
    const std::vector<double> s{0, 1, 2, 3, 4, 5};
    const auto emb = delay_embedding(s, 2, 2);
    CHECK(emb.size() == 4);
    CHECK(emb[0][0] == 0.0 && emb[0][1] == 2.0);
    CHECK(emb[3][0] == 3.0 && emb[3][1] == 5.0);

    const auto identity = delay_embedding(s, 1, 7); // lag irrelevant at dim 1
    CHECK(identity.size() == 6 && identity[5][0] == 5.0);

    CHECK(delay_embedding({1.0, 2.0}, 3, 5).empty());
    CHECK(delay_vector_count(100, 3, 10) == 80);
}

static void test_fnn_sine_embeds_at_two()
{
    // A sine wave is a limit cycle: 1D delay coordinates fold opposite
    // branches onto each other (false neighbors), 2D unfolds them onto a
    // closed curve. Period of 50.7 samples avoids exact duplicate points.
    std::vector<double> s;
    for (int t = 0; t < 2000; ++t)
        s.push_back(std::sin(2.0 * M_PI * t / 50.7));
    // At dim 1 only ~15% of points are false: the signal spans ~39 periods,
    // so most nearest neighbors are phase-aliased copies from other periods
    // with identical futures — genuinely true neighbors. The discriminating
    // fact is the drop to ~0 at dim 2.
    const int lag = 13; // ~ quarter period
    CHECK(false_neighbor_fraction(s, 1, lag) > 0.1);
    CHECK(false_neighbor_fraction(s, 2, lag) < 0.01);
}

static void test_fnn_henon_embeds_at_two()
{
    // x_{t+2} = 1 - a*x_{t+1}^2 + b*x_t: the next value is an exact
    // function of the last two, so the fraction must collapse at dim 2
    // and stay collapsed.
    const auto s = henon_x(2000);
    const auto curve = fnn_curve(s, 3, 1);
    CHECK(curve[0] > 0.2);
    CHECK(curve[1] < 0.05);
    CHECK(curve[2] < 0.05);
}

static void test_fnn_noise_never_embeds()
{
    // White noise has no finite embedding dimension; the algorithm must be
    // able to say no. Fixed seed + inequality assertion keeps this
    // deterministic and seed-robust. The "no" saturates rather than staying
    // near 1: at high dim the ratio test cannot fire (neighbor distances get
    // large), leaving the size test, which for uniform noise fires with
    // probability (1 - a_tol*sd)^2 ~ 0.18 regardless of sample count. Still
    // several times the <0.05 of a genuinely embedded signal.
    std::mt19937 gen(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::vector<double> s;
    for (int t = 0; t < 2000; ++t)
        s.push_back(dist(gen));
    for (const double fraction : fnn_curve(s, 5, 1))
        CHECK(fraction > 0.15);
}

static void test_fnn_degenerate_inputs()
{
    CHECK(false_neighbor_fraction(std::vector<double>(100, 1.0), 2, 1) == 0.0);
    CHECK(false_neighbor_fraction({1.0, 2.0}, 3, 5) == 0.0);
    CHECK(false_neighbor_fraction({}, 1, 1) == 0.0);
}

static void test_full_recipe_on_lorenz()
{
    // The whole pipeline: AMI picks tau, FNN picks m; the Lorenz attractor
    // must embed at m = 3.
    const auto x = lorenz_x(4500);
    const int tau = first_local_minimum(ami_curve(x, 40, 16));
    CHECK(tau >= 8 && tau <= 30); // literature value ~16 samples at dt=0.01

    const auto curve = fnn_curve(x, 3, tau);
    CHECK(curve[0] > 0.2);
    CHECK(curve[2] < 0.05);
}

// ------------------------------------------------- Correlation dimension

static void test_correlation_integral_exact()
{
    // Hand-computed: pair distances are 0.5, 2.0, 1.5, so with radii
    // {0.6, 1.6, 2.5} the integrals are 1/3, 2/3, 3/3 (d < r, strict).
    const std::vector<std::vector<double>> pts{{0.0}, {0.5}, {2.0}};
    const auto c = correlation_integrals(pts, {0.6, 1.6, 2.5});
    CHECK_NEAR(c[0], 1.0 / 3.0, 1e-15);
    CHECK_NEAR(c[1], 2.0 / 3.0, 1e-15);
    CHECK_NEAR(c[2], 1.0, 1e-15);

    // Theiler window 1 leaves only the pair (0, 2.0), distance 2.
    const auto ct = correlation_integrals(pts, {1.0}, 1);
    CHECK(ct[0] == 0.0);
}

static void test_correlation_integral_unsorted_radii_and_theiler()
{
    // Same point set as the exact test; radii in arbitrary order must give
    // the same integrals, index-aligned with the input.
    const std::vector<std::vector<double>> pts{{0.0}, {0.5}, {2.0}};
    const auto c = correlation_integrals(pts, {2.5, 0.6, 1.6});
    CHECK_NEAR(c[0], 1.0, 1e-15);
    CHECK_NEAR(c[1], 1.0 / 3.0, 1e-15);
    CHECK_NEAR(c[2], 2.0 / 3.0, 1e-15);

    // A negative Theiler window means 0, not self-pairs at distance zero.
    const auto c0 = correlation_integrals(pts, {1.0});
    const auto cn = correlation_integrals(pts, {1.0}, -2);
    CHECK(cn[0] == c0[0]);

    // A window wider than the data excludes every pair.
    const auto ce = correlation_integrals(pts, {10.0}, 5);
    CHECK(ce[0] == 0.0);
}

static void test_correlation_dimension_degenerate_inputs()
{
    // Degenerate arguments yield 0 (the convention set by AMI and FNN),
    // never NaN, inf, or an out-of-range access.
    const std::vector<std::vector<double>> pts{{0.0}, {0.5}, {2.0}};
    CHECK(correlation_dimension(pts, 0.1, 0.1) == 0.0);      // empty range
    CHECK(correlation_dimension(pts, 0.2, 0.1) == 0.0);      // reversed range
    CHECK(correlation_dimension(pts, 0.0, 0.1) == 0.0);      // log(0) radius
    CHECK(correlation_dimension(pts, 0.01, 1.0, 1) == 0.0);  // one radius
    CHECK(correlation_dimension(pts, 0.01, 1.0, -3) == 0.0); // negative count
    CHECK(correlation_dimension({}, 0.01, 1.0) == 0.0);      // no points
    CHECK(correlation_dimension({{1.0}}, 0.01, 1.0) == 0.0); // no pairs

    // Radii entirely below the smallest pair distance: no data to fit.
    CHECK(correlation_dimension(pts, 0.001, 0.01) == 0.0);
}

static void test_correlation_dimension_known_sets()
{
    // Point sets with known dimension. Uniform sets use a fixed seed and
    // tolerance-band assertions; the mild downward bias on the line/square
    // (~ 1 - r/2L) is edge effect, not error.
    std::mt19937 gen(42);
    std::uniform_real_distribution<double> u(0.0, 1.0);
    std::vector<std::vector<double>> line, square, circle;
    for (int i = 0; i < 2000; ++i) {
        line.push_back({u(gen)});
        square.push_back({u(gen), u(gen)});
        const double th = 2.0 * M_PI * 0.3819660113 * i; // golden angle
        circle.push_back({std::cos(th), std::sin(th)});
    }
    const double d_line = correlation_dimension(line, 0.01, 0.1);
    const double d_square = correlation_dimension(square, 0.01, 0.1);
    const double d_circle = correlation_dimension(circle, 0.02, 0.3);
    CHECK(d_line > 0.93 && d_line < 1.03);     // exact: 1
    CHECK(d_square > 1.90 && d_square < 2.05); // exact: 2
    CHECK(d_circle > 0.97 && d_circle < 1.08); // exact: 1 (deterministic set)
}

static void test_correlation_dimension_henon()
{
    // End-to-end from the scalar series: embed x with (m=2, tau=1) and
    // recover the published attractor dimension D2 ~ 1.22.
    const auto emb = delay_embedding(henon_x(4000), 2, 1);
    const double d2 = correlation_dimension(emb, 0.01, 0.2, 12, 2);
    CHECK(d2 > 1.1 && d2 < 1.3); // measured 1.21
}

static void test_correlation_dimension_lorenz_full_pipeline()
{
    // The complete recipe ends here: scalar signal -> AMI picks tau ->
    // embed at m=3 -> D2 ~ 2.06 (published). The scaling region tops out
    // at ~5% of the attractor extent; larger radii sample the two-lobe
    // macrostructure and depress the slope (measured 1.83 at r_max=6).
    const auto x = lorenz_x(8500);
    const int tau = first_local_minimum(ami_curve(x, 40, 16));
    CHECK(tau >= 8 && tau <= 30); // a -1 (no minimum) must not reach delay_embedding
    const auto emb = delay_embedding(x, 3, tau);
    const double d2 = correlation_dimension(emb, 0.3, 2.0, 12, 2 * tau);
    CHECK(d2 > 1.9 && d2 < 2.2); // measured 2.07
}

int main()
{
    test_henon_fixed_points();
    test_henon_inverse_round_trip();
    test_henon_orbit_bounded();
    test_henon_orbit_step_consistency();
    test_ikeda_phase();
    test_ikeda_trapping_ball();
    test_ikeda_orbit_step_consistency();
    test_logistic_fixed_point();
    test_logistic_period_two_cycle();
    test_logistic_unit_interval_invariant();
    test_logistic_orbit_step_consistency();
    test_lorenz_fixed_points();
    test_lorenz_euler_step();
    test_lorenz_orbit_bounded();
    test_rk4_is_fourth_order();
    test_euler_is_first_order();
    test_rk4_lorenz_self_convergence();
    test_rk4_lorenz_fixed_point();
    test_lorenz_orbit_uses_rk4();
    test_ami_debruijn_zero_at_lag1();
    test_ami_periodic_returns_to_entropy();
    test_ami_lag0_equals_entropy();
    test_ami_bounds_on_henon();
    test_ami_degenerate_inputs();
    test_first_local_minimum();
    test_delay_embedding_indexing();
    test_fnn_sine_embeds_at_two();
    test_fnn_henon_embeds_at_two();
    test_fnn_noise_never_embeds();
    test_fnn_degenerate_inputs();
    test_full_recipe_on_lorenz();
    test_correlation_integral_exact();
    test_correlation_integral_unsorted_radii_and_theiler();
    test_correlation_dimension_degenerate_inputs();
    test_correlation_dimension_known_sets();
    test_correlation_dimension_henon();
    test_correlation_dimension_lorenz_full_pipeline();

    std::printf("%d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
