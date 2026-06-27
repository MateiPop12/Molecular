# Molecular — Documentation

**Molecular** is a real-time molecular and subatomic physics simulator built on a
custom C++ / OpenGL engine. It is a bachelor's thesis project (*proiect de licență*).

The repository is a single CMake workspace containing two products:

| Target      | Kind             | Role                                                        |
|-------------|------------------|-------------------------------------------------------------|
| `Molecular` | Static library   | The engine: windowing, rendering, events, ImGui, physics    |
| `Sandbox`   | Executable       | The application that drives a simulation through the engine  |

The engine architecture is modeled closely on TheCherno's
[Hazel](https://github.com/TheCherno/Hazel) engine (layer stack, event system,
renderer abstraction, `MOL_` macros).

## Documentation index

| Document                                        | Contents                                                      |
|-------------------------------------------------|---------------------------------------------------------------|
| [01 — Architecture](01-architecture.md)         | Repository layout, module map, the application loop           |
| [02 — Engine](02-engine.md)                     | Core, events, renderer, platform layers                       |
| [03 — Physics: 2D molecular dynamics](03-physics-2d.md) | Atoms, forces, integrators, bonding, energy            |
| [04 — Physics: 3D subatomic](04-physics-3d.md)  | Subatomic particles, Coulomb + Yukawa forces                  |
| [05 — Building & running](05-building-and-running.md)   | CMake build, configurations, how to run               |
| [06 — Status & roadmap](06-status-and-roadmap.md)       | What works today, known issues, next steps            |

## At a glance

- **Language / standard:** C++17
- **Build system:** CMake (≥ 3.24)
- **Platform:** Windows / MSVC (only platform implemented)
- **Graphics:** OpenGL (via glad), GLFW windowing
- **UI:** Dear ImGui
- **Math:** glm (double precision used for physics)
- **Logging:** spdlog
- **Vendored libs:** glfw, glad, glm, imgui, spdlog, stb_image

## Two simulation domains

1. **2D molecular dynamics** (`Molecular/src/Molecular/Physics`) — the mature part.
   Atoms of H, O, C, N interacting via Lennard-Jones and Coulomb forces, with
   covalent bonding, several numerical integrators, energy tracking, and CSV
   export. Driven by `Sandbox2D` with a full ImGui control panel.

2. **3D subatomic** (`Molecular/src/Molecular/Physics3D`) — newer, in progress.
   Protons / neutrons / electrons at the femtometre scale, interacting via
   Coulomb and the Yukawa strong-force approximation, integrated with
   Velocity-Verlet. Rendering scaffolding exists in `Sandbox3D`; the physics is
   not yet wired into the render loop.

> Only `Sandbox2D` is active by default — see `Sandbox/src/Sandbox.cpp`.