# HarborTune

## Parallel Screening of Breakwater Entrances for Long-Wave Resilience

**Course:** CSCI 596 — Scientific Computing and Visualization  
**Project type:** Programming  
**Author(s):** _To be added_  
**Final repository due:** December 11, 2026

## 1. Project Overview

Harbor entrances and breakwaters influence how long waves enter, reflect within, and leave a protected basin. A configuration that reduces water-surface elevation inside a harbor may also produce high current speeds through its entrance. Furthermore, a layout that performs well for one incoming wave period may perform poorly for another.

HarborTune is a scientific-computing prototype for comparing simplified breakwater entrance designs under controlled long-wave conditions. It will use a two-dimensional numerical model to simulate wave propagation through an idealized harbor. A discrete collection of entrance positions and widths will be tested against periodic long waves and a tsunami-like pulse. Each design will be evaluated using peak interior water elevation, entrance-current speed, and time-averaged wave energy.

The design-and-wave combinations are independent. HarborTune will therefore use OpenMP to evaluate them in parallel and will compare the performance of sequential and parallel parameter sweeps. Scientific visualizations will show the simulated wave fields, current patterns, design rankings, and measured parallel speedup.

The project is intended as an educational design-screening model. It is not a construction-ready coastal-engineering system and does not certify the safety or tsunami resistance of a real harbor.

## 2. Motivation

Harbor resonance and wave amplification can expose vessels, infrastructure, and harbor operations to damaging water-level fluctuations and currents. The response of a harbor depends on both the incident wave and the basin geometry. Evaluating multiple configurations is naturally a parameter-sweep problem: the same numerical model must be executed repeatedly with different design and forcing parameters.

This project combines four central scientific-computing activities:

1. Numerical solution of a system of partial differential equations.
2. Parallel evaluation of independent simulation cases.
3. Quantitative comparison and validation of numerical results.
4. Scientific visualization of spatial and performance data.

## 3. Problem Statement

The project will determine how the position and width of a simplified breakwater entrance affect long-wave behavior inside an idealized rectangular harbor. Each candidate design will be exposed to the same collection of incoming waves. The simulations will measure interior elevation, wave energy, and entrance currents so that the designs can be compared under consistent conditions.

The computational problem is to evaluate the complete design space efficiently. A sequential implementation will provide the correctness and performance baseline. An OpenMP implementation will distribute independent design-and-wave simulations among CPU threads. Runtime, speedup, and parallel efficiency will be measured as the number of threads and experiments changes.

## 4. Research Questions

The project addresses two primary questions:

1. **Physical question:** How do breakwater entrance position and width affect peak water elevation, average wave energy, and entrance-current speed under long-period and tsunami-like disturbances?
2. **Computational question:** How effectively can a collection of independent harbor-design simulations be evaluated with an OpenMP parallel parameter sweep?

## 5. Hypothesis

Narrow entrances are expected to reduce interior wave elevation for some incident-wave conditions while producing higher entrance-current speeds. Consequently, the design with the lowest interior elevation will not necessarily have the best overall resilience score. A configuration optimized for one periodic wave may also perform poorly under other wave periods or a tsunami-like pulse.

For the computational component, the OpenMP parameter sweep is expected to reduce total runtime as the number of threads increases. Parallel efficiency is expected to decrease at higher thread counts because of scheduling overhead, memory contention, and differences in simulation workload.

## 6. Project Objectives

The project has the following specific objectives:

1. Implement a stable two-dimensional long-wave solver.
2. Verify the solver using theoretical wave speed and grid-refinement experiments.
3. Represent a rectangular harbor, breakwater, entrance, and protected measurement region on a numerical grid.
4. Generate periodic long-wave inputs and a controlled tsunami-like pulse.
5. Evaluate multiple entrance positions and widths under identical wave scenarios.
6. Calculate physically interpretable resilience metrics for each experiment.
7. Implement sequential and OpenMP parameter sweeps.
8. Measure runtime, speedup, and parallel efficiency.
9. Produce animations, heatmaps, design comparisons, and performance graphs.
10. Document a reproducible build, execution, and analysis workflow.

## 7. Model Scope and Geometry

The initial model will use a deliberately controlled geometry:

