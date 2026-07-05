# 🧩 Exercise 004 — Wire Physics3D into Sandbox3D

**Status:** 🔴 open · assigned 2026-07-05 · do after [003](003-imgui-build-fix.md)
**Concepts:** ownership & RAII *(re-tests 001)* · **unit consistency in simulation** · fixed-timestep loops · owner vs. observer pointers

## Motivation

The subatomic core (`Physics3D/`) compiles, has CODATA-correct particle data,
Coulomb + Yukawa forces, and a Velocity-Verlet integrator — and *nothing runs
it*. `Sandbox3D::OnUpdate` draws placeholder boxes. This is roadmap item #1.

And there's a bug waiting for you. **The integrator is not unit-consistent:**
positions live in femtometres, but `VelocityVerlet` adds `v·dt + ½a·dt²`
computed from accelerations in m/s² — *metre* displacements added straight
onto *femtometre* positions, off by a factor of 10¹⁵. Any simulation wired to
it today would look frozen. The provided tests catch it; fixing it is part
of this exercise.

## Spec

### A. Fix the unit convention (red → green)

Wire `tests/physics3d_tests.cpp` into `tests/CMakeLists.txt` as your first
step. Nine of the ten cases pass against today's core; **"unit consistency —
electron falls the right DISTANCE" fails.** Choose one convention (e.g.
positions fm, velocities fm/s, accelerations converted to fm/s² at the
boundary — or convert positions to metres inside the integrator), implement
it, and **document it in comments in `Integrator.h` and `PhysicsObject.h`**.
The test defines correct: an electron 100 fm from a proton falls ~1.27×10⁻³ fm
in one 10⁻²³ s step.

### B. Own the particles properly (the RAII re-test)

`Sandbox3D` owns the simulation state:

```cpp
std::vector<std::unique_ptr<SubatomicParticle>> m_particles;  // owner
std::vector<PhysicsObject*>                     m_simView;    // observer
```

The integrator API wants `std::vector<PhysicsObject*>&` — build the raw
pointer view from the owning vector (rebuild on spawn/despawn). Be ready to
defend in review: why `unique_ptr` and not values in a `vector`? (Hint:
what happens to `m_simView`'s pointers when a `std::vector<SubatomicParticle>`
grows?) Why raw pointers in the view and not `shared_ptr`?

🦀 *This split is exactly Rust's `Vec<Box<Particle>>` owner with `&mut [..]`
borrows — ownership in one place, access everywhere else.*

### C. Fixed physics timestep — derive it, don't guess it

`OnUpdate(Timestep ts)` gives you *frame* time (~16 ms). You cannot feed that
to the integrator. Add a fixed physics step and an accumulator, and **derive
the step size in a comment**: with `a = F/m` for an electron a few fm from a
proton, how big may `dt` be so one step moves it ≪ 1 fm? (You'll land around
10⁻²²–10⁻²³ s.) Then decide how many physics steps per frame you can afford
and what "simulation speed" means in the UI.

### D. Render and control

- Each particle → `Renderer3D::DrawSphere` using `GetRadius()`/`GetColor()`,
  with one named constant mapping fm → world units.
- Minimal ImGui panel: spawn proton/neutron/electron, pause/resume, reset,
  live particle count. (A full control panel mirroring 2D is *not* in scope —
  that's a later exercise.)

## Test file

`tests/physics3d_tests.cpp` — already written, **not yet wired into the test
target**; adding it to `target_sources` is step one. The cases pin: Coulomb
directions and magnitude (units!), neutron neutrality, Yukawa range behaviour
(strong beats Coulomb at 1.5 fm, negligible at 10 fm, zero for electrons),
Newton's third law, momentum conservation, and the unit-consistency fall
distance.

## Acceptance bar

- [ ] All `physics3d` test cases green — including the unit one, and you
      watched it fail first.
- [ ] Unit convention documented in the headers it governs.
- [ ] `Sandbox3D` shows moving spheres: spawn a proton and an electron and
      watch the electron accelerate toward it (softening keeps it from
      exploding at contact).
- [ ] Ownership review clean: `unique_ptr` owner + raw observer view,
      justified out loud.
- [ ] Panel: spawn/pause/reset/count work.

Suggested commits (two, in this order):
`fix: make Physics3D integrator unit-consistent` ·
`feat: wire Physics3D particles into Sandbox3D`