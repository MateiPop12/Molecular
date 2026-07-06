# 🧩 Exercițiul 003 — Repararea build-ului ImGui (clone proaspete + CRT)

**Stadiu:** ✅ gata · dat pe 2026-07-05 · finalizat pe 2026-07-06
**Concepte:** definirea țintelor CMake & vizibilitate *(re-testează 001/002 — țintă ≤ H1)* · dependențe vendored & igiena submodulelor · modelul CRT runtime MSVC

## Motivație

Build-ul rădăcină face `add_subdirectory(Molecular/vendor/imgui)` — dar
`CMakeLists.txt` din acel director **nu există în istoria niciunui
repository**. Stă în working tree-ul *submodulului* imgui, pe care upstream-ul
nu-l urmărește. Consecințe, toate active acum:

1. **Clonele proaspete nu se pot configura.** `git clone --recurse-submodules`
   scoate imgui-ul upstream curat → fără `CMakeLists.txt` → CMake eșuează.
   Pentru un repo de licență asta e un defect, nu un moft.
2. **`git status` e permanent murdar** — pata `m Molecular/vendor/imgui` pe
   care o vezi de două sesiuni *este acest fișier*.
3. **Cauzează avertismentul `LNK4098`.** Fișierul forțează
   `MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"` — CRT-ul
   **static** — în timp ce rădăcina impune `MultiThreadedDebugDLL` pentru
   toate celelalte ținte. CRT-uri amestecate = `LIBCMTD conflicts` la fiecare
   link al `Sandbox.exe`.
4. Neglijențe diverse: `imgui_widgets.cpp` apare **de două ori**, iar
   `-g` / `-O3` sunt **flag-uri GCC** date lui MSVC, care le ignoră.
5. **Căile de include nu călătoresc cu ținta.** Consumatorii găsesc `imgui.h`
   doar pentru că `Molecular` hardcodează
   `${CMAKE_SOURCE_DIR}/Molecular/vendor/imgui` în include-urile *sale*
   PUBLIC (`Molecular/CMakeLists.txt:24-25`), iar `Sandbox.cpp:7` recurge la
   `#include "../../Molecular/vendor/imgui/imgui.h"`. Ținta care deține
   header-ele ar trebui să le exporte — ai învățat exact mecanismul ăsta când
   ai trecut `MOL_ASSETS_DIR` pe `PRIVATE` în 002.

## Specificație

Creează **`Molecular/vendor/imgui-build/CMakeLists.txt`** — un director
frate, *urmărit de acest repo* — și lasă submodulul curat.

Cerințe:

1. Definește acolo ținta `ImGui` STATIC, listând sursele din `../imgui/`
   explicit, **fără duplicate**. Decide dacă `imgui_demo.cpp` rămâne și scrie
   un comentariu de un rând care apără alegerea.
2. `target_include_directories(ImGui PUBLIC ...)` către directorul
   submodulului (și `backends/` dacă engine-ul folosește fișierele impl) —
   astfel orice țintă care leagă `ImGui` moștenește automat calea de include.
3. **Fără override `MSVC_RUNTIME_LIBRARY`.** Moștenește
   `MultiThreadedDebugDLL` din rădăcină. Avertismentul `LNK4098` trebuie să
   dispară la un rebuild complet.
4. Fără flag-uri GCC. Dacă crezi că e nevoie de o opțiune per-configurație,
   trebuie să fie validă pentru MSVC — altfel nu adăuga nimic; default-urile
   sunt bune.
5. `CMakeLists.txt:20` din rădăcină arată spre `imgui-build`; fișierul
   parazit e șters din arborele submodulului; `git status` arată submodulul
   curat.
6. **Stretch:** scoate cele două linii imgui din include-urile PUBLIC ale lui
   `Molecular` și schimbă `Sandbox.cpp` la simplul `#include "imgui.h"` —
   dacă propagarea include-urilor e legată corect, *merge din prima*, iar
   dovada asta e chiar scopul.

## Prag de acceptare

Testul onest e o clonă proaspătă. Dintr-un director scratch **în afara**
acestui repo:

```powershell
git clone --recurse-submodules https://github.com/MateiPop12/Molecular molecular-fresh
cd molecular-fresh
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --target MolecularTests
```

- [x] Clona proaspătă se configurează și se compilează fără pași manuali.
      *(verificat 2026-07-06 — clonă → configurare → build `MolecularTests` +
      `Sandbox`, doctest 11/11 în clona proaspătă)*
- [x] `LNK4098` nu mai apare la link-ul lui `Sandbox.exe` (rebuild complet).
- [x] `git status` în acest repo arată submodulul imgui curat — pata `m`
      dispare definitiv.
- [x] Review-ul lui Claude e curat (3 cicluri).

Fără fișier doctest aici — ca la 001, sistemul de build *este* exercițiul,
iar clona proaspătă e testul.

Titlu de commit sugerat: `build: move ImGui target out of the submodule tree`.