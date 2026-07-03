# 02 — Motorul (engine)

Biblioteca statică `Molecular` este un mic framework în stil motor de joc.
Acest document acoperă tot în afară de fizică, care are paginile ei
([03](03-physics-2d.md), [04](04-physics-3d.md)).

## Core (`Molecular/src/Molecular/Core`)

| Fișier            | Responsabilitate                                                          |
|-------------------|---------------------------------------------------------------------------|
| `App.{h,cpp}`     | Aplicație singleton; deține fereastra, stiva de layere, overlay-ul ImGui, bucla principală |
| `EntryPoint.h`    | `main()` — apelează `Molecular::CreateApplication()`, apoi `Run()`         |
| `Layer.{h,cpp}`   | Clasa de bază pentru tot ce se actualizează / randează / tratează evenimente |
| `LayerStack.{h,cpp}` | Layere ordonate + overlay-uri                                           |
| `Window.h`        | Interfața abstractă de fereastră (implementată de `Platform/Windows`)     |
| `Input.h`         | API static de interogare (polling) pentru tastatură/mouse                 |
| `KeyCodes.h` / `MouseCodes.h` | Constantele codurilor de input                                |
| `Timestep.h`      | Înveliș subțire peste un delta de timp în secunde                          |
| `Log.{h,cpp}`     | Loggere core + client bazate pe spdlog, macro-urile `MOL_CORE_*` / `MOL_*` |
| `Core.h`          | Alias-urile `Ref`/`Scope`, macro-uri de assert + bind-event, macro-uri de export API |

Șablonul de fabrică (factory) face ca motorul să nu refere niciodată `Sandbox`
direct: aplicația moștenește `App`, își împinge layerele în constructor și
furnizează `CreateApplication()`.

## Evenimente (`Molecular/src/Molecular/Events`)

Un sistem de evenimente sincron, blocant (în stilul Hazel).

- `Event.h` — clasa de bază `Event`, `EventDispatcher`, categorii, flag-ul `Handled`.
- `ApplicationEvent.h` — `WindowResizeEvent`, `WindowCloseEvent` etc.
- `KeyEvent.h` — tastă apăsată/eliberată/tastată.
- `MouseEvent.h` — mișcare, scroll, butoane.

Fluxul: callback GLFW → `Window` → `App::OnEvent` → dirijat către handler-ul
potrivit, apoi propagat prin stiva de layere de sus în jos până la `Handled`.

## ImGui (`Molecular/src/Molecular/ImGui`)

- `ImGuiLayer.{h,cpp}` — overlay care configurează backend-urile ImGui pentru
  GLFW + OpenGL3 și deschide/închide cadrele în jurul fiecărei treceri de randare.
- `ImGuiBuild.cpp` — o singură unitate de compilare care compilează backend-urile ImGui.

Tot UI-ul simulării (panouri de control, slidere, preset-uri de molecule,
grafice) este desenat din `OnImGuiRender()` al layerelor din sandbox.

## Renderer (`Molecular/src/Molecular/Renderer`)

Renderer-ul este **agnostic de API**: interfața `RendererAPI` este implementată
de `Platform/OpenGL`. Azi există doar OpenGL, dar indirectarea este pregătită.

| Fișier                                | Rol                                                        |
|---------------------------------------|------------------------------------------------------------|
| `RendererAPI.{h,cpp}`                 | API abstract de desenare (clear, draw indexed, set viewport) |
| `RenderCommand.{h,cpp}`               | Fațadă statică peste `RendererAPI`-ul activ                |
| `Renderer.{h,cpp}`                    | Begin/end de scenă la nivel înalt + submit (3D, view-proj) |
| `Renderer2D.{h,cpp}`                  | **Renderer 2D cu batching** — `DrawQuad`, `DrawCircle`     |
| `Renderer3D.{h,cpp}`                  | Renderer 3D — `DrawSphere`, `DrawCube`, `DrawBox`          |
| `Shader.{h,cpp}`                      | Interfața de shader + `ShaderLibrary` (încărcare/căutare după nume) |
| `Buffer.{h,cpp}`                      | Buffere de vârfuri/indici + `BufferLayout` / `ShaderDataType` |
| `VertexArray.{h,cpp}`                 | Abstractizarea vertex array-ului                           |
| `Texture.{h,cpp}`                     | Interfața `Texture2D` (încărcată prin stb_image)           |
| `Framebuffer.{h,cpp}`                 | Țintă de randare offscreen (folosită de viewport-ul 3D)    |
| `OrthographicCamera.{h,cpp}`          | Cameră 2D                                                  |
| `OrthographicCameraController.{h,cpp}`| Controller de pan/zoom pentru 2D                           |
| `Camera.h` / `EditorCamera.{h,cpp}`   | Cameră 3D + cameră de editor orbit/fly                     |

### Shadere

Shaderele GLSL trăiesc în `Sandbox/assets/shaders` și se încarcă prin
`ShaderLibrary`:

- `FlatColor.glsl` — culoare uniformă
- `Texture.glsl` — quad texturat
- `Sphere.glsl` — shading de sferă 3D
- `Box.glsl` — shading de cutie 3D

Câteva shadere sunt definite și inline, ca string literals, în `ExampleLayer`
(moștenire istorică) din `Sandbox.cpp`.

## Platform (`Molecular/src/Platform`)

- **OpenGL/** — implementările concrete: `OpenGLContext`, `OpenGLBuffer`,
  `OpenGLVertexArray`, `OpenGLShader`, `OpenGLTexture`, `OpenGLFramebuffer`,
  `OpenGLRendererAPI`.
- **Windows/** — `WindowsWindow` (peste GLFW) și `WindowsInput` (polling GLFW).

Pentru că sunt implementate doar Windows + OpenGL, CMake adaugă
`MOL_PLATFORM_WINDOWS` și leagă `opengl32`.