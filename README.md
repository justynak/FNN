# FNN — False Nearest Neighbors (chaotic time-series analysis)

An unfinished Qt 5 desktop application (started ~2015, QtCreator-generated) laying the
groundwork for **nonlinear time-series analysis** — specifically *phase-space
reconstruction* of chaotic signals via Takens embedding. The name FNN refers to the
**False Nearest Neighbors** algorithm for choosing the embedding dimension of a time
series.

Only the data-generation and plotting parts were ever implemented; the analysis
algorithms themselves are empty stubs.

## Repository layout

| Path | What it is |
|---|---|
| `FNN.pro`, `*.cpp`, `*.h`, `mainwindow.ui` | The main FNN application |
| `MutualInformation/` | A separate, standalone Qt app (own `.pro` file) |
| `qcustomplot.cpp/.h` | Third-party plotting library ([QCustomPlot](https://www.qcustomplot.com/)), vendored in both apps |
| `build-MutualInformation-...-Debug/` | Build artifacts, not source |

## What actually works

### 1. Main FNN app

On startup, `MainWindow` generates three classic chaotic systems and shows each as a
scatter plot (x vs. y) in its own QCustomPlot panel — drawing the well-known strange
attractors:

- **`CIkedaGenerator`** — iterates the Ikeda map (a discrete chaotic map from laser
  physics), 100,000 points.
- **`CHenonGenerator`** — iterates the Hénon map
  (`x' = 1 − a·x² + y`, `y' = b·x`, canonical parameters a=1.4, b=0.3), 10,000 points.
- **`CLorentzGenerator`** — integrates the Lorenz system (σ=10, β=8/3, ρ=28) with
  forward-Euler steps of dt=0.005, 100,000 points.

### 2. `MutualInformation/` subproject

A standalone app that generates a **logistic map** time series (`x' = r·x·(1−x)`) and
plots it, with a slider that changes the parameter `r` live — letting you watch the
signal go from a stable fixed point through period-doubling into chaos as r → 4.
Despite the project's name, its `MutualInformationSolver` class is empty; only the
logistic-map demo works.

## Architecture (as sketched)

A small class hierarchy separates data from algorithms:

- `ICData` — abstract base for data containers
  - `ICMultipleDimDataSeries<T>` — templated point-list container, specialized as
    `C2DDataSeries` (`QVector2D`) and `C3DDataSeries` (`QVector3D`)
  - `CTimeseriesOneDimension` / `CTimeseriesMultiDimension` — sampled-signal containers
- `ICSolver` — abstract base for anything with a `Solve()` method
  - `ICDataGenerator` — base for the chaotic-system generators above
- `ICAlgorithm`, `ICSolution`, `ICNotifier` — supporting scaffolding, unused

## What was planned but never built

The class names reveal the intended workflow — the standard Takens-embedding recipe:

1. **`CMutualInformationSolver`** — compute average mutual information of the signal
   vs. its lagged copy; the first minimum picks the time delay τ. *(empty stub)*
2. **`CFNNSolver`** — run the False Nearest Neighbors algorithm to pick the embedding
   dimension m. *(empty stub)*
3. **`CEmbeddingSolver`** — reconstruct the attractor from a single scalar signal
   using (τ, m). *(empty stub)*

The corresponding data classes (`CFNNData`, `CMutualInformationData`,
`CEmbeddedSignalData`) are likewise empty. None of the analysis math exists in the
code — the project stops at "generate chaotic data and look at it."

## Building

Both apps now build on Linux with hand-written Makefiles against Qt 5.15
(no Qt Creator / qmake needed). Install the dependencies once:

```sh
sudo apt-get install qtbase5-dev libqt5opengl5-dev
```

Then:

```sh
make          # builds the main app → build/FNN
make run      # builds and launches it

cd MutualInformation
make run      # builds and launches build/MutualInformation
```

`make clean` removes the `build/` directory. The original QtCreator project
files (`FNN.pro`, `MutualInformation/MutualInformation.pro`) are kept for
reference; the project was originally developed with Qt 5.4.1 / MinGW 32-bit
(see the old build folder name).