- a rectangular two-dimensional computational domain;
- constant water depth;
- a straight coastline;
- a rectangular harbor basin;
- one straight breakwater;
- one opening through the breakwater; and
- one protected region in which wave behavior is measured.

Two discrete design variables will be investigated:

### Entrance position

- left;
- left-center;
- center;
- right-center; and
- right.

### Entrance width

- narrow;
- medium; and
- wide.

The initial design space therefore contains

$$
5\ \text{positions} \times 3\ \text{widths} = 15\ \text{designs}.
$$

With four wave scenarios, the complete experiment contains 60 independent simulations. This is sufficiently large to demonstrate parallel parameter evaluation while remaining practical within the project schedule.

## 8. Physical Model

HarborTune will use the linear shallow-water equations with constant undisturbed depth $H$:

$$
\frac{\partial \eta}{\partial t}
+ H\left(
\frac{\partial u}{\partial x}
+ \frac{\partial v}{\partial y}
\right)=0,
$$

$$
\frac{\partial u}{\partial t}
+ g\frac{\partial \eta}{\partial x}
=-ru,
$$

$$
\frac{\partial v}{\partial t}
+ g\frac{\partial \eta}{\partial y}
=-rv.
$$

The variables are:

- $\eta(x,y,t)$: water-surface displacement;
- $u(x,y,t)$: depth-averaged velocity in the $x$ direction;
- $v(x,y,t)$: depth-averaged velocity in the $y$ direction;
- $H$: constant undisturbed water depth;
- $g$: gravitational acceleration; and
- $r$: a small damping coefficient.

The model captures propagation, reflection, interference, and depth-averaged currents for small-amplitude long waves. Solid grid cells will represent harbor walls and the breakwater. A damping zone near the offshore boundary will reduce artificial reflection from the edge of the computational domain.

## 9. Wave Scenarios

Two forms of incident-wave forcing will be used.

### 9.1 Periodic long waves

Three sinusoidal inputs will represent different incident periods:

- a relatively short long-period wave;
- a medium-period wave; and
- a period selected near a strong response of the harbor.

These experiments will be used to observe standing waves, reflection, and resonance-like amplification.

### 9.2 Tsunami-like pulse

A smooth, non-breaking elevation pulse will provide a repeatable transient disturbance:

$$
\eta(t)=A\exp\left[-\frac{(t-t_c)^2}{2\sigma^2}\right],
$$

where $A$ is the pulse amplitude, $t_c$ is its center time, and $\sigma$ controls its duration. This pulse is a controlled long-wave test case rather than a model of tsunami generation, coastal inundation, or wave breaking.

## 10. Numerical Method

The governing equations will be discretized on a regular two-dimensional grid using a finite-difference method. The implementation will advance water elevation and horizontal velocity through time while applying solid-wall, incident-wave, and damping-zone boundary conditions.

The time step will satisfy an appropriate Courant–Friedrichs–Lewy stability restriction based on the shallow-water wave speed

$$
c=\sqrt{gH}.
$$

The first implementation will favor clarity and correctness over advanced numerical features. Grid resolution and time-step sensitivity will be measured before the design sweep is performed.

## 11. Resilience Metrics

Every simulation will record:

- maximum water elevation in the protected harbor region;
- maximum current speed through the entrance;
- time-averaged wave energy inside the harbor;
- duration of elevated wave activity; and
- spatial location of the strongest amplification.

Relevant quantities will be normalized against a reference harbor configuration. For example, normalized peak elevation will be calculated as

$$
A_{\text{elevation}}
=
\frac{\text{peak elevation for the candidate design}}
{\text{peak elevation for the reference design}}.
$$

A value below one indicates lower peak elevation than the reference case under the same forcing.

## 12. Design Evaluation

A transparent composite score will support comparison without hiding the individual measurements:

$$
J(d,s)
=
w_1 E_{\text{peak}}
+ w_2 V_{\text{entrance}}
+ w_3 E_{\text{average}},
$$

where:

- $d$ denotes a design;
- $s$ denotes a wave scenario;
- $E_{\text{peak}}$ is normalized peak interior elevation;
- $V_{\text{entrance}}$ is normalized maximum entrance-current speed; and
- $E_{\text{average}}$ is normalized average interior wave energy.

