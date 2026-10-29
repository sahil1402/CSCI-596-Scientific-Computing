# HarborTune

## Parallel Screening of Breakwater Entrances for Long-Wave Resilience

HarborTune is a C++17 scientific-computing project that studies how the
position and width of a simplified breakwater entrance affect long-wave
behavior inside an idealized harbor. It solves the two-dimensional linear
shallow-water equations, evaluates 15 harbor layouts under four wave
scenarios, ranks the layouts using elevation, current, and energy metrics,
and parallelizes the 60 independent simulations with OpenMP.

The project was developed for **CSCI 596: Scientific Computing and
Visualization**. It combines numerical simulation, parallel computing,
verification, performance analysis, and scientific visualization.

> **Scope:** HarborTune is an educational design-screening model. It is not a
> construction-ready coastal-engineering tool and does not certify the safety
> or tsunami resistance of a real harbor.

## Research questions

1. How do breakwater entrance position and width affect peak water elevation,
   interior wave energy, and entrance-current speed under periodic long waves
   and a tsunami-like pulse?
2. How effectively can the independent design cases be evaluated with an
   OpenMP parallel parameter sweep?

The working hypothesis is that narrow entrances can reduce interior elevation
while increasing entrance currents. The configuration with the lowest water
level may therefore differ from the most robust configuration across all
metrics and wave scenarios.

## Features

- finite-volume linear shallow-water solver on a regular 2D grid;
- reflective harbor and breakwater boundaries;
- characteristic offshore forcing for periodic waves and a Gaussian pulse;
- five entrance positions and three entrance widths;
- peak elevation, entrance speed, wave energy, activity duration, and mass
  anomaly measurements;
- sequential and OpenMP design sweeps;
- normalized multi-metric and worst-case robust design rankings;
- wave-speed, stability, symmetry, refinement, and parallel-consistency tests;
- repeated strong-scaling benchmarks; and
- publication-style plots and animated wave fields.

## Model

For constant depth \(H\), HarborTune advances

$$
\frac{\partial \eta}{\partial t}
+H\left(\frac{\partial u}{\partial x}+\frac{\partial v}{\partial y}\right)=0,
$$

$$
\frac{\partial u}{\partial t}+g\frac{\partial \eta}{\partial x}=-ru,
\qquad
\frac{\partial v}{\partial t}+g\frac{\partial \eta}{\partial y}=-rv.
$$

The numerical flux is a first-order Rusanov flux. The time step is selected
from a two-dimensional CFL restriction using \(c=\sqrt{gH}\). See
[docs/METHODOLOGY.md](docs/METHODOLOGY.md) for the equations, boundary
conditions, scoring method, assumptions, and verification plan.

## Requirements

### Simulation

- C++17 compiler;
- GNU Make or CMake 3.16+; and
- OpenMP runtime.

On macOS with Apple Clang, install `libomp` with Homebrew. The Makefile detects
the Homebrew installation automatically. On Linux, a recent GCC installation
normally provides OpenMP support.

### Analysis

- Python 3.9+; and
- packages listed in `requirements.txt`.

```bash
python3 -m pip install -r requirements.txt
```

## Build and test

```bash
make
make test
```

The executable is created at `bin/harbortune`. A sequential fallback can be
built with `make NO_OPENMP=1`, although parallel sweeps will then execute with
one worker.

An optional CMake build is also available:

```bash
cmake -S . -B build
cmake --build build
```

## Usage

List the predefined layouts and wave cases:

```bash
./bin/harbortune list
```

Run and export one simulation:

```bash
./bin/harbortune simulate \
  --position center \
  --width medium \
  --wave pulse \
  --snapshots 40 \
  --output results/data/simulation
```

Run the full 60-case OpenMP sweep:

```bash
./bin/harbortune sweep \
  --threads 8 \
  --output results/data/sweep
```

Run scientific verification:

```bash
./bin/harbortune verify \
  --output results/data/verification
```

Benchmark strong scaling:

```bash
./bin/harbortune benchmark \
  --threads 1,2,4,8 \
  --repetitions 3 \
  --output results/data/benchmark.csv
```

Generate plots and an animation:

```bash
python3 scripts/plot_sweep.py results/data/sweep results/figures
python3 scripts/plot_benchmark.py results/data/benchmark.csv results/figures
python3 scripts/plot_verification.py results/data/verification results/figures
python3 scripts/animate_simulation.py results/data/simulation \
  --output results/animations/wave_elevation.gif
```

The complete demonstration can be reproduced with:

```bash
THREADS=8 scripts/run_demo.sh
```

## Experiment matrix

The default sweep evaluates:

- positions: left, left-center, center, right-center, and right;
- widths: narrow, medium, and wide; and
- waves: 20 s, 30 s, and 45 s periodic cases plus one Gaussian pulse.

This gives \(5\times3\times4=60\) independent simulations. The center/medium
layout is the normalization reference for every wave scenario. A design's
robust score is its worst composite score across the four scenarios.

## Verification

`harbortune verify` performs the following checks:

- measured wave speed versus \(\sqrt{gH}\);
- finite, bounded numerical state;
- mirrored-layout symmetry;
- sequential/OpenMP result equivalence; and
- three-level grid-refinement sensitivity.

The regression test executable also exercises case parsing, geometry masks,
solver stability, sweep size, ranking size, and parallel equivalence.

## Repository structure

```text
├── include/harbortune/  C++ public interfaces
├── src/                 solver, experiments, verification, and CLI
├── tests/               automated C++ regression tests
├── scripts/             analysis, plots, animation, and demo pipeline
├── config/              documented experiment matrix
├── docs/                methodology and presentation material
├── results/             generated data, figures, and animations
├── specs/               original proposal and planning context
├── Makefile
└── CMakeLists.txt
```

## Limitations

The current model assumes constant depth, small-amplitude waves, depth-averaged
velocity, idealized geometry, and rigid reflecting structures. It does not
model real bathymetry, nonlinear breaking, wetting and drying, inundation,
sediment, debris, tides, structural loads, vessel motion, or construction
constraints. Conclusions apply only to the controlled numerical experiments.

## Documentation

- [Formal project proposal](specs/HarborTune.md)
- [Numerical and experimental methodology](docs/METHODOLOGY.md)
- [Reproducibility guide](docs/REPRODUCIBILITY.md)
- [One-minute presentation outline](docs/PRESENTATION.md)

## References

1. OpenMP Architecture Review Board, [OpenMP API
   Specifications](https://www.openmp.org/specifications/).
2. NOAA Center for Tsunami Research, [Tsunami Modeling and
   Research](https://nctr.pmel.noaa.gov/model.html).
3. NOAA Center for Tsunami Research, [Benchmark
   Problems](https://nctr.pmel.noaa.gov/benchmark/index.html).
4. R. J. LeVeque, *Finite Volume Methods for Hyperbolic Problems*, Cambridge
   University Press, 2002.

