# 06 — Status & roadmap

A snapshot of what works, what's partial, and known issues — useful for the
thesis write-up and for picking up the project after a break.

## What works today

- ✅ **Engine foundation** — windowing (GLFW), OpenGL rendering, layer stack,
  event system, ImGui integration, logging. Fixed-timestep main loop.
- ✅ **2D molecular dynamics** — the complete, working simulator:
  - Atoms for H, O, C, N with realistic per-element parameters.
  - Lennard-Jones (van der Waals) + Coulomb forces with clamping.
  - Multiple integrators (Euler, RK4, LeapFrog, Verlet, Velocity-Verlet) +
    optional adaptive timestep.
  - Collisions with restitution / energy loss and boundary handling.
  - Covalent bond formation/breaking driven by valence + electronegativity.
  - Energy tracking (kinetic / potential / total) with CSV export.
  - Full ImGui control panel and molecule presets (H₂O, O₃, CH₄).
- ✅ **3D rendering primitives** — `Renderer3D` (spheres, cubes, boxes),
  `EditorCamera`, framebuffer-backed viewport in `Sandbox3D`.
- ✅ **3D subatomic physics model** — particle data (CODATA constants), Coulomb +
  Yukawa strong force, Velocity-Verlet integrator. Compiles and is unit-testable
  in isolation.

## Partial / in progress

- 🚧 **3D simulation is not wired up.** `Physics3D` has forces + an integrator,
  but `Sandbox3D::OnUpdate` only draws placeholder boxes — particles aren't
  created, stepped, or rendered as spheres yet. `Sandbox3D` is commented out in
  `Sandbox.cpp`.
- 🚧 **Naming cleanup in flight.** The working tree renames
  `Physics3D/ForceCalculator` → `ForceCalculator3D` to disambiguate from the 2D
  class. (`Physics3D/Integrator` still shares the name `Integrator` with the 2D
  integrator — a similar rename would be consistent.)

## Known issues / cleanup opportunities

- ✅ ~~**Hardcoded absolute asset paths**~~ — *resolved.* Shader/texture loads in
  `Sandbox.cpp`, `Renderer2D.cpp`, and `Renderer3D.cpp` now use the CMake-defined
  `MOL_ASSETS_DIR` macro instead of `D:/FACULTATE/Licenta/...`. See
  [05 — Building & running](05-building-and-running.md).
- 🧹 **Legacy `ExampleLayer`** (a ~200-line renderer demo with inline GLSL) still
  lives in `Sandbox.cpp`. It's the Hazel learning scaffold and could be removed
  or moved out of the main entry file.
- 🧩 **Duplicate concepts across 2D/3D** (`ForceCalculator`, `Integrator`,
  `PhysicsObject`/`Atom`). A shared physics base could reduce duplication if the
  two domains are meant to converge.
- 🖥️ **Single platform.** Only Windows + OpenGL are implemented, though the
  `RendererAPI` / `Window` abstractions are designed to allow more.

## Natural next steps

1. Wire `Physics3D` into `Sandbox3D`: spawn protons/neutrons/electrons, step the
   Velocity-Verlet integrator each frame, and render them with
   `Renderer3D::DrawSphere`.
2. Add a 3D ImGui control panel mirroring the 2D one (spawn particles, build a
   nucleus, start/stop, energy readout).
3. Trim the legacy `ExampleLayer` out of `Sandbox.cpp`.
4. Expand `README.md` and link to `docs/`.

## Commit history context

```
c8fef8a  Added 3D physics package         ← Physics3D introduced
622ca12  Added boxes in Renderer3D
c6daa80  Added 3d Camera
6401842  To many changes to count
529904b  Abstracted integration methods and force calculations
```

The trajectory is clear: a working 2D simulator first, then building out a 3D
subatomic mode on the same engine.