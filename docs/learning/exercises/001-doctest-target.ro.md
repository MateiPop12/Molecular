# 🧩 Exercițiul 001 — Configurarea țintei de teste doctest

**Stadiu:** ✅ gata · dat și finalizat pe 2026-07-03
**Concepte:** graful de ținte CMake · CTest · doctest · disciplina roșu→verde

Toate exercițiile viitoare vin ca teste doctest compilate, așa că primul
exercițiu este chiar harness-ul. Când e gata, proiectul are un executabil
`MolecularTests` pe care CTest îl cunoaște, iar „rulează testele" devine o
singură comandă.

## Specificație

1. **Vendorizează doctest.** Descarcă header-ul unic din
   [release-ul doctest v2.4.12](https://github.com/doctest/doctest/releases/tag/v2.4.12)
   (fișierul `doctest.h`) și pune-l la `tests/vendor/doctest/doctest.h`. E un
   singur header, ~7k linii, zero dependențe — acesta e tot framework-ul.

2. **Creează ținta.** Scrie `tests/CMakeLists.txt` care definește un executabil
   `MolecularTests` din sursele de mai jos, legat de biblioteca `Molecular`,
   ca testele să poată exersa codul motorului:
   - `tests/main.cpp`
   - `tests/sanity_tests.cpp`

3. **Leagă-l în workspace.** În `CMakeLists.txt`-ul din rădăcină:
   - `enable_testing()` (trebuie să fie la rădăcină, *înaintea*
     subdirectorului — află de ce, face parte din exercițiu)
   - `add_subdirectory(tests)`

4. **Înregistrează la CTest.** În `tests/CMakeLists.txt`, `add_test(...)`
   astfel încât `ctest` să descopere și să ruleze executabilul.

## Fișierele pe care le primești

`tests/main.cpp` — doctest îți furnizează `main()`:

```cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "vendor/doctest/doctest.h"
```

`tests/sanity_tests.cpp` — dovedește că harness-ul funcționează:

```cpp
#include "vendor/doctest/doctest.h"

#include "Molecular/Physics/Vec2D.h" // ajustează la un header real din motor

TEST_CASE("sanity: the harness runs")
{
    CHECK(1 + 1 == 2);
}

TEST_CASE("sanity: engine headers are reachable")
{
    // Construiește aici orice tip ieftin din motor, ca să dovedești că
    // include-path-urile și linia de link sunt corecte. Alege ceva din Physics/.
    CHECK(true);
}
```

Înlocuiește include-ul/tipul substituent cu unul real din `Molecular/src` —
o parte din exercițiu este să descoperi ce directoare de include exportă ținta
`Molecular` și să adaugi `target_include_directories` dacă nu exportă niciunul.

## Prag de acceptare

- [ ] Reconfigurarea CMake de la zero reușește (profilul CLion, generatorul VS 2022).
- [ ] `MolecularTests` compilează fără warning-uri *introduse de tine*.
- [ ] `ctest` rulat din directorul de build raportează testele ca trecute.
- [ ] **Proba de roșu:** schimbă temporar un `CHECK` în ceva fals, privește-l
      cum pică cu un mesaj util, apoi revino. Dacă nu l-ai văzut niciodată
      roșu, nu știi că funcționează.
- [ ] `tests/` este comis (inclusiv header-ul vendorizat); titlu sugerat:
      `build: add doctest test target`.

## Indicii (folosește-le cu zgârcenie)

??? hint "Unde merge `enable_testing()` și de ce?"
    `enable_testing()` afectează doar directorul în care e apelat *și mai jos*.
    Apelat în `tests/`, directorul de build din rădăcină nu primește
    `CTestTestfile.cmake`, iar `ctest` rulat din rădăcina build-ului găsește
    zero teste.

??? hint "Erori de link care menționează GLFW/OpenGL?"
    `Molecular` își leagă bibliotecile vendor. Dacă sunt `PRIVATE`, ținta ta de
    teste nu le moștenește și are nevoie doar de `Molecular`. Dacă primești
    simboluri nerezolvate, verifică ce vizibilitate declară
    `Molecular/CMakeLists.txt` la `target_link_libraries` — și judecă ce
    înseamnă `PRIVATE` vs `PUBLIC` pentru linia *ta* de link.

## După ce termini

Dă-i de știre lui Claude pentru review. Apoi se deblochează
[Exercițiul 002](002-asset-path-helper.md).