# Molecular — Documentație

**Molecular** este un simulator în timp real de fizică moleculară și subatomică,
construit pe un motor propriu C++ / OpenGL. Este un proiect de licență.

Depozitul este un singur workspace CMake care conține două produse:

| Țintă       | Tip              | Rol                                                          |
|-------------|------------------|--------------------------------------------------------------|
| `Molecular` | Bibliotecă statică | Motorul: fereastră, randare, evenimente, ImGui, fizică      |
| `Sandbox`   | Executabil       | Aplicația care rulează o simulare prin intermediul motorului |

Arhitectura motorului urmează îndeaproape motorul
[Hazel](https://github.com/TheCherno/Hazel) al lui TheCherno (stivă de layere,
sistem de evenimente, abstractizarea renderer-ului, macro-uri `MOL_`).

## Indexul documentației

| Document                                        | Conținut                                                      |
|-------------------------------------------------|---------------------------------------------------------------|
| [01 — Arhitectură](01-architecture.md)          | Structura depozitului, harta modulelor, bucla aplicației      |
| [02 — Motorul](02-engine.md)                    | Core, evenimente, renderer, layerele de platformă             |
| [03 — Fizică: dinamică moleculară 2D](03-physics-2d.md) | Atomi, forțe, integratoare, legături, energie         |
| [04 — Fizică: subatomic 3D](04-physics-3d.md)   | Particule subatomice, forțe Coulomb + Yukawa                  |
| [05 — Compilare și rulare](05-building-and-running.md)  | Build-ul CMake, configurații, cum se rulează          |
| [06 — Stadiu și plan](06-status-and-roadmap.md)         | Ce funcționează azi, probleme cunoscute, pași următori |

## Pe scurt

- **Limbaj / standard:** C++17
- **Sistem de build:** CMake (≥ 3.24)
- **Platformă:** Windows / MSVC (singura platformă implementată)
- **Grafică:** OpenGL (prin glad), fereastră GLFW
- **UI:** Dear ImGui
- **Matematică:** glm (dublă precizie pentru fizică)
- **Logging:** spdlog
- **Biblioteci vendorizate:** glfw, glad, glm, imgui, spdlog, stb_image

## Două domenii de simulare

1. **Dinamică moleculară 2D** (`Molecular/src/Molecular/Physics`) — partea matură.
   Atomi de H, O, C, N care interacționează prin forțe Lennard-Jones și Coulomb,
   cu legături covalente, mai multe integratoare numerice, urmărirea energiei și
   export CSV. Rulată de `Sandbox2D`, cu un panou de control ImGui complet.

2. **Subatomic 3D** (`Molecular/src/Molecular/Physics3D`) — mai nou, în lucru.
   Protoni / neutroni / electroni la scara femtometrilor, interacționând prin
   forța Coulomb și aproximația Yukawa a forței tari, integrați cu
   Velocity-Verlet. Schela de randare există în `Sandbox3D`; fizica nu este încă
   legată de bucla de randare.

> Implicit este activ doar `Sandbox2D` — vezi `Sandbox/src/Sandbox.cpp`.