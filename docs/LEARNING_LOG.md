# Learning Log

A running journal of this project as a learning vehicle (C++, computer graphics,
and molecular/subatomic physics). Newest entries on top. Maintained together by
Matei and Claude — see `CLAUDE.md` (local, gitignored) for how we work.

Legend: 🧩 exercise · 🔬 physics · 🎨 graphics/architecture · 🦀 Rust/Zig seed

---

## Template (copy for each session)

```
## YYYY-MM-DD — <short title>
**Did:** what we built/changed.
**Learned:** concepts that clicked.
**Exercises:** 🧩 attempted / completed.
**Open questions:** things to revisit.
**Next:** the next concrete step.
```

---

## 2026-07-03 — Knowledge-base site (MkDocs Material), exercises 001–002

**Did:**
- Decided the interaction workflow: learning content lives in a local **MkDocs
  Material** site (searchable, MathJax formulas, media/links), chat stays terse.
  Set it up: `mkdocs.yml`, venv (`.venv/`, gitignored), learning hub at
  `docs/learning/`. Run with `.\.venv\Scripts\mkdocs.exe serve`.
- Wrote the first physics note: `docs/learning/physics/lennard-jones.md` —
  including a finding: `ForceCalculator.cpp` mixes ε with an **arithmetic** mean
  where the Berthelot rule is **geometric** (overestimates cross-species
  attraction).
- Resurrected the lost asset-helper exercise as a proper page.

**Learned:**
- 🔬 LJ deep-dive: where r⁻⁶ (London dispersion, derived) vs r⁻¹² (computational
  convenience) come from; force-over-r trick avoiding `normalize()`;
  Lorentz–Berthelot mixing rules.

**Exercises:** 🧩 **001 — doctest test target** (open) · 🧩 **002 — asset path
helper** (open, blocked on 001). Specs in `docs/learning/exercises/`.

**Open questions:**
- ε-mixing: switch to geometric mean or document the deviation? (thesis-relevant)
- `Molecular/vendor/imgui` has an untracked-by-upstream `CMakeLists.txt` staged
  *inside the submodule* — the root build depends on it, so **fresh clones are
  broken**. Move it out (e.g. build ImGui sources from `Molecular/CMakeLists.txt`
  or a sibling dir).

**Next:**
- Matei: push the 3 pending commits; commit the Sandbox3D layer toggle; do 001.
- Fix the imgui CMakeLists placement.
- Wire `Physics3D` particles into `Sandbox3D::OnUpdate` (spheres, not boxes).

---

## 2026-06-27 — Project documentation, asset-path fix, learning setup

**Did:**
- Read through the whole project and wrote full documentation under `docs/`
  (architecture, engine, 2D physics, 3D physics, build, status/roadmap) and
  linked it from the root `README.md`.
- Removed the hardcoded `D:/FACULTATE/...` asset paths from `Sandbox.cpp`,
  `Renderer2D.cpp`, and `Renderer3D.cpp`, replacing them with a CMake-defined
  `MOL_ASSETS_DIR` macro. Rebuilt `Sandbox` successfully to confirm.
- Set up this learning system: working agreement in `CLAUDE.md` + this log +
  GitHub milestones for bigger goals.

**Learned:**
- 🎨 How the engine is layered (Hazel heritage): `App` → `LayerStack` →
  `Renderer` (API-abstracted) → `Platform/OpenGL`. Fixed-timestep main loop.
- A CMake `target_compile_definitions(... PUBLIC MOL_ASSETS_DIR="...")` bakes a
  path into the binary at configure time; in C++ it's a string literal, so
  `MOL_ASSETS_DIR "/shaders/x.glsl"` concatenates at compile time.

**Exercises:** _(see session reply — starter exercise on `MOL_ASSETS_DIR` helper)_

**Open questions:**
- Should the engine (`Molecular`) really know about the app's (`Sandbox`) assets?
  (Layering smell — fine for now.)

**Next:**
- Commit the pending work in logical chunks (docs / asset-fix / 3D package).
- Wire `Physics3D` particles into `Sandbox3D::OnUpdate` and render them as
  spheres (currently only placeholder boxes).