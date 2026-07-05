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

## 2026-07-05 — Exercise 002 done (Assets helper); Claude Code session skills

**Did:**
- 🧩 Completed **002 — asset path helper**: new `Molecular::Assets` API
  (`Root`/`Path`/`Exists`) in `Molecular/Core`; all call sites in
  `Renderer2D.cpp`, `Renderer3D.cpp`, `Sandbox.cpp` now resolve assets through
  it. `MOL_ASSETS_DIR` is `PRIVATE` to the Molecular target and referenced in
  exactly one TU — **compiler-enforced**, not convention. Shipped in `a94690d`.
- Built three local Claude Code skills (`.claude/skills/`, gitignored):
  `/kickoff` (start-of-session briefing), `/new-exercise` (scaffold spec + RO +
  nav + doctest file), `/serve-docs` (check/restart the MkDocs server).

**Learned:**
- A function-local `static` initializes **once, with the first call's
  argument** — right for caching argument-independent values (`Root`), a
  first-call-wins bug in argument-dependent ones (`Path` froze the first path).
- `std::filesystem::path` implicitly converts only to its *native* string type
  (`std::wstring` on Windows); `.string()` is deliberately explicit because
  narrowing depends on encoding. The same line compiles silently on Linux —
  a portability trap.
- Windows path taxonomy: absolute (`C:/x`), root-relative (`/x` — **not**
  absolute, `has_root_directory()` only), drive-relative (`C:x` — neither,
  `has_root_name()` only). `operator/` *replaces* the left side when the right
  has a root directory or a different root name.
- `||`/`&&` short-circuiting is a **sequencing guarantee**: the guard goes in
  the left operand (`check.empty() || *check.begin() == ".."`). Had UB
  (dereferencing an empty path's begin iterator) hiding behind green tests.
- **Green tests are evidence, not proof** — the drive-relative hole and the UB
  both passed the original suite; reasoning found them, not the tests.
- LNK2019 anatomy: the compiler trusts declarations per-TU, the linker pays the
  debts. Unresolved external = no definition, file missing from the link, or
  signature mismatch (mangled names encode the full signature).
- CLion debugging: doctest gutter icons, F7/F8/Shift+F8, Evaluate Expression;
  `MolecularTests.exe -s` prints expanded values even for passing CHECKs.

**Exercises:** 🧩 **002 — asset path helper** — ✅ **completed & reviewed**
(cycles: `current_path()` → macro; frozen `static`; drive-relative escape +
empty-path UB; guard ordering; `PUBLIC` → `PRIVATE` enforcement).

**Open questions:**
- imgui submodule still carries an untracked `CMakeLists.txt` → fresh clones
  broken (unchanged since 2026-07-03 — now the oldest debt on the books).
- `LNK4098: LIBCMTD conflicts` when linking `Sandbox.exe` — CRT runtime
  mismatch in a vendored lib; investigate.
- ε-mixing (arithmetic vs geometric Berthelot) in `ForceCalculator.cpp` —
  still undecided, thesis-relevant.

**Next:**
- Fix the imgui `CMakeLists.txt` placement (top candidate for next session).
- Wire `Physics3D` particles into `Sandbox3D::OnUpdate` (spheres, not boxes).
- 🧩 003 — scaffold with `/new-exercise` once the topic is picked.

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
- Made the site **bilingual (EN/RO)** via `mkdocs-static-i18n`: every page has
  a `.ro.md` sibling, language switcher in the header, untranslated pages fall
  back to English. All 11 pages translated (LEARNING_LOG stays EN-only).

**Learned:**
- 🔬 LJ deep-dive: where r⁻⁶ (London dispersion, derived) vs r⁻¹² (computational
  convenience) come from; force-over-r trick avoiding `normalize()`;
  Lorentz–Berthelot mixing rules.

**Exercises:** 🧩 **001 — doctest test target** — ✅ **completed & reviewed**
(took ~4 review cycles: link-vs-include concept, `add_test` registration, then
RAII — `new`/`delete` → explicit destructor call → stack object). 🧩 **002 —
asset path helper** (open, now unblocked). Specs in `docs/learning/exercises/`.

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