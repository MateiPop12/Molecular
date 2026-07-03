# 03 — Fizică: dinamică moleculară 2D

Locație: `Molecular/src/Molecular/Physics`. Aceasta este partea cea mai
dezvoltată a proiectului și cea care rulează implicit (condusă de `Sandbox2D`).

Este o simulare clasică de **dinamică moleculară** (MD): atomi punctiformi în 2D
se mișcă sub forțe de pereche, integrate în pași de timp ficși, cu coliziuni,
legături covalente și contabilizarea energiei.

## Componente

| Fișier                     | Rol                                                             |
|----------------------------|-----------------------------------------------------------------|
| `AtomData.h`               | Tabelul de proprietăți ale elementelor (`elementData`)          |
| `Atom.{h,cpp}`             | Un atom: stare + proprietăți per element + legături             |
| `BoundingBox.h`            | Limitele 2D ale simulării, aliniate la axe                      |
| `ForceCalculator.{h,cpp}`  | Forțe de pereche + energie + răspuns la coliziuni               |
| `Integrator.{h,cpp}`       | Scheme de integrare numerică                                    |
| `SimulationSpace.{h,cpp}`  | Deține atomii, rulează pasul, urmărește legăturile + istoricul energiei |

## Datele elementelor (`AtomData.h`)

Fiecare element suportat poartă parametri motivați fizic:

| Element | Masă (amu) | Rază vdW (nm) | Lungime legătură (nm) | ε (eV) | σ (nm) | Electronegativitate | Valență |
|---------|-----------:|--------------:|----------------------:|-------:|-------:|--------------------:|--------:|
| H       | 1.008      | 0.12          | 0.074                 | 0.028  | 0.2958 | 2.20                | 1       |
| O       | 15.999     | 0.152         | 0.121                 | 0.095  | 0.3165 | 3.44                | 2       |
| C       | 12.011     | 0.17          | 0.154                 | 0.120  | 0.34   | 2.55                | 4       |
| N       | 14.007     | 0.155         | 0.145                 | 0.070  | 0.325  | 3.04                | 3       |

Un `Atom` poate fi construit fie din parametri bruți, fie după simbolul
elementului (care caută valorile în `elementData`).

## Forțe (`ForceCalculator`)

Pentru fiecare pereche de atomi se însumează două forțe:

### Van der Waals — Lennard-Jones

```
F = 48·ε·[ (σ/r)¹² − ½·(σ/r)⁶ ] / r²   ·  r̂
```

cu regulile de combinare `ε = (εₐ+ε_b)/2`, `σ = (σₐ+σ_b)/2`, un mic termen de
înmuiere (softening) adăugat la `r` și limitarea mărimii forței la `±m_maxForce`
(implicit `1e3`) pentru stabilitate numerică.

> Vezi și nota de studiu [Potențialul Lennard-Jones](learning/physics/lennard-jones.md) —
> inclusiv observația că media aritmetică pentru ε se abate de la regula
> standard Berthelot (medie geometrică).

### Coulomb — electrostatic

```
F = kₑ·q₁·q₂ / (εᵣ·(r+softening)²)   ·  r̂
```

cu `kₑ = 8.9875…e9 N·m²/C²`, sarcinile exprimate în unități de sarcină
elementară, permitivitatea vidului (`εᵣ = 1`) și aceeași limitare.

`CalculateTotalForce` iterează peste toți ceilalți atomi, însumează ambele
contribuții și limitează vectorul rezultat.

### Energie

- **Cinetică:** `Σ ½·m·v²`
- **Potențială:** Lennard-Jones de pereche `Σ 4·ε·[(σ/r)¹² − (σ/r)⁶]`
- **Totală:** cinetică + potențială — folosită pentru graficele de conservare
  a energiei.

### Coliziuni

`HandleCollision` face o reflexie de tip impuls (`v' = v − 2(v·n)n`), aplică un
**factor de pierdere de energie** (restituție, implicit `0.9`) și separă atomii
suprapuși. Distanța minimă depinde de existența unei legături între cei doi
(lungimea legăturii covalente) sau nu (raza van der Waals × 0.9).

## Integrare (`Integrator`)

`IntegrationMethod` alege schema:

- `Euler`
- `RungeKutta4`
- `LeapFrog`
- `Verlet`
- `VelocityVerlet` *(implicit în constructorul parametrizat)*

Plus, opțional, **pas de timp adaptiv** (dt min/max, toleranță de eroare) și
tratarea coliziunilor cu granițele `BoundingBox` (cu restituție).

> Notă: constructorul implicit al lui `SimulationSpace` folosește `RungeKutta4`;
> transmiterea explicită a unei metode permite alegerea alteia. Pasul fix al
> aplicației este `1e-3 s`.

## Orchestrare (`SimulationSpace`)

`SimulationSpace` este fața publică a simulării 2D:

- **Gestiunea obiectelor:** `AddObject`, `ClearAllAtoms`, `GetObjects`.
- **Ciclu de viață:** `StartSimulation` / `StopSimulation` / `ResetSimulation`,
  `SaveInitialState` / `ResetToInitialPositions` (salvează aranjamentul inițial
  pentru a putea rejuca o rulare).
- **Pas:** `Update(Timestep, BoundingBox)` integrează fiecare atom la fiecare
  cadru (doar cât timp simularea rulează).
- **Legături:** `UpdateBonds`, `GetTotalBondCount`, `GetBondPairs` — legăturile
  covalente se formează/rup pe baza distanței, valenței și electronegativității
  (vezi `Atom::TryFormBond` / `ShouldBreakBondWith`).
- **Urmărirea energiei:** înregistrează energia totală la fiecare câțiva pași în
  `m_energyHistory` / `m_timeHistory`, cu `ExportEnergyDataToCSV` pentru a
  exporta seria spre analiză.

## Scena 2D (`Sandbox2D`)

`Sandbox/src/Sandbox2D.cpp` leagă simularea de UI și de renderer:

- **Randare** (`OnUpdate`): avansează simularea, desenează pereții cutiei de
  delimitare ca quad-uri și fiecare atom ca cerc colorat prin `Renderer2D`.
- **Panoul de control** (`OnImGuiRender`): start/stop/reset, alegerea metodei de
  integrare, factorul de pierdere de energie, adăugare/ștergere atomi per
  element și **preset-uri de molecule**:
  - `SetupH2OSimulation()` — apă
  - `SetupO3Simulation()` — ozon
  - `SetupCH4Simulation()` — metan
- **Spawnare:** `AddRandomAtom`, `GenerateRandomPosition`, `IsPositionValid`
  plasează atomii fără suprapuneri inițiale în interiorul cutiei de delimitare.