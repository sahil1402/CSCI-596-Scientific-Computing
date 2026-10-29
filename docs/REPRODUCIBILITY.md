# Reproducibility Guide

## Record the environment

Before benchmarking, record:

```bash
uname -a
c++ --version
python3 --version
./bin/harbortune --version
```

Also record CPU model, physical core count, operating system, OpenMP runtime,
and whether the computer was connected to external power.

## Build from a clean checkout

```bash
make clean
make
make test
```

## Reproduce verification

```bash
./bin/harbortune verify --output results/data/verification
python3 scripts/plot_verification.py \
  results/data/verification results/figures
```

## Reproduce the design study

Choose a thread count no larger than the available hardware threads:

```bash
./bin/harbortune sweep --threads 8 --output results/data/sweep
python3 scripts/plot_sweep.py results/data/sweep results/figures
```

## Reproduce scaling results

Use an optimized build, close unrelated applications, and repeat each timing:

```bash
./bin/harbortune benchmark \
  --threads 1,2,4,8 \
  --repetitions 5 \
  --output results/data/benchmark.csv
python3 scripts/plot_benchmark.py \
  results/data/benchmark.csv results/figures
```

Very small grids can produce unrealistically ideal scaling because each sweep
finishes quickly. The final report should use the default grid or a larger one.

## Reproduce an animation

```bash
./bin/harbortune simulate \
  --position center --width medium --wave pulse --snapshots 60 \
  --output results/data/simulation
python3 scripts/animate_simulation.py results/data/simulation \
  --output results/animations/wave_elevation.gif
```

Generated files are ignored by Git by default. Copy selected, reviewed results
into `docs/images/` if they are intended to appear in the repository.