Equal weights will be used initially, and every component will also be reported separately. The robust score for a design will be its worst score across the tested scenarios:

$$
J_{\text{robust}}(d)=\max_s J(d,s).
$$

This criterion favors configurations that perform consistently instead of a design that performs exceptionally under only one wave condition.

## 13. Parallel Computing Strategy

Each design-and-wave combination can be simulated independently. Parallelism will therefore be applied to the outer experiment loop rather than initially dividing a single numerical grid among threads.

Two implementations will be compared:

1. **Sequential sweep:** all experiments execute one after another.
2. **OpenMP sweep:** experiments are assigned to multiple CPU threads.

Dynamic scheduling will be evaluated because different cases may not require identical execution time. Both implementations will use the same solver, input parameters, and output calculations. Their numerical results must agree within a defined floating-point tolerance.

Performance will be measured using

$$
S_p=\frac{T_1}{T_p}
$$

for speedup and

$$
E_p=\frac{S_p}{p}
$$

for parallel efficiency, where $T_1$ is sequential runtime, $T_p$ is runtime with $p$ threads, and $p$ is the thread count.

## 14. Verification and Validation Plan

The prototype will be evaluated with the following checks:

1. **Wave-speed verification:** compare the measured propagation speed in an empty domain with $\sqrt{gH}$.
2. **Stability testing:** confirm that the selected time step produces a bounded numerical solution.
3. **Grid refinement:** compare key measurements at multiple spatial resolutions.
4. **Time-step sensitivity:** confirm that reducing the time step does not materially change the conclusions.
5. **Symmetry check:** verify that mirrored geometries produce approximately mirrored solutions under symmetric forcing.
6. **Sequential-parallel agreement:** confirm that sequential and OpenMP sweeps produce equivalent metrics.
7. **Conservation monitoring:** track numerical mass change when boundary forcing and damping permit a meaningful comparison.

These tests establish that observed differences among designs arise from the modeled geometry rather than an unstable or inconsistent implementation.

## 15. Scientific Visualization

The final repository will include:

- animated water-elevation heatmaps;
- current-speed maps or velocity-vector plots;
- side-by-side animations of the strongest and weakest designs;
- a heatmap of resilience score by entrance position and width;
- peak elevation and entrance current by design;
- sequential and parallel runtime graphs;
- OpenMP speedup and efficiency graphs; and
- solver verification and grid-refinement plots.

All comparisons will use consistent color scales and axes where appropriate.

## 16. Implementation Plan

### Phase 1: Sequential solver

- Implement the numerical grid and state variables.
- Add incident-wave and boundary conditions.
- Verify wave speed and numerical stability.

### Phase 2: Harbor geometry

- Add harbor walls and a breakwater mask.
- Generate entrance position and width variants.
- Define the protected region and entrance measurement cells.

### Phase 3: Experiment framework

- Implement periodic and pulse forcing.
- Calculate and export resilience metrics.
- Run all designs sequentially and rank the results.

### Phase 4: Parallel evaluation

- Parallelize the experiment sweep with OpenMP.
- Verify agreement with the sequential implementation.
- Benchmark multiple thread counts and scheduling policies.

### Phase 5: Visualization and documentation

- Generate animations, heatmaps, and performance plots.
- Document build and execution commands.
- Add example configurations and reproducible experiment scripts.
- Record results, conclusions, and limitations.

## 17. Minimum, Target, and Stretch Scope

### Minimum viable project

- stable sequential solver;
- constant-depth rectangular harbor;
- five entrance positions and three widths;
- three periodic waves and one tsunami-like pulse;
- peak elevation, entrance-current, and wave-energy measurements;
- basic solver verification; and
- one best-versus-worst visualization.

### Target project

- complete sequential and OpenMP parameter sweeps;
- robust multi-scenario design ranking;
- runtime, speedup, and efficiency analysis;
- automated plots and animations;
- grid-refinement and time-step sensitivity results; and
- reproducible repository documentation.

### Optional stretch goals

- variable but synthetic bathymetry;
- a second breakwater arrangement;
- parallelization within an individual simulation;
- additional objective-weight sensitivity analysis; or
- comparison of static and dynamic OpenMP scheduling.

Stretch work will begin only after the target implementation has been verified.

