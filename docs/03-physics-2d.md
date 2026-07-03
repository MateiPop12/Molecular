# 03 — Physics: 2D molecular dynamics

Location: `Molecular/src/Molecular/Physics`. This is the most developed part of
the project and is what runs by default (driven by `Sandbox2D`).

It is a classical **molecular dynamics** (MD) simulation: point-like atoms in 2D
move under pairwise forces, integrated over fixed time steps, with collisions,
covalent bonding, and energy bookkeeping.

## Components

| File                       | Role                                                            |
|----------------------------|-----------------------------------------------------------------|
| `AtomData.h`               | Element property table (`elementData`)                          |
| `Atom.{h,cpp}`             | A single atom: state + per-element properties + bonding         |
| `BoundingBox.h`            | Axis-aligned 2D simulation bounds                               |
| `ForceCalculator.{h,cpp}`  | Pairwise forces + energy + collision response                  |
| `Integrator.{h,cpp}`       | Numerical integration schemes                                   |
| `SimulationSpace.{h,cpp}`  | Owns the atoms, runs the step, tracks bonds + energy history    |

## Element data (`AtomData.h`)

Each supported element carries physically-motivated parameters:

| Element | Mass (amu) | vdW radius (nm) | Bond length (nm) | ε (eV) | σ (nm) | Electronegativity | Valence |
|---------|-----------:|----------------:|-----------------:|-------:|-------:|------------------:|--------:|
| H       | 1.008      | 0.12            | 0.074            | 0.028  | 0.2958 | 2.20              | 1       |
| O       | 15.999     | 0.152           | 0.121            | 0.095  | 0.3165 | 3.44              | 2       |
| C       | 12.011     | 0.17            | 0.154            | 0.120  | 0.34   | 2.55              | 4       |
| N       | 14.007     | 0.155           | 0.145            | 0.070  | 0.325  | 3.04              | 3       |

An `Atom` can be constructed either from raw parameters or by element symbol
(which looks the values up in `elementData`).

## Forces (`ForceCalculator`)

Two pairwise forces are summed for every atom pair:

### Van der Waals — Lennard-Jones

```
F = 48·ε·[ (σ/r)¹² − ½·(σ/r)⁶ ] / r²   ·  r̂
```

with combining rules `ε = (εₐ+ε_b)/2`, `σ = (σₐ+σ_b)/2`, a small softening
term added to `r`, and force-magnitude clamping to `±m_maxForce` (default
`1e3`) for numerical stability.

> See also the learning note on the
> [Lennard-Jones potential](learning/physics/lennard-jones.md) — including the
> observation that the arithmetic ε mixing deviates from the standard
> Berthelot rule (geometric mean).

### Coulomb — electrostatic

```
F = kₑ·q₁·q₂ / (εᵣ·(r+softening)²)   ·  r̂
```

with `kₑ = 8.9875…e9 N·m²/C²`, charges expressed in elementary-charge units,
vacuum permittivity (`εᵣ = 1`), and the same clamping.

`CalculateTotalForce` loops over all other atoms, sums both contributions, and
clamps the resulting vector.

### Energy

- **Kinetic:** `Σ ½·m·v²`
- **Potential:** pairwise Lennard-Jones `Σ 4·ε·[(σ/r)¹² − (σ/r)⁶]`
- **Total:** kinetic + potential — used for the energy-conservation plots.

### Collisions

`HandleCollision` does impulse-style reflection (`v' = v − 2(v·n)n`), applies an
**energy-loss factor** (restitution, default `0.9`), and separates overlapping
atoms. Minimum distance depends on whether the pair is bonded (covalent bond
length) or not (van der Waals radius × 0.9).

## Integration (`Integrator`)

`IntegrationMethod` selects the scheme:

- `Euler`
- `RungeKutta4`
- `LeapFrog`
- `Verlet`
- `VelocityVerlet` *(default in the parameterized constructor)*

Plus optional **adaptive time stepping** (min/max dt, error tolerance) and
boundary-collision handling against the `BoundingBox` (with restitution).

> Note: `SimulationSpace`'s default constructor uses `RungeKutta4`; passing a
> method explicitly lets you pick another. The fixed app step is `1e-3 s`.

## Orchestration (`SimulationSpace`)

`SimulationSpace` is the public face of the 2D simulation:

- **Object management:** `AddObject`, `ClearAllAtoms`, `GetObjects`.
- **Lifecycle:** `StartSimulation` / `StopSimulation` / `ResetSimulation`,
  `SaveInitialState` / `ResetToInitialPositions` (snapshots the initial layout
  so a run can be replayed).
- **Step:** `Update(Timestep, BoundingBox)` integrates every atom each frame
  (only while running).
- **Bonds:** `UpdateBonds`, `GetTotalBondCount`, `GetBondPairs` — covalent bonds
  form/break based on distance, valence, and electronegativity (see
  `Atom::TryFormBond` / `ShouldBreakBondWith`).
- **Energy tracking:** records total energy every few steps into
  `m_energyHistory` / `m_timeHistory`, with `ExportEnergyDataToCSV` to dump the
  series for analysis.

## The 2D scene (`Sandbox2D`)

`Sandbox/src/Sandbox2D.cpp` wires the simulation to the UI and renderer:

- **Render** (`OnUpdate`): steps the simulation, draws the bounding-box walls as
  quads and each atom as a colored circle via `Renderer2D`.
- **Control panel** (`OnImGuiRender`): start/stop/reset, choose integration
  method, energy-loss factor, add/remove atoms per element, and **molecule
  presets**:
  - `SetupH2OSimulation()` — water
  - `SetupO3Simulation()` — ozone
  - `SetupCH4Simulation()` — methane
- **Spawning:** `AddRandomAtom`, `GenerateRandomPosition`, `IsPositionValid`
  place atoms without initial overlap inside the bounding box.