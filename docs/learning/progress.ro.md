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

\* găsit cu debugger-ul într-o sesiune ghidată — se pune, dar cu asterisc.

## Citirea curentă (2026-07-05)

Două exerciții făcute: ciclurile stau la 4, iar 002 a necesitat mai mult
eșafodaj decât 001 (H3 vs H1). De așteptat — 002 a fost un API real cu cazuri
limită ostile, nu configurare de build — dar, conform regulilor,
**dificultatea se menține pentru 003 și acesta trebuie să re-testeze cel puțin
unul dintre:** semantica `static` local în funcție, ordinea gărzilor la
scurtcircuit, sau citirea diagnosticelor de compilator/linker. Țintă: ≤ H1 pe
conceptul re-testat.