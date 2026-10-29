# One-Minute Informal Presentation

## Problem

Harbor entrance geometry changes how long waves enter and reflect inside a
protected basin. A narrow entrance may lower interior elevation while producing
stronger currents, so there may not be one design that is best for every metric
and wave condition.

## Method

HarborTune solves the two-dimensional linear shallow-water equations for 15
idealized breakwater layouts and four incident-wave scenarios. It measures
interior elevation, entrance speed, and wave energy. The 60 independent cases
are evaluated sequentially and with an OpenMP parameter sweep. The solver is
checked against theoretical wave speed, mirrored geometry, grid refinement,
and sequential-parallel agreement.

## Expected result

The study is expected to reveal an elevation-current tradeoff and show that the
best layout for one wave is not necessarily the most robust layout across all
scenarios. OpenMP is expected to reduce total sweep runtime, with efficiency
eventually limited by overhead and available CPU resources.

## Spoken version

> HarborTune studies how the position and width of a breakwater entrance affect
> long waves inside a simplified harbor. We solve the two-dimensional linear
> shallow-water equations and compare 15 entrance layouts under three periodic
> waves and a tsunami-like pulse. Each case measures peak interior elevation,
> entrance-current speed, and wave energy. Because the 60 simulations are
> independent, we evaluate them with OpenMP and compare runtime, speedup, and
> efficiency against a sequential baseline. We expect to find that narrow
> entrances can reduce elevation while increasing current, so the best design
> depends on the chosen metric and wave scenario. This is an educational
> screening model rather than a real harbor certification tool.
