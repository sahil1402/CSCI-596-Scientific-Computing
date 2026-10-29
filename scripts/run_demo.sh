#!/usr/bin/env bash
set -euo pipefail
threads="${THREADS:-4}"
make
./bin/harbortune verify --output results/data/verification
./bin/harbortune sweep --threads "$threads" --output results/data/sweep
./bin/harbortune benchmark --threads "1,2,4,$threads" --repetitions 2 --output results/data/benchmark.csv
./bin/harbortune simulate --position center --width medium --wave pulse --snapshots 40 --output results/data/simulation
python3 scripts/plot_sweep.py results/data/sweep results/figures
python3 scripts/plot_benchmark.py results/data/benchmark.csv results/figures
python3 scripts/plot_verification.py results/data/verification results/figures
python3 scripts/animate_simulation.py results/data/simulation --output results/animations/wave_elevation.gif
echo "HarborTune demonstration outputs are under results/."