## 18. Expected Results

The experiments are expected to show that entrance geometry changes both interior elevation and current speed. Narrow entrances may reduce wave penetration for some scenarios while concentrating flow through the opening. The design that minimizes peak elevation may therefore differ from the design that minimizes entrance currents or worst-case multi-scenario performance.

The OpenMP sweep is expected to provide measurable speedup because the simulations are independent. Near-linear scaling is not assumed: scheduling overhead, memory bandwidth, finite experiment count, and workload differences may reduce efficiency as additional threads are used.

The project will remain scientifically useful if the physical or computational hypotheses are not supported. Negative or unexpected results will be documented and interpreted using the recorded numerical and performance evidence.

## 19. Limitations and Responsible Use

HarborTune uses an idealized constant-depth, linear, depth-averaged model. The initial implementation does not include:

- real bathymetry or coastline geometry;
- nonlinear wave behavior;
- wave breaking;
- wetting and drying or land inundation;
- tides or wind forcing;
- sediment transport and erosion;
- debris interaction;
- structural loads or failure;
- vessel response; or
- regulatory and economic constraints.

Accordingly, HarborTune may demonstrate computational trends and compare controlled hypothetical layouts, but it cannot recommend a real construction plan or certify tsunami safety. Real harbor design requires site-specific data, higher-fidelity numerical models, physical experiments, uncertainty analysis, and review by qualified coastal and structural engineers.

## 20. Planned Deliverables

The GitHub repository will contain:

- documented simulation source code;
- sequential and OpenMP execution modes;
- build configuration and dependency instructions;
- example harbor and wave configurations;
- automated experiment scripts;
- verification tests;
- tabulated numerical results;
- performance measurements;
- plots and animations;
- a description of the numerical method; and
- conclusions, limitations, and reproducibility instructions.

## 21. Proposed Repository Structure

```text
HarborTune/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── config/
│   ├── harbor_cases/
│   └── wave_cases/
├── include/
├── src/
├── tests/
├── scripts/
├── results/
│   ├── data/
│   ├── figures/
│   └── animations/
└── docs/
```

Generated data will be separated from source code. Small reference results may be committed for reproducibility, while large temporary outputs will be excluded from version control.

## 22. Reproducibility Requirements

The final repository will document:

- compiler and dependency versions;
- build commands;
- sequential and OpenMP run commands;
- configuration-file formats;
- numerical parameters and stability conditions;
- commands for reproducing the principal experiments;
- commands for regenerating figures and animations; and
- the hardware used for performance measurements.

Every reported benchmark will identify the grid size, simulation duration, number of experiments, thread count, scheduling policy, and number of repeated timing trials.

## 23. Schedule

| Milestone | Target |
|---|---|
| Confirm project scope and obtain instructor approval | November 20, 2026 |
| Create preliminary repository and sequential solver | Before the informal presentation |
| Present problem, method, and expected results | November 30 or December 2, 2026 |
| Complete geometry generation and design sweep | Early December 2026 |
| Complete OpenMP implementation and benchmarks | Early December 2026 |
| Finalize validation, visualization, and documentation | Before December 11, 2026 |
| Submit the GitHub repository URL | December 11, 2026 |

## 24. Broader Impact

HarborTune demonstrates how numerical simulation and parallel computing can support early-stage comparison of engineering concepts. Its educational value extends beyond harbor design: the same workflow—define a model, verify it, sweep a design space, parallelize independent experiments, and visualize tradeoffs—applies to many scientific and engineering problems.

The project also illustrates the importance of reporting model assumptions and limitations. Visual plausibility alone is not evidence of predictive accuracy; credible scientific software requires verification, reproducibility, and careful interpretation.

## 25. References

1. OpenMP Architecture Review Board, “OpenMP Application Programming Interface.” <https://www.openmp.org/specifications/>
2. NOAA Center for Tsunami Research, “Tsunami Modeling and Research.” <https://nctr.pmel.noaa.gov/model.html>
3. NOAA Center for Tsunami Research, “Benchmark Problems.” <https://nctr.pmel.noaa.gov/benchmark/index.html>
4. R. J. LeVeque, *Finite Volume Methods for Hyperbolic Problems*. Cambridge University Press, 2002.

