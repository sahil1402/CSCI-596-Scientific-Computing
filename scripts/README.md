# Scientific analysis and visualization

The scripts in this directory consume CSV output from the C++ executable.
They use a non-interactive Matplotlib backend and can run on a cluster login
node or through a batch job.

```bash
python3 scripts/plot_sweep.py results/data/sweep results/figures
python3 scripts/plot_benchmark.py results/data/benchmark.csv results/figures
python3 scripts/plot_verification.py results/data/verification results/figures
python3 scripts/animate_simulation.py results/data/simulation \
    --output results/animations/wave_elevation.gif
```

Run `scripts/run_demo.sh` to generate a compact end-to-end demonstration.
