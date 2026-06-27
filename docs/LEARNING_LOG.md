# Learning Log

A running journal of this project as a learning vehicle (C++, computer graphics,
and molecular/subatomic physics). Newest entries on top. Maintained together by
Matei and Claude — see [`CLAUDE.md`](../CLAUDE.md) for how we work.

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