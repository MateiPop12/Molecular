# 🧩 Exercise 003 — ImGui build fix (fresh clones + CRT)

**Status:** ✅ done · assigned 2026-07-05 · completed 2026-07-06
**Concepts:** CMake target definition & visibility *(re-tests 001/002 — target ≤ H1)* · vendored dependencies & submodule hygiene · MSVC CRT runtime model

## Motivation

The root build does `add_subdirectory(Molecular/vendor/imgui)` — but the
`CMakeLists.txt` in that directory **does not exist in any repository's
history**. It sits in the working tree of the imgui *submodule*, which
upstream doesn't track. Consequences, all currently live:

1. **Fresh clones cannot configure.** `git clone --recurse-submodules` checks
   out pristine upstream imgui → no `CMakeLists.txt` → CMake fails. For a
   thesis repo this is a defect, not a nicety.
2. **`git status` is permanently dirty** — that `m Molecular/vendor/imgui`
   smudge you've been seeing for two sessions *is this file*.
3. **It causes the `LNK4098` warning.** The rogue file forces
   `MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"` — the
   **static** CRT — while the root enforces `MultiThreadedDebugDLL` for every
   other target. Mixed CRTs = `LIBCMTD conflicts` at every `Sandbox.exe` link.
4. Assorted sloppiness: `imgui_widgets.cpp` is listed **twice**, and
   `-g` / `-O3` are **GCC flags** handed to MSVC, which ignores them.
5. **Include paths don't travel with the target.** Consumers find `imgui.h`
   only because `Molecular` hardcodes
   `${CMAKE_SOURCE_DIR}/Molecular/vendor/imgui` in *its own* PUBLIC include
   dirs (`Molecular/CMakeLists.txt:24-25`), and `Sandbox.cpp:7` resorts to
   `#include "../../Molecular/vendor/imgui/imgui.h"`. The target that owns
   the headers should export them — you learned exactly this mechanism when
   you flipped `MOL_ASSETS_DIR` to `PRIVATE` in 002.

## Spec

Create **`Molecular/vendor/imgui-build/CMakeLists.txt`** — a sibling
directory *tracked by this repo* — and make the submodule pristine.

Requirements:

1. Define the `ImGui` STATIC target there, listing the sources from
   `../imgui/` explicitly, **no duplicates**. Decide whether `imgui_demo.cpp`
   stays and write a one-line comment defending the choice.
2. `target_include_directories(ImGui PUBLIC ...)` pointing at the submodule
   directory (and `backends/` if the engine uses the impl files) — so any
   target that links `ImGui` inherits the include path automatically.
3. **No `MSVC_RUNTIME_LIBRARY` override.** Inherit the root's
   `MultiThreadedDebugDLL`. The `LNK4098` warning must be gone from a full
   rebuild.
4. No GCC flags. If you think a config-specific option is needed, it must be
   MSVC-valid — otherwise add nothing; the defaults are fine.
5. Root `CMakeLists.txt:20` points at `imgui-build`; the rogue file is
   deleted from the submodule tree; `git status` shows the submodule clean.
6. **Stretch:** remove the two imgui lines from `Molecular`'s PUBLIC include
   dirs and change `Sandbox.cpp` to plain `#include "imgui.h"` — if include
   propagation is wired right, this *just works*, and proving it is the
   point.

## Acceptance bar

The honest test is a fresh clone. From a scratch directory **outside** this
repo:

```powershell
git clone --recurse-submodules https://github.com/MateiPop12/Molecular molecular-fresh
cd molecular-fresh
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --target MolecularTests
```

- [x] Fresh clone configures and builds with zero manual steps.
      *(verified 2026-07-06 — clone → configure → build `MolecularTests` +
      `Sandbox`, doctest 11/11 in the fresh clone)*
- [x] `LNK4098` no longer appears when linking `Sandbox.exe` (full rebuild).
- [x] `git status` in this repo shows the imgui submodule clean — the `m`
      smudge is gone for good.
- [x] Claude's review pass is clean (3 cycles).

No doctest file for this one — like 001, the build system *is* the exercise,
and the fresh clone is the test.

Suggested commit title: `build: move ImGui target out of the submodule tree`.