# 06 — Stadiu și plan

O fotografie a ceea ce funcționează, ce e parțial și ce probleme sunt cunoscute
— utilă pentru redactarea licenței și pentru reluarea proiectului după o pauză.

## Ce funcționează azi

- ✅ **Fundația motorului** — fereastră (GLFW), randare OpenGL, stivă de layere,
  sistem de evenimente, integrare ImGui, logging. Buclă principală cu pas de
  timp fix.
- ✅ **Dinamică moleculară 2D** — simulatorul complet, funcțional:
  - Atomi de H, O, C, N cu parametri realiști per element.
  - Forțe Lennard-Jones (van der Waals) + Coulomb cu limitare (clamping).
  - Mai multe integratoare (Euler, RK4, LeapFrog, Verlet, Velocity-Verlet) +
    pas de timp adaptiv opțional.
  - Coliziuni cu restituție / pierdere de energie și tratarea granițelor.
  - Formarea/ruperea legăturilor covalente pe baza valenței + electronegativității.
  - Urmărirea energiei (cinetică / potențială / totală) cu export CSV.
  - Panou de control ImGui complet și preset-uri de molecule (H₂O, O₃, CH₄).
- ✅ **Primitive de randare 3D** — `Renderer3D` (sfere, cuburi, cutii),
  `EditorCamera`, viewport pe framebuffer în `Sandbox3D`.
- ✅ **Modelul fizic subatomic 3D** — datele particulelor (constante CODATA),
  Coulomb + forța tare Yukawa, integrator Velocity-Verlet. Compilează și poate
  fi testat unitar izolat.

## Parțial / în lucru

- 🚧 **Simularea 3D nu este legată.** `Physics3D` are forțe + un integrator, dar
  `Sandbox3D::OnUpdate` desenează doar cutii substituent — particulele nu sunt
  create, avansate sau randate ca sfere încă. `Sandbox3D` este comentat în
  `Sandbox.cpp`.
- 🚧 **Curățenie de nume în desfășurare.** Arborele de lucru redenumește
  `Physics3D/ForceCalculator` → `ForceCalculator3D`, pentru dezambiguizare față
  de clasa 2D. (`Physics3D/Integrator` încă împarte numele `Integrator` cu
  integratorul 2D — o redenumire similară ar fi consecventă.)

## Probleme cunoscute / oportunități de curățenie

- ✅ ~~**Căi absolute hardcodate către asset-uri**~~ — *rezolvat.* Încărcările de
  shadere/texturi din `Sandbox.cpp`, `Renderer2D.cpp` și `Renderer3D.cpp`
  folosesc acum macro-ul `MOL_ASSETS_DIR` definit de CMake, în locul căilor
  `D:/FACULTATE/Licenta/...`. Vezi [05 — Compilare și rulare](05-building-and-running.md).
- 🧹 **`ExampleLayer`-ul istoric** (un demo de renderer de ~200 de linii cu GLSL
  inline) trăiește încă în `Sandbox.cpp`. Este schela de învățare din Hazel și
  ar putea fi șters sau mutat din fișierul principal de intrare.
- 🧩 **Concepte duplicate între 2D/3D** (`ForceCalculator`, `Integrator`,
  `PhysicsObject`/`Atom`). O bază comună de fizică ar reduce duplicarea, dacă
  cele două domenii urmează să conveargă.
- 🖥️ **O singură platformă.** Doar Windows + OpenGL sunt implementate, deși
  abstractizările `RendererAPI` / `Window` sunt gândite să permită mai multe.

## Pași naturali următori

1. Legarea `Physics3D` în `Sandbox3D`: spawnarea de protoni/neutroni/electroni,
   avansarea integratorului Velocity-Verlet la fiecare cadru și randarea lor cu
   `Renderer3D::DrawSphere`.
2. Un panou de control ImGui pentru 3D, în oglindă cu cel 2D (spawnare
   particule, construirea unui nucleu, start/stop, afișarea energiei).
3. Scoaterea `ExampleLayer`-ului istoric din `Sandbox.cpp`.
4. Extinderea `README.md` și legarea către `docs/`.

## Context din istoricul de commit-uri

```
c8fef8a  Added 3D physics package         ← introducerea Physics3D
622ca12  Added boxes in Renderer3D
c6daa80  Added 3d Camera
6401842  To many changes to count
529904b  Abstracted integration methods and force calculations
```

Traiectoria e clară: întâi un simulator 2D funcțional, apoi construirea unui
mod subatomic 3D pe același motor.