# 🧩 Exercise 002 — Asset path helper

**Status:** 🔴 open · assigned 2026-07-03 · **blocked on [001](001-doctest-target.md)**
**Concepts:** `std::filesystem` · `std::string_view` · API design · defensive boundaries

*(This is the exercise from the 2026-06-27 session that previously existed only
in chat — resurrected here so it can't get lost again.)*

## Motivation

Right now every asset load site concatenates the `MOL_ASSETS_DIR` string macro
by hand (`MOL_ASSETS_DIR "/shaders/x.glsl"`). That works only for literals,
scatters path logic across the codebase, and gives garbage errors when a file
is missing. Centralize it.

## Spec

Create `Molecular/src/Molecular/Core/Assets.h` / `.cpp` with:

```cpp
namespace Molecular::Assets {

    // The asset root baked in at configure time (MOL_ASSETS_DIR),
    // as a normalized absolute path.
    std::filesystem::path Root();

    // Resolve a path relative to the asset root.
    //   Path("shaders/flat.glsl") -> <root>/shaders/flat.glsl
    // Throws std::invalid_argument if `relative`:
    //   - is an absolute path, or
    //   - escapes the root after normalization (e.g. "../../secrets").
    std::filesystem::path Path(std::string_view relative);

    // True if the resolved asset exists on disk.
    bool Exists(std::string_view relative);

}
```

Requirements:

1. `MOL_ASSETS_DIR` is referenced in **exactly one** translation unit
   (`Assets.cpp`). No other file mentions the macro.
2. `Path` must reject escapes *after* normalization —
   `"shaders/../../x"` escapes even though it doesn't start with `..`.
   Look at `std::filesystem::path::lexically_normal`.
3. Replace the existing uses of `MOL_ASSETS_DIR` in `Renderer2D.cpp`,
   `Renderer3D.cpp`, and `Sandbox.cpp` with the helper.
4. Think about *why* `string_view` is the right parameter type here and be
   ready to defend it in review (what would `const std::string&` cost? what
   would `const char*` lose?).

## Test file

Drop this in as `tests/asset_path_tests.cpp` and add it to the
`MolecularTests` sources:

```cpp
#include "vendor/doctest/doctest.h"

#include "Molecular/Core/Assets.h"

#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;
using namespace Molecular;

TEST_CASE("Root is absolute and normalized")
{
    const fs::path root = Assets::Root();
    CHECK(root.is_absolute());
    CHECK(root == root.lexically_normal());
    CHECK(fs::exists(root)); // the assets dir itself must exist
}

TEST_CASE("Path resolves relative paths under the root")
{
    const fs::path p = Assets::Path("shaders/whatever.glsl");
    CHECK(p == (Assets::Root() / "shaders" / "whatever.glsl").lexically_normal());
}

TEST_CASE("Path rejects absolute inputs")
{
    CHECK_THROWS_AS((void)Assets::Path("C:/Windows/System32"), std::invalid_argument);
}

TEST_CASE("Path rejects escapes from the root")
{
    CHECK_THROWS_AS((void)Assets::Path("../outside.txt"), std::invalid_argument);
    CHECK_THROWS_AS((void)Assets::Path("shaders/../../outside.txt"), std::invalid_argument);
}

TEST_CASE("Path accepts dotdot that stays inside the root")
{
    const fs::path p = Assets::Path("shaders/../textures/x.png");
    CHECK(p == (Assets::Root() / "textures" / "x.png").lexically_normal());
}

TEST_CASE("Exists reports real files")
{
    // Adjust to any asset actually present in the repo:
    CHECK(Assets::Exists("shaders/FlatColor.glsl"));
    CHECK_FALSE(Assets::Exists("definitely/not/here.bin"));
}
```

One test references a real shader file — adjust the name to an asset that
actually exists before blaming your implementation.

## Acceptance bar

- [ ] All tests above pass (and you watched at least one fail first).
- [ ] `MOL_ASSETS_DIR` appears in exactly one `.cpp` in the whole repo
      (`git grep MOL_ASSETS_DIR` to prove it — the CMake definition doesn't count).
- [ ] Sandbox still runs and renders (assets actually load through the helper).
- [ ] Claude's review pass is clean: naming, const-correctness, no needless
      copies, sensible error messages.

Suggested commit title: `refactor: centralize asset paths in Assets helper`.