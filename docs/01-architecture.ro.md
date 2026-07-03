# 01 — Arhitectură

## Structura depozitului

```
Molecular/                         (rădăcina repo-ului = workspace CMake)
├── CMakeLists.txt                 Workspace: configurează bibliotecile vendor + țintele
├── README.md                      Titlul lucrării de licență
├── docs/                          Această documentație
│
├── Molecular/                     ── MOTORUL (bibliotecă statică) ──
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── molpch.h / .cpp        Header precompilat
│   │   ├── Molecular.h            Header-ul „umbrelă" inclus de aplicație
│   │   ├── Molecular/
│   │   │   ├── Core/              App, Layer, LayerStack, Log, Input, Window, Timestep
│   │   │   ├── Events/            Baza Event + evenimente Application/Key/Mouse
│   │   │   ├── ImGui/             ImGuiLayer + unitatea de compilare ImGui
│   │   │   ├── Renderer/          Renderer agnostic de API (2D, 3D, shadere, camere)
│   │   │   ├── Physics/           Dinamică moleculară 2D
│   │   │   └── Physics3D/         Particule subatomice 3D
│   │   └── Platform/
│   │       ├── OpenGL/            Implementările OpenGL ale interfețelor renderer-ului
│   │       └── Windows/           Fereastră + input Win32
│   └── vendor/                    glfw, glad, glm, imgui, spdlog, stb_image
│
└── Sandbox/                       ── APLICAȚIA (executabil) ──
    ├── CMakeLists.txt
    ├── assets/
    │   ├── shaders/               FlatColor, Texture, Sphere, Box (.glsl)
    │   └── textures/              Checkerboard.png, ChernoLogo.png
    └── src/
        ├── Sandbox.cpp            Punctul de intrare; alege layerul activ
        ├── Sandbox2D.{h,cpp}      UI + scenă dinamică moleculară 2D (ACTIV)
        └── Sandbox3D.{h,cpp}      Viewport subatomic 3D (în lucru)
```

## Harta modulelor

```
                ┌──────────────────────────────────────────┐
                │              Sandbox (exe)                 │
                │   Sandbox.cpp → PushLayer(Sandbox2D)       │
                │   Sandbox2D / Sandbox3D  (Molecular::Layer)│
                └───────────────┬────────────────────────────┘
                                │ leagă (links)
                ┌───────────────▼────────────────────────────┐
                │           Molecular (bibliotecă statică)    │
                │                                             │
                │  Core ── App / LayerStack / Window / Log    │
                │  Events ── dispatcher + tipuri de evenimente│
                │  ImGui ── ImGuiLayer (overlay)              │
                │  Renderer ── Renderer / Renderer2D /        │
                │              Renderer3D / Shader / Camera    │
                │       │ (abstractizarea RendererAPI)        │
                │  Physics   ── dinamică moleculară 2D        │
                │  Physics3D ── particule subatomice 3D       │
                └───────────────┬─────────────────────────────┘
                                │ implementat de / folosește
        ┌───────────────────────┼───────────────────────────────┐
        │ Platform/OpenGL        │ Platform/Windows               │ vendor/
        │ (apeluri GL prin glad) │ (fereastră + input GLFW)       │ glfw glad glm
        │                        │                                │ imgui spdlog stb
        └────────────────────────┴────────────────────────────────┘
```

## Bucla aplicației

`Molecular::App` este un singleton (`App::Get()`) creat de fabrica
`Molecular::CreateApplication()`, pe care o implementează `Sandbox.cpp`.
Punctul de intrare se află în `Core/EntryPoint.h`.

`App::Run()` (`Molecular/src/Molecular/Core/App.cpp`) folosește un **acumulator
cu pas de timp fix**, astfel încât fizica avansează determinist indiferent de
rata de cadre:

```cpp
const float fixedTimestep = 0.001f;   // pas de fizică de 1 ms
float accumulator = 0.0f;
while (m_running) {
    Timestep ts = now - lastFrameTime;
    accumulator += ts.GetSeconds();
    while (accumulator >= fixedTimestep) {       // fizica avansează în felii fixe
        for (Layer* layer : m_layerStack)
            layer->OnUpdate(fixedTimestep);
        accumulator -= fixedTimestep;
    }
    m_ImGuiLayer->Begin();                        // UI-ul se randează o dată pe cadru
    for (Layer* layer : m_layerStack)
        layer->OnImGuiRender();
    m_ImGuiLayer->End();
    m_window->OnUpdate();                         // swap buffers + poll events
}
```

### Stiva de layere

Motorul urmează șablonul de **stivă de layere** din Hazel. Un `Layer` expune
`OnAttach`, `OnDetach`, `OnUpdate(Timestep)`, `OnImGuiRender` și `OnEvent`.
Layerele se actualizează din față spre spate; evenimentele se propagă din spate
spre față și se opresc când un layer marchează evenimentul ca tratat
(`Handled`). `ImGuiLayer` este împins ca overlay (mereu deasupra).

Întreaga simulare trăiește într-un singur layer (`Sandbox2D` sau `Sandbox3D`).

## Convenții

- **Namespace:** tot ce ține de motor trăiește în `Molecular::`.
- **Alias-uri pentru smart pointeri:** `Ref<T>` = `std::shared_ptr<T>`,
  `Scope<T>` = `std::unique_ptr<T>` (vezi `Core/Core.h`).
- **Macro-uri:** prefixul `MOL_` — `MOL_CORE_INFO`, `MOL_CORE_ASSERT`,
  `MOL_BIND_EVENT_FN`, define-ul de platformă `MOL_PLATFORM_WINDOWS`,
  define-urile de configurație `MOL_DEBUG` / `MOL_RELEASE` / `MOL_DIST`.
- **Precizie:** fizica folosește **dublă precizie** (`glm::dvec2` / `dvec3`);
  randarea face downcast la `float`.