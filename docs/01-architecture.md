# 01 — Architecture

## Repository layout

```
Molecular/                         (repo root = CMake workspace)
├── CMakeLists.txt                 Workspace: configures vendor libs + targets
├── README.md                      One-line thesis title
├── docs/                          This documentation
│
├── Molecular/                     ── ENGINE (static library) ──
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── molpch.h / .cpp        Precompiled header
│   │   ├── Molecular.h            Umbrella header the app includes
│   │   ├── Molecular/
│   │   │   ├── Core/              App, Layer, LayerStack, Log, Input, Window, Timestep
│   │   │   ├── Events/            Event base + Application/Key/Mouse events
│   │   │   ├── ImGui/             ImGuiLayer + ImGui build unit
│   │   │   ├── Renderer/          API-agnostic renderer (2D, 3D, shaders, cameras)
│   │   │   ├── Physics/           2D molecular dynamics
│   │   │   └── Physics3D/         3D subatomic particles
│   │   └── Platform/
│   │       ├── OpenGL/            OpenGL implementations of renderer interfaces
│   │       └── Windows/           Win32 window + input
│   └── vendor/                    glfw, glad, glm, imgui, spdlog, stb_image
│
└── Sandbox/                       ── APPLICATION (executable) ──
    ├── CMakeLists.txt
    ├── assets/
    │   ├── shaders/               FlatColor, Texture, Sphere, Box (.glsl)
    │   └── textures/              Checkerboard.png, ChernoLogo.png
    └── src/
        ├── Sandbox.cpp            Entry point; selects the active layer
        ├── Sandbox2D.{h,cpp}      2D molecular dynamics UI + scene (ACTIVE)
        └── Sandbox3D.{h,cpp}      3D subatomic viewport (WIP)
```

## Module map

```
                ┌──────────────────────────────────────────┐
                │              Sandbox (exe)                 │
                │   Sandbox.cpp → PushLayer(Sandbox2D)       │
                │   Sandbox2D / Sandbox3D  (Molecular::Layer)│
                └───────────────┬────────────────────────────┘
                                │ links
                ┌───────────────▼────────────────────────────┐
                │            Molecular (static lib)           │
                │                                             │
                │  Core ── App / LayerStack / Window / Log    │
                │  Events ── dispatcher + event types         │
                │  ImGui ── ImGuiLayer (overlay)              │
                │  Renderer ── Renderer / Renderer2D /        │
                │              Renderer3D / Shader / Camera    │
                │       │ (RendererAPI abstraction)           │
                │  Physics   ── 2D molecular dynamics         │
                │  Physics3D ── 3D subatomic particles        │
                └───────────────┬─────────────────────────────┘
                                │ implemented by / uses
        ┌───────────────────────┼───────────────────────────────┐
        │ Platform/OpenGL        │ Platform/Windows               │ vendor/
        │ (glad GL calls)        │ (GLFW window + input)          │ glfw glad glm
        │                        │                                │ imgui spdlog stb
        └────────────────────────┴────────────────────────────────┘
```

## The application loop

`Molecular::App` is a singleton (`App::Get()`) created by the
`Molecular::CreateApplication()` factory, which `Sandbox.cpp` implements. The
entry point lives in `Core/EntryPoint.h`.

`App::Run()` (`Molecular/src/Molecular/Core/App.cpp`) uses a **fixed-timestep
accumulator** so physics is stepped deterministically regardless of frame rate:

```cpp
const float fixedTimestep = 0.001f;   // 1 ms physics step
float accumulator = 0.0f;
while (m_running) {
    Timestep ts = now - lastFrameTime;
    accumulator += ts.GetSeconds();
    while (accumulator >= fixedTimestep) {       // step physics in fixed slices
        for (Layer* layer : m_layerStack)
            layer->OnUpdate(fixedTimestep);
        accumulator -= fixedTimestep;
    }
    m_ImGuiLayer->Begin();                        // UI rendered once per frame
    for (Layer* layer : m_layerStack)
        layer->OnImGuiRender();
    m_ImGuiLayer->End();
    m_window->OnUpdate();                         // swap buffers + poll events
}
```

### Layer stack

The engine follows Hazel's **layer stack** pattern. A `Layer` exposes
`OnAttach`, `OnDetach`, `OnUpdate(Timestep)`, `OnImGuiRender`, and
`OnEvent`. Layers update front-to-back; events propagate back-to-front and stop
once a layer marks the event handled. The `ImGuiLayer` is pushed as an overlay
(always on top).

The whole simulation lives inside one layer (`Sandbox2D` or `Sandbox3D`).

## Conventions

- **Namespace:** everything in the engine lives in `Molecular::`.
- **Smart-pointer aliases:** `Ref<T>` = `std::shared_ptr<T>`, `Scope<T>` =
  `std::unique_ptr<T>` (see `Core/Core.h`).
- **Macros:** `MOL_` prefix — `MOL_CORE_INFO`, `MOL_CORE_ASSERT`,
  `MOL_BIND_EVENT_FN`, platform define `MOL_PLATFORM_WINDOWS`, config defines
  `MOL_DEBUG` / `MOL_RELEASE` / `MOL_DIST`.
- **Precision:** physics uses **double precision** (`glm::dvec2` / `dvec3`);
  rendering downcasts to `float`.