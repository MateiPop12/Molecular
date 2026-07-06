# Măsurarea progresului

Scopul este să măsurăm **învățarea**, nu activitatea. Trei semnale, în ordinea
în care chiar prezic învățarea:

1. **Tendința eșafodajului** — cât ajutor a necesitat fiecare exercițiu
   (scala H de mai jos). Învățarea se vede când H *scade* în timp ce
   dificultatea se menține sau crește.
2. **Cicluri de review** — câte runde review → corectură până la acceptare.
3. **Retenția** — dacă un concept predat o dată se întoarce mai târziu ca
   bug. Fiecare exercițiu nou refolosește deliberat cel puțin un concept
   dintr-unul anterior — asta face retenția măsurabilă.

Timpul petrecut se notează drept *context* (numitorul), nu ca țintă —
optimizarea vitezei te învață scurtături, nu fizică sau C++.

## Scala de ajutor (H)

| Nivel | Semnificație |
| --- | --- |
| **H0** | Doar specificația — rezolvat singur |
| **H1** | Indicii conceptuale (nume de unelte/funcții, „uită-te la `lexically_normal`") |
| **H2** | Pseudocod / rețetă de pași ordonată |
| **H3** | Ghidaj pas cu pas cu instrucțiuni exacte |
| **H4** | S-a arătat cod funcțional |

!!! warning "Legea lui Goodhart"
    A cere ajutor este **comportament corect**, nu o penalizare. Scala există
    ca să calibreze *următorul* exercițiu, niciodată ca să judece cererea de
    ajutor. În momentul în care „H mic" devine scopul, cifra scade și
    învățarea se oprește. Contează tendința între exerciții — niciodată un
    singur punct de date.

## Reguli de ajustare

Aplicate când se pregătește următorul exercițiu (`/new-exercise` citește mai
întâi acest tabel):

- **H0–H1 și ≤ 2 cicluri** → crește dificultatea: mai multe concepte noi,
  specificație mai puțin prescriptivă.
- **H2 și ≤ 3 cicluri** → menține dificultatea, introduce concepte noi.
- **H3+ sau > 3 cicluri** → menține dificultatea și **consolidează**:
  următorul exercițiu re-testează aceleași concepte slabe, cu țintă ≤ H1.
- Un concept care apare ca bug găsit la review **de două ori** primește
  propriul exercițiu dedicat.

## Fișa de scor

Un rând per exercițiu, completat de Claude la acceptarea review-ului.
**Timpul** e raportat de Matei (granularitate de 0,5 h e suficientă).

| # | Exercițiu | Zile | Sesiuni | Timp | Cicluri | Ajutor maxim | Bug-uri: singur / review | Concepte slabe de re-testat |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 001 | [ținta de teste doctest](exercises/001-doctest-target.md) | 1 | 1 | ~2 h | 4 | H1 | 0 / 3 | CMake link-vs-include · RAII |
| 002 | [helper pentru căi de asset-uri](exercises/002-asset-path-helper.md) | 3 | 2 | ~3 h | 4 | H3 | 1\* / 3 | semantica inițializării `static` · ordinea gărzilor la scurtcircuit · citirea erorilor de linker |
| 003 | [repararea build-ului ImGui](exercises/003-imgui-build-fix.md) | 2 | 2 | ~2 h | 3 | H4† | 0 / 3 | rezolvarea căilor relative în fișierele CMake · unity include & simboluri duplicate |

\* găsit cu debugger-ul într-o sesiune ghidată — se pune, dar cu asterisc.

† doar pe mecanica include-path. Jumătatea de propagare — conceptul pe care
003 îl re-testa — a fost rezolvată **singur** (directoarele hardcodate
scoase, link-ul trecut pe `PUBLIC`) înainte de renunțare, deci ținta de
re-testare ≤ H1 a fost de fapt atinsă.

## Citirea curentă (2026-07-06)

Trei exerciții făcute: ciclurile au scăzut în sfârșit (4 → 4 → 3), iar
conceptul pe care 003 îl re-testa deliberat — propagarea include/link — a fost
rezolvat **singur**, exact semnalul de retenție pe care îl așteptam. H4-ul din
titlu e real, dar îngust: acoperă un singur concept (căile relative se rezolvă
față de `CMAKE_CURRENT_SOURCE_DIR`), atins ghicind de două ori în loc de a
întreba *„relativ la ce?"*. Conform regulilor, **H3+ înseamnă că 004
consolidează**: trebuie să re-testeze *rezolvarea căilor relative în fișierele
CMake* și *raționamentul unity-include / simboluri duplicate* oriunde atinge
build-ul (țintă ≤ H1); ce nu acoperă natural se mută în 005. Datorie de proces
de urmărit: de două ori s-a raportat o corectură ca gata fără să fie în
fișierul pe care build-ul chiar îl citește — verifică înainte de a raporta.