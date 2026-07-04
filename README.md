# FNN — False Nearest Neighbors (chaotic time-series analysis)

A Qt 5 desktop application (started ~2015) laying the groundwork for **nonlinear
time-series analysis** — specifically *phase-space reconstruction* of chaotic signals
via Takens embedding. The name FNN refers to the **False Nearest Neighbors** algorithm
for choosing the embedding dimension of a time series.

The data-generation and plotting parts are implemented; the analysis algorithms
(mutual information, FNN, embedding) are not yet.

## Repository layout

| Path | What it is |
|---|---|
| `core/` | Qt-free math core: generators and analysis algorithms, header-only |
| `tests/` | Unit tests for `core/` (no framework, no Qt — plain C++) |
| `main.cpp`, `mainwindow.*`, `mainwindow.ui` | Qt shell: generates the systems via `core/` and plots them |
| `qcustomplot.cpp/.h` | Third-party plotting library ([QCustomPlot](https://www.qcustomplot.com/) 1.3.1), vendored |
| `FNN.pro` | Original QtCreator project file, kept for reference |

## The math core (`core/`)

Plain C++17, header-only, no Qt. Each system exposes its **single-step map (or ODE
right-hand side) as a pure function**, plus an orbit generator built on top of it —
the single-step functions are what the unit tests pin down:

- `core/ikeda.h` — Ikeda map (`ikeda_phase`, `ikeda_step`, `ikeda_orbit`)
- `core/henon.h` — Hénon map, including its analytic inverse
  (`henon_step`, `henon_inverse_step`, `henon_orbit`)
- `core/integrate.h` — fixed-step ODE integrators (forward Euler and classical
  RK4), generic over the system so their convergence order is testable
- `core/lorenz.h` — Lorenz system, integrated with RK4
  (`lorenz_deriv`, `lorenz_euler_step`, `lorenz_rk4_step`, `lorenz_orbit`)
- `core/logistic.h` — logistic map (`logistic_step`, `logistic_orbit`)
- `core/mutual_information.h` — histogram-based average mutual information
  (Fraser & Swinney) for choosing the embedding delay τ
  (`average_mutual_information`, `ami_curve`, `first_local_minimum`)
- `core/embedding.h` — Takens delay embedding
  (`delay_embedding`, `delay_vector_count`)
- `core/fnn.h` — False Nearest Neighbors (Kennel–Brown–Abarbanel) for choosing
  the embedding dimension m (`false_neighbor_fraction`, `fnn_curve`)

Default parameters are the classic chaotic ones (Hénon a=1.4 b=0.3; Lorenz σ=10,
ρ=28, β=8/3; Ikeda as plotted by the app).

### What the tests assert

Exact trajectory values of a chaotic system are not a usable oracle — sensitive
dependence amplifies any change in floating-point evaluation order. The tests in
`tests/test_main.cpp` therefore check properties that survive reordering:

- **analytic fixed points** of each single-step map / ODE right-hand side
  (one map application only — the fixed points are unstable saddles);
- **provable orbit bounds** — e.g. the Ikeda map is a rotation scaled by b plus a
  shift by a, so every orbit is trapped in the ball of radius a/(1−b);
- **exact inverse round-trip** for the Hénon map (analytically invertible for b≠0);
- **convergence order** for the integrators: at fixed final time, halving dt must
  shrink the global error 16× for RK4 and 2× for Euler (measured against a linear
  system with closed-form solution, plus Richardson self-convergence on Lorenz
  over a short horizon);
- orbit/step consistency and determinism.

The AMI estimator is tested against constructed inputs with exact answers rather
than statistical ones: a tiled de Bruijn sequence whose lag-1 joint distribution
factorizes exactly (AMI = 0, no estimator bias), the identity I(0) = H (entropy of
the histogram), exact return to full entropy at the signal period, and the bounds
0 ≤ I(τ) ≤ I(0) on Hénon data. On a Lorenz x-trajectory the first AMI minimum
lands at τ·dt ≈ 0.16, matching the published value.

## The Qt app

Three tabs:

- **Attractors** — the three systems generated via `core/`, each as an x-vs-y
  scatter plot (Ikeda 100k points, Hénon 10k, Lorenz 100k at dt=0.005).
- **Logistic map** — a logistic-map time series with a slider that changes the
  parameter r live, showing the route from fixed point through period-doubling
  into chaos as r → 4. (Formerly the standalone `MutualInformation/` app.)
- **Takens reconstruction** — the full pipeline run on the Lorenz x-coordinate
  alone: the true attractor, the AMI curve with the chosen τ marked, the FNN
  curve with the chosen m marked, and the attractor reconstructed from the
  scalar signal via delay embedding.

`./build/FNN --screenshot <dir>` renders every tab to PNG and exits (headless
verification).

## Analysis pipeline

The standard Takens-embedding recipe is implemented as pure functions in `core/`:

1. **Average mutual information** (`core/mutual_information.h`) — the first minimum
   of the AMI curve picks the time delay τ.
2. **Embedding** (`core/embedding.h`) — reconstruct delay vectors from a single
   scalar signal using (τ, m).
3. **False Nearest Neighbors** (`core/fnn.h`) — the dimension where the
   false-neighbor fraction collapses picks the embedding dimension m.

The FNN tests use three signals with known answers: a sine wave (embeds at m=2,
analytically), the Hénon x-series (embeds at m=2, since x_{t+2} is an exact function
of the last two values), and fixed-seed white noise (must never embed — the fraction
stays high at every m). An end-to-end test runs the whole recipe on a Lorenz
x-trajectory: AMI picks τ ≈ 16 samples (dt=0.01, the literature value) and FNN
collapses at m = 3, the known answer for the Lorenz attractor.

## What's next (planned)

- Correlation dimension (Grassberger–Procaccia) as an end-to-end validation of
  the embedding pipeline (Hénon D₂ ≈ 1.22, Lorenz D₂ ≈ 2.06).

## Building and testing

Builds on Linux with hand-written Makefiles against Qt 5.15. Install the
dependencies once:

```sh
sudo apt-get install qtbase5-dev libqt5opengl5-dev
```

Then:

```sh
make test     # builds and runs the core unit tests (needs no Qt)
make          # builds the app → build/FNN
make run      # builds and launches it
```

`make clean` removes the `build/` directory. The original QtCreator project file
is kept for reference; the project was originally developed with Qt 5.4.1 /
MinGW 32-bit.
