# 02 — Engine

The `Molecular` static library is a small game-engine-style framework. This
document covers everything except the physics, which has its own pages
([03](03-physics-2d.md), [04](04-physics-3d.md)).

## Core (`Molecular/src/Molecular/Core`)

| File              | Responsibility                                                            |
|-------------------|---------------------------------------------------------------------------|
| `App.{h,cpp}`     | Singleton application; owns the window, layer stack, ImGui overlay, main loop |
| `EntryPoint.h`    | `main()` — calls `Molecular::CreateApplication()` then `Run()`             |
| `Layer.{h,cpp}`   | Base class for everything that updates/renders/handles events             |
| `LayerStack.{h,cpp}` | Ordered layers + overlays                                              |
| `Window.h`        | Abstract window interface (implemented by `Platform/Windows`)            |
| `Input.h`         | Static polling API for keyboard/mouse                                     |
| `KeyCodes.h` / `MouseCodes.h` | Input code constants                                          |
| `Timestep.h`      | Thin wrapper around a delta time in seconds                               |
| `Log.{h,cpp}`     | spdlog-based core + client loggers, `MOL_CORE_*` / `MOL_*` macros        |
| `Core.h`          | `Ref`/`Scope` aliases, assert + bind-event macros, API export macros     |

The factory pattern means the engine never references `Sandbox` directly: the
app subclasses `App`, pushes its layers in the constructor, and provides
`CreateApplication()`.

## Events (`Molecular/src/Molecular/Events`)

A synchronous, blocking event system (Hazel-style).

- `Event.h` — base `Event`, `EventDispatcher`, categories, `Handled` flag.
- `ApplicationEvent.h` — `WindowResizeEvent`, `WindowCloseEvent`, etc.
- `KeyEvent.h` — key pressed/released/typed.
- `MouseEvent.h` — move, scroll, button.

Flow: GLFW callback → `Window` → `App::OnEvent` → dispatched to the matching
handler, then propagated through the layer stack top-to-bottom until `Handled`.

## ImGui (`Molecular/src/Molecular/ImGui`)

- `ImGuiLayer.{h,cpp}` — overlay that sets up the ImGui GLFW + OpenGL3 backends,
  begins/ends frames around each render pass.
- `ImGuiBuild.cpp` — single translation unit that compiles the ImGui backends.

All simulation UI (control panels, sliders, molecule presets, graphs) is drawn
from the sandbox layers' `OnImGuiRender()`.

## Renderer (`Molecular/src/Molecular/Renderer`)

The renderer is **API-agnostic**: a `RendererAPI` interface is implemented by
`Platform/OpenGL`. Today only OpenGL exists, but the indirection is in place.

| File                                  | Role                                                       |
|---------------------------------------|------------------------------------------------------------|
| `RendererAPI.{h,cpp}`                 | Abstract draw API (clear, draw indexed, set viewport)      |
| `RenderCommand.{h,cpp}`               | Static façade over the active `RendererAPI`                |
| `Renderer.{h,cpp}`                    | High-level scene begin/end + submit (3D-ish, view-proj)    |
| `Renderer2D.{h,cpp}`                  | **Batch 2D renderer** — `DrawQuad`, `DrawCircle`           |
| `Renderer3D.{h,cpp}`                  | 3D renderer — `DrawSphere`, `DrawCube`, `DrawBox`          |
| `Shader.{h,cpp}`                      | Shader interface + `ShaderLibrary` (load/get by name)      |
| `Buffer.{h,cpp}`                      | Vertex/index buffers + `BufferLayout` / `ShaderDataType`   |
| `VertexArray.{h,cpp}`                 | Vertex array abstraction                                   |
| `Texture.{h,cpp}`                     | `Texture2D` interface (loaded via stb_image)               |
| `Framebuffer.{h,cpp}`                 | Offscreen render target (used by the 3D viewport)          |
| `OrthographicCamera.{h,cpp}`          | 2D camera                                                  |
| `OrthographicCameraController.{h,cpp}`| Pan/zoom controller for 2D                                 |
| `Camera.h` / `EditorCamera.{h,cpp}`   | 3D camera + orbit/fly editor camera                        |

### Shaders

GLSL shaders live in `Sandbox/assets/shaders` and are loaded through
`ShaderLibrary`:

- `FlatColor.glsl` — solid color
- `Texture.glsl` — textured quad
- `Sphere.glsl` — 3D sphere shading
- `Box.glsl` — 3D box shading

Some shaders are also defined inline as raw string literals in the legacy
`ExampleLayer` inside `Sandbox.cpp`.

## Platform (`Molecular/src/Platform`)

- **OpenGL/** — concrete implementations: `OpenGLContext`, `OpenGLBuffer`,
  `OpenGLVertexArray`, `OpenGLShader`, `OpenGLTexture`, `OpenGLFramebuffer`,
  `OpenGLRendererAPI`.
- **Windows/** — `WindowsWindow` (GLFW-backed) and `WindowsInput` (GLFW polling).

Because only Windows + OpenGL are implemented, the CMake adds
`MOL_PLATFORM_WINDOWS` and links `opengl32`.