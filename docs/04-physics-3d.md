# 04 — Physics: 3D subatomic

Location: `Molecular/src/Molecular/Physics3D`. This is the **newer, in-progress**
half of the project (added in commit `c8fef8a`, "Added 3D physics package"). It
models individual **subatomic particles** — protons, neutrons, electrons — at
the femtometre scale, rather than whole atoms.

> Status: the data model, forces, and integrator exist and compile, but the
> physics is **not yet wired into a render loop**. `Sandbox3D` currently only
> draws boxes, and it is commented out in `Sandbox.cpp` (the 2D layer is active).
> See [06 — Status & roadmap](06-status-and-roadmap.md).

## Components

| File                          | Role                                                       |
|-------------------------------|------------------------------------------------------------|
| `ParticleTypeData.h`          | Particle enum + CODATA physical constants                  |
| `PhysicsObject.{h,cpp}`       | Base particle: position, velocity, mass, charge (`dvec3`)  |
| `SubatomicParticle.{h,cpp}`   | Adds render radius + color on top of `PhysicsObject`       |
| `ForceCalculator3D.{h,cpp}`   | Coulomb + Yukawa strong-force                              |
| `Integrator.{h,cpp}`          | Velocity-Verlet integration                                |

> Working-tree note: the git status shows `Physics3D/ForceCalculator.{h,cpp}`
> being renamed to `ForceCalculator3D.{h,cpp}`, disambiguating it from the 2D
> `ForceCalculator`. Keep that in mind when cross-referencing older commits.

## Particle data (`ParticleTypeData.h`)

```cpp
enum class ParticleType : uint8_t { PROTON, NEUTRON, ELECTRON };
```

Each type stores CODATA 2018/2022 constants: mass (kg and MeV/c²), charge (C),
spin (stored as spin×2), magnetic moment, and lifetime (∞ for stable particles;
the neutron is ~880.2 s).

| Particle | Mass (kg)        | Mass (MeV/c²) | Charge (C)        | Lifetime |
|----------|------------------|---------------|-------------------|----------|
| Proton   | 1.67262192369e-27| 938.272       | +1.602176634e-19  | stable   |
| Neutron  | 1.67492749804e-27| 939.565       | 0                 | 880.2 s  |
| Electron | 9.1093837015e-31 | 0.511         | −1.602176634e-19  | stable   |

## State model

- `PhysicsObject` holds `glm::dvec3` position/velocity, mass, and a pointer to
  its `ParticleTypeData` (charge comes from there). Positions are expressed in
  **femtometres** (fm = 1e-15 m).
- `SubatomicParticle` extends it with a render `radius` and `color`, chosen per
  particle type.

## Forces (`ForceCalculator3D`)

Constructed with a **softening length** `epsilon_fm` (default 0.1 fm) that keeps
forces finite when two particles nearly coincide.

### Coulomb (electrostatic)

`CalculateCoulombForce(a, b)` — standard inverse-square electrostatic force in
Newtons, with positions in fm and the softening length applied.

### Strong nuclear force (Yukawa approximation)

`CalculateStrongForce(a, b)` — a [Yukawa potential](https://en.wikipedia.org/wiki/Yukawa_potential)
approximation of the residual strong force, significant only at ranges of
~2–3 fm. This is what allows protons and neutrons to bind despite Coulomb
repulsion.

`CalculateTotalForce` = Coulomb + strong.

## Integration (`Integrator`)

`VelocityVerlet(objects, deltaTime, forceCalc)` advances all particles one step
(`deltaTime` in seconds). Acceleration is computed by summing pairwise forces
from every other particle and dividing by mass. `Reset()` clears integrator
state. Velocity-Verlet is chosen for its good energy-conservation properties,
which matter for bound (orbiting/oscillating) systems.

## The 3D scene (`Sandbox3D`)

`Sandbox/src/Sandbox3D.cpp`:

- Renders into an offscreen `Framebuffer` shown as an ImGui viewport.
- Uses an `EditorCamera` (orbit/fly) for navigation.
- Currently calls `Renderer3D` to draw boxes (`DrawSpace`) — a placeholder
  scene. Hooking the `Physics3D` particles + forces into `OnUpdate` and drawing
  them as spheres is the natural next step.