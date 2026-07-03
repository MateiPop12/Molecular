# 🧩 Exercise 001 — Set up the doctest test target

**Status:** ✅ done · assigned & completed 2026-07-03
**Concepts:** CMake target graph · CTest · doctest · red→green discipline

Every future exercise ships as compiled doctest tests, so the harness itself is
the first exercise. When this is done, the project has a `MolecularTests`
executable that CTest knows about, and "run the tests" is one command.

## Spec

1. **Vendor doctest.** Download the single header from the
   [doctest v2.4.12 release](https://github.com/doctest/doctest/releases/tag/v2.4.12)
   (file `doctest.h`) and put it at `tests/vendor/doctest/doctest.h`. It is one
   header, ~7k lines, zero dependencies — that's the whole framework.

2. **Create the target.** Write `tests/CMakeLists.txt` defining an executable
   `MolecularTests` from the sources below, linking against the `Molecular`
   library so tests can exercise engine code:
   - `tests/main.cpp`
   - `tests/sanity_tests.cpp`

3. **Wire it into the workspace.** In the root `CMakeLists.txt`:
   - `enable_testing()` (must be at the root, *before* the subdirectory — find
     out why, it's part of the exercise)
   - `add_subdirectory(tests)`

4. **Register with CTest.** In `tests/CMakeLists.txt`, `add_test(...)` so that
   `ctest` discovers and runs the executable.

## Files you get

`tests/main.cpp` — doctest supplies `main()` for you:

```cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "vendor/doctest/doctest.h"
```

`tests/sanity_tests.cpp` — proves the harness works:

```cpp
#include "vendor/doctest/doctest.h"

#include "Molecular/Physics/Vec2D.h" // adjust to a real engine header

TEST_CASE("sanity: the harness runs")
{
    CHECK(1 + 1 == 2);
}

TEST_CASE("sanity: engine headers are reachable")
{
    // Construct any cheap engine type here to prove the include paths
    // and link line are correct. Pick something from Physics/.
    CHECK(true);
}
```

Replace the placeholder include/type with a real one from `Molecular/src` —
part of the exercise is discovering what include directories the `Molecular`
target exports, and adding `target_include_directories` if it doesn't export
any.

## Acceptance bar

- [ ] Fresh CMake reconfigure succeeds (CLion profile, VS 2022 generator).
- [ ] `MolecularTests` builds with no warnings *you introduced*.
- [ ] Running `ctest` from the build directory reports the tests passing.
- [ ] **Red check:** temporarily change a `CHECK` to something false, watch it
      fail with a useful message, revert. If you've never seen it red, you
      don't know it works.
- [ ] `tests/` is committed (including the vendored header), suggested title:
      `build: add doctest test target`.

## Hints (use sparingly)

??? hint "Where does `enable_testing()` go and why?"
    `enable_testing()` only affects the directory it's called in *and below*.
    Called inside `tests/`, the root build directory gets no `CTestTestfile.cmake`,
    and `ctest` run from the build root finds zero tests.

??? hint "Link errors mentioning GLFW/OpenGL?"
    `Molecular` links its vendor libs. If they're `PRIVATE`, your test target
    doesn't inherit them and only needs `Molecular` itself. If you get
    unresolved externals, check how `Molecular/CMakeLists.txt` declares its
    `target_link_libraries` visibility — and reason about what `PRIVATE` vs
    `PUBLIC` means for *your* link line.

## After you're done

Ping Claude for the review pass. Then [Exercise 002](002-asset-path-helper.md)
unblocks.