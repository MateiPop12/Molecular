# 05 — Building & running

## Prerequisites

- **CMake ≥ 3.24**
- **Windows + MSVC** (the only implemented platform; the build forces an MSVC
  runtime and links `opengl32`)
- A C++17 compiler
- All third-party libraries are **vendored** under `Molecular/vendor`
  (glfw, glad, glm, imgui, spdlog, stb_image) — nothing extra to install.

## CMake structure

The root `CMakeLists.txt` defines the `MolecularWorkspace` project and:

1. Sets C++17, no compiler extensions.
2. Forces a consistent MSVC runtime (`MultiThreadedDebugDLL`) across all targets
   to avoid `LNK4098` runtime-mismatch link errors.
3. Routes build output to `bin/<Config>-<System>-<Arch>` and intermediates to
   `bin-int/...`.
4. Adds the vendor libraries that need building: `glfw`, `glad`, `imgui`.
   (`stb_image` is compiled directly into `Molecular` via a GLOB, **not** as a
   separate subdirectory — see the comments in the CMake files.)
5. Adds the `Molecular` and `Sandbox` targets.

### Targets

- **`Molecular`** — `STATIC` library. Globs `src/*.{h,cpp}` (+ stb_image),
  exposes its include dirs as `PUBLIC` so dependents inherit them, links
  `glfw glad ImGui opengl32`, uses a precompiled header (`molpch.h`), and defines
  `MOL_PLATFORM_WINDOWS`, `MOL_BUILD_DLL`, `GLFW_INCLUDE_NONE`.
- **`Sandbox`** — executable. Globs its own sources and links `Molecular`
  (inheriting all include dirs transitively).

### Build configurations

Both targets define a macro per CMake config:

| Config    | Define        |
|-----------|---------------|
| `Debug`   | `MOL_DEBUG`   |
| `Release` | `MOL_RELEASE` |
| `Dist`    | `MOL_DIST`    |

## Building

```sh
# from the repository root
cmake -S . -B build
cmake --build build --config Debug
```

The `Sandbox` executable lands under `build/bin/<Config>-Windows-<Arch>/`.

> The project is also set up to open directly in CLion / Visual Studio as a
> CMake project. `.gitignore` excludes generated `bin/`, `bin-int/`, `cmake-*/`,
> `.vs/`, `.idea/`, and the Visual Studio solution/project files.

## Running

Run the `Sandbox` executable. By default `Sandbox.cpp` pushes `Sandbox2D`, so
the 2D molecular dynamics simulator opens with its ImGui control panel.

To switch scenes, edit the `Sandbox` constructor in
`Sandbox/src/Sandbox.cpp`:

```cpp
Sandbox() {
    //PushLayer(new ExampleLayer());   // legacy renderer demo
    PushLayer(new Sandbox2D());        // 2D molecular dynamics (default)
    //PushLayer(new Sandbox3D());      // 3D subatomic viewport (WIP)
}
```

## Asset paths (`MOL_ASSETS_DIR`)

Shaders and textures are loaded through the compile-time macro **`MOL_ASSETS_DIR`**,
defined by CMake in `Molecular/CMakeLists.txt`:

```cmake
target_compile_definitions(Molecular PUBLIC
        MOL_ASSETS_DIR="${CMAKE_SOURCE_DIR}/Sandbox/assets")
```

It expands to a string literal holding the absolute path to `Sandbox/assets`,
resolved when you run CMake. Because it is `PUBLIC` on the `Molecular` target,
both the engine and `Sandbox` inherit it. Call sites concatenate onto it:

```cpp
Shader::Create(MOL_ASSETS_DIR "/shaders/Texture.glsl");
Molecular::Texture2D::Create(MOL_ASSETS_DIR "/textures/Checkerboard.png");
```

This makes asset loading work regardless of the process's working directory
(IDE debug runs, double-clicking the exe, etc.) and portable across machines:
the path is recomputed each time the project is configured, so it is correct
wherever the repository is cloned. No assets are copied — they are read straight
from the source tree.

> If you later want a fully relocatable build (one you can zip up and run on a
> machine without the source), switch to copying `assets/` next to the executable
> via a CMake `add_custom_command` and resolving paths relative to the exe at
> runtime. `MOL_ASSETS_DIR` is the lightweight, develop-on-this-machine option.