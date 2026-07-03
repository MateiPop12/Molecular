# 05 — Compilare și rulare

## Cerințe preliminare

- **CMake ≥ 3.24**
- **Windows + MSVC** (singura platformă implementată; build-ul forțează un
  runtime MSVC și leagă `opengl32`)
- Un compilator C++17
- Toate bibliotecile terțe sunt **vendorizate** sub `Molecular/vendor`
  (glfw, glad, glm, imgui, spdlog, stb_image) — nu trebuie instalat nimic în plus.

## Structura CMake

`CMakeLists.txt`-ul din rădăcină definește proiectul `MolecularWorkspace` și:

1. Setează C++17, fără extensii de compilator.
2. Forțează un runtime MSVC consecvent (`MultiThreadedDebugDLL`) pe toate
   țintele, ca să evite erorile de link `LNK4098` (nepotrivire de runtime).
3. Direcționează output-ul build-ului în `bin/<Config>-<System>-<Arch>` și
   intermediarele în `bin-int/...`.
4. Adaugă bibliotecile vendor care au nevoie de compilare: `glfw`, `glad`,
   `imgui`. (`stb_image` se compilează direct în `Molecular` printr-un GLOB,
   **nu** ca subdirector separat — vezi comentariile din fișierele CMake.)
5. Adaugă țintele `Molecular` și `Sandbox`.

### Ținte

- **`Molecular`** — bibliotecă `STATIC`. Face GLOB pe `src/*.{h,cpp}`
  (+ stb_image), își expune directoarele de include ca `PUBLIC` (dependențele le
  moștenesc), leagă `glfw glad ImGui opengl32`, folosește un header precompilat
  (`molpch.h`) și definește `MOL_PLATFORM_WINDOWS`, `MOL_BUILD_DLL`,
  `GLFW_INCLUDE_NONE`.
- **`Sandbox`** — executabil. Face GLOB pe propriile surse și leagă `Molecular`
  (moștenind tranzitiv toate directoarele de include).

### Configurații de build

Ambele ținte definesc câte un macro per configurație CMake:

| Configurație | Define        |
|--------------|---------------|
| `Debug`      | `MOL_DEBUG`   |
| `Release`    | `MOL_RELEASE` |
| `Dist`       | `MOL_DIST`    |

## Compilare

```sh
# din rădăcina depozitului
cmake -S . -B build
cmake --build build --config Debug
```

Executabilul `Sandbox` ajunge în `build/bin/<Config>-Windows-<Arch>/`.

> Proiectul este pregătit să fie deschis direct în CLion / Visual Studio ca
> proiect CMake. `.gitignore` exclude `bin/`, `bin-int/`, `cmake-*/`, `.vs/`,
> `.idea/` și fișierele de soluție/proiect Visual Studio generate.

## Rulare

Rulează executabilul `Sandbox`. Implicit, `Sandbox.cpp` împinge `Sandbox2D`,
deci se deschide simulatorul de dinamică moleculară 2D cu panoul lui de control
ImGui.

Pentru a schimba scena, editează constructorul `Sandbox` din
`Sandbox/src/Sandbox.cpp`:

```cpp
Sandbox() {
    //PushLayer(new ExampleLayer());   // demo istoric de renderer
    PushLayer(new Sandbox2D());        // dinamică moleculară 2D (implicit)
    //PushLayer(new Sandbox3D());      // viewport subatomic 3D (în lucru)
}
```

## Căile de asset-uri (`MOL_ASSETS_DIR`)

Shaderele și texturile se încarcă prin macro-ul de compilare **`MOL_ASSETS_DIR`**,
definit de CMake în `Molecular/CMakeLists.txt`:

```cmake
target_compile_definitions(Molecular PUBLIC
        MOL_ASSETS_DIR="${CMAKE_SOURCE_DIR}/Sandbox/assets")
```

El se expandează într-un string literal care conține calea absolută către
`Sandbox/assets`, rezolvată la rularea CMake. Fiind `PUBLIC` pe ținta
`Molecular`, atât motorul cât și `Sandbox` îl moștenesc. Punctele de apel
concatenează peste el:

```cpp
Shader::Create(MOL_ASSETS_DIR "/shaders/Texture.glsl");
Molecular::Texture2D::Create(MOL_ASSETS_DIR "/textures/Checkerboard.png");
```

Astfel, încărcarea asset-urilor funcționează indiferent de directorul de lucru
al procesului (rulări de debug din IDE, dublu-click pe exe etc.) și este
portabilă între mașini: calea se recalculează la fiecare configurare a
proiectului, deci este corectă oriunde este clonat depozitul. Asset-urile nu se
copiază — se citesc direct din arborele sursă.

> Dacă mai târziu vrei un build complet relocabil (unul pe care îl poți arhiva
> și rula pe o mașină fără surse), treci la copierea directorului `assets/`
> lângă executabil printr-un `add_custom_command` CMake și rezolvă căile
> relativ la exe la runtime. `MOL_ASSETS_DIR` este varianta ușoară, pentru
> dezvoltare pe această mașină.