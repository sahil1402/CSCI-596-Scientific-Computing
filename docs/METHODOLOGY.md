# Numerical and Experimental Methodology

## Governing equations

HarborTune uses the linear shallow-water equations for a constant undisturbed
depth. The state is water-surface displacement and two depth-averaged velocity
components. Linear friction is applied to velocity to represent weak damping.

The system is discretized with a first-order finite-volume method. A local
Lax–Friedrichs (Rusanov) flux is evaluated at each cell face using the constant
characteristic speed `sqrt(gH)`. This method is intentionally conservative and
robust for the educational parameter study.

The two-dimensional stable time step is

```text
dt <= CFL / (sqrt(gH) * (1/dx + 1/dy)).
```

The program divides the requested duration by an integer number of steps so
that the final simulated time is exact and the selected time step does not
exceed the stability limit.

## Boundary conditions

- Top, bottom, right, and solid-structure faces reflect the normal velocity.
- The left boundary uses linear shallow-water characteristics: the incoming
  characteristic is prescribed by the selected wave, while the outgoing
  characteristic is taken from the adjacent interior state.
- Breakwaters are solid grid masks with a fluid opening at the selected
  position and width.

## Incident waves

Periodic cases have 20 s, 30 s, and 45 s periods and use a one-period amplitude
ramp. The transient case is a Gaussian elevation pulse. These inputs are
controlled comparison cases; the pulse does not simulate earthquake rupture,
ocean-scale propagation, or coastal inundation.

## Measured quantities

For every case the program records:

- maximum absolute elevation in the protected region;
- maximum velocity magnitude in the entrance region;
- time-averaged linear wave energy in the protected region;
- duration of elevation above five percent of incident amplitude;
- final domain-integrated elevation anomaly; and
- wall-clock runtime.

The energy density used for comparison is

```text
0.5 * g * eta^2 + 0.5 * H * (u^2 + v^2).
```

## Design score

Each physical metric is divided by the result from the center-position,
medium-width reference layout under the same wave. The per-case score is the
equal-weight mean of normalized peak elevation, entrance speed, and mean
energy. Individual metrics are retained in all output files.

The robust score is the maximum per-case score across the four wave scenarios.
Lower scores indicate better worst-case performance within this idealized
experiment. The composite score is a comparison aid, not a universal safety
criterion.

## Parallel experiment design

The 15 designs and four waves form 60 independent simulations. OpenMP dynamic
scheduling distributes the outer experiment loop across threads. Each
simulation owns its complete state arrays, so no solver state is shared among
workers. This design makes numerical results deterministic and avoids data
races.

Strong scaling is measured against a genuinely sequential sweep. Every timing
configuration is repeated, and mean, standard deviation, minimum time,
speedup, and efficiency are exported.

## Verification criteria

- Wave-speed relative error must be below 12 percent on the verification grid.
- All state values must remain finite and bounded.
- Mirrored layouts must agree within `1e-10` relative error.
- Sequential and OpenMP case metrics must agree within `1e-12` relative error.
- Grid-refinement measurements are reported to expose numerical sensitivity.

The loose wave-speed threshold accounts for the intentionally diffusive
first-order method, while the measured result is normally substantially closer.
