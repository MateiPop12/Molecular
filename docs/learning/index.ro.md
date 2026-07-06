# Cum funcționează această bază de cunoștințe

Aceasta este partea de citit a sistemului de învățare. Chat-ul rămâne scurt;
tot ce merită păstrat — fapte de fizică, aprofundări de grafică/arhitectură,
exerciții, review-uri — ajunge **aici**, versionat în git, căutabil, cu
formule randate.

## Rularea site-ului

Din rădăcina depozitului:

```powershell
.\.venv\Scripts\mkdocs.exe serve
```

Apoi deschide <http://127.0.0.1:8000>. Site-ul se reîncarcă automat când o
pagină se schimbă — lasă-l deschis într-un tab pe durata sesiunilor.
Comutatorul de limbă (EN/RO) este în bara de sus; paginile netraduse încă se
afișează în engleză.

## Ce stă unde

| Conținut | Locație |
| --- | --- |
| 🔬 Note de fizică | `docs/learning/physics/<subiect>.md` |
| 🎨 Note de grafică / arhitectură | `docs/learning/graphics/<subiect>.md` |
| 🦀 Semințe Rust/Zig | `docs/learning/rust-zig/<subiect>.md` |
| 🧩 Specificațiile exercițiilor | `docs/learning/exercises/NNN-<titlu>.md` |
| Codul de test al exercițiilor | `tests/` (real, compilat, rulat prin CTest) |
| Jurnalul sesiunilor | [Jurnalul de învățare](../LEARNING_LOG.md) |

Traducerile în română sunt fișiere-pereche cu sufixul `.ro.md`
(ex. `lennard-jones.ro.md` lângă `lennard-jones.md`).

## Bucla unui exercițiu

1. **Specificație** — fiecare pagină de exercițiu enunță scopul, conceptul pe
   care îl predă, semnăturile exacte de implementat și pragul de acceptare.
2. **Roșu → verde** — fiecare exercițiu vine cu teste unitare doctest. Pornești
   de la teste care pică (sau nici nu compilează) și le faci să treacă.
3. **Review** — odată verde, Claude analizează critic soluția: corectitudine,
   stil idiomatic, ownership, denumiri. Corecturile sunt obligatorii, nu
   opționale.

Un exercițiu e **gata** doar când testele trec *și* review-ul e curat. Stadiul
se urmărește în tabelul de mai jos și se verifică la începutul fiecărei sesiuni.

## Stadiul exercițiilor

| # | Exercițiu | Dat | Stadiu |
| --- | --- | --- | --- |
| 001 | [ținta de teste doctest](exercises/001-doctest-target.md) | 2026-07-03 | ✅ gata (2026-07-03) |
| 002 | [helper pentru căi de asset-uri](exercises/002-asset-path-helper.md) | 2026-07-03 | ✅ gata (2026-07-05) |
| 003 | [repararea build-ului ImGui](exercises/003-imgui-build-fix.md) | 2026-07-05 | ✅ gata (2026-07-06) |
| 004 | [Physics3D în Sandbox3D](exercises/004-physics3d-wiring.md) | 2026-07-05 | 🔴 deschis |