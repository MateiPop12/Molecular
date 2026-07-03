# 🧩 Exercițiul 002 — Helper pentru căi de asset-uri

**Stadiu:** 🔴 deschis · dat pe 2026-07-03 · **blocat de [001](001-doctest-target.md)**
**Concepte:** `std::filesystem` · `std::string_view` · design de API · granițe defensive

*(Acesta este exercițiul din sesiunea de 2026-06-27 care înainte exista doar în
chat — reînviat aici ca să nu se mai poată pierde.)*

## Motivație

Acum, fiecare loc care încarcă un asset concatenează de mână macro-ul string
`MOL_ASSETS_DIR` (`MOL_ASSETS_DIR "/shaders/x.glsl"`). Asta merge doar pentru
literali, împrăștie logica de căi prin toată baza de cod și dă erori inutile
când lipsește un fișier. Centralizeaz-o.

## Specificație

Creează `Molecular/src/Molecular/Core/Assets.h` / `.cpp` cu:

```cpp
namespace Molecular::Assets {

    // Rădăcina asset-urilor fixată la configurare (MOL_ASSETS_DIR),
    // ca o cale absolută normalizată.
    std::filesystem::path Root();

    // Rezolvă o cale relativă la rădăcina asset-urilor.
    //   Path("shaders/flat.glsl") -> <root>/shaders/flat.glsl
    // Aruncă std::invalid_argument dacă `relative`:
    //   - este o cale absolută, sau
    //   - iese din rădăcină după normalizare (ex. "../../secrets").
    std::filesystem::path Path(std::string_view relative);

    // Adevărat dacă asset-ul rezolvat există pe disc.
    bool Exists(std::string_view relative);

}
```

Cerințe:

1. `MOL_ASSETS_DIR` este referit în **exact o** unitate de compilare
   (`Assets.cpp`). Niciun alt fișier nu menționează macro-ul.
2. `Path` trebuie să respingă evadările *după* normalizare —
   `"shaders/../../x"` evadează deși nu începe cu `..`.
   Uită-te la `std::filesystem::path::lexically_normal`.
3. Înlocuiește utilizările existente ale `MOL_ASSETS_DIR` din `Renderer2D.cpp`,
   `Renderer3D.cpp` și `Sandbox.cpp` cu helper-ul.
4. Gândește-te *de ce* `string_view` este tipul potrivit de parametru aici și
   fii pregătit să-l aperi la review (cât ar costa `const std::string&`? ce ar
   pierde `const char*`?).

## Fișierul de teste

Pune-l ca `tests/asset_path_tests.cpp` și adaugă-l la sursele
`MolecularTests`:

```cpp
#include "vendor/doctest/doctest.h"

#include "Molecular/Core/Assets.h"

#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;
using namespace Molecular;

TEST_CASE("Root is absolute and normalized")
{
    const fs::path root = Assets::Root();
    CHECK(root.is_absolute());
    CHECK(root == root.lexically_normal());
    CHECK(fs::exists(root)); // directorul de asset-uri trebuie să existe
}

TEST_CASE("Path resolves relative paths under the root")
{
    const fs::path p = Assets::Path("shaders/whatever.glsl");
    CHECK(p == (Assets::Root() / "shaders" / "whatever.glsl").lexically_normal());
}

TEST_CASE("Path rejects absolute inputs")
{
    CHECK_THROWS_AS((void)Assets::Path("C:/Windows/System32"), std::invalid_argument);
}

TEST_CASE("Path rejects escapes from the root")
{
    CHECK_THROWS_AS((void)Assets::Path("../outside.txt"), std::invalid_argument);
    CHECK_THROWS_AS((void)Assets::Path("shaders/../../outside.txt"), std::invalid_argument);
}

TEST_CASE("Path accepts dotdot that stays inside the root")
{
    const fs::path p = Assets::Path("shaders/../textures/x.png");
    CHECK(p == (Assets::Root() / "textures" / "x.png").lexically_normal());
}

TEST_CASE("Exists reports real files")
{
    // Ajustează la un asset care chiar există în repo:
    CHECK(Assets::Exists("shaders/FlatColor.glsl"));
    CHECK_FALSE(Assets::Exists("definitely/not/here.bin"));
}
```

Un test referă un fișier de shader real — ajustează numele la un asset care
există cu adevărat înainte să dai vina pe implementarea ta.

## Prag de acceptare

- [ ] Toate testele de mai sus trec (și ai văzut măcar unul picând mai întâi).
- [ ] `MOL_ASSETS_DIR` apare în exact un `.cpp` din tot repo-ul
      (`git grep MOL_ASSETS_DIR` ca dovadă — definiția din CMake nu se pune).
- [ ] Sandbox-ul încă rulează și randează (asset-urile chiar se încarcă prin helper).
- [ ] Review-ul lui Claude e curat: denumiri, const-correctness, fără copii
      inutile, mesaje de eroare cu sens.

Titlu de commit sugerat: `refactor: centralize asset paths in Assets helper`.