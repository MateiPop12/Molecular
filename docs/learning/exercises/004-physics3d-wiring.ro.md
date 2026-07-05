# 🧩 Exercițiul 004 — Conectarea Physics3D în Sandbox3D

**Stadiu:** 🔴 deschis · dat pe 2026-07-05 · de făcut după [003](003-imgui-build-fix.md)
**Concepte:** ownership & RAII *(re-testează 001)* · **consistența unităților în simulare** · bucle cu pas fix · pointeri proprietari vs. observatori

## Motivație

Nucleul subatomic (`Physics3D/`) compilează, are date de particule corecte
CODATA, forțe Coulomb + Yukawa și un integrator Velocity-Verlet — și *nimic
nu-l rulează*. `Sandbox3D::OnUpdate` desenează cutii placeholder. Ăsta e
punctul #1 din roadmap.

Și te așteaptă un bug. **Integratorul nu e consistent în unități:** pozițiile
sunt în femtometri, dar `VelocityVerlet` adună `v·dt + ½a·dt²` calculat din
accelerații în m/s² — deplasări în *metri* adunate direct peste poziții în
*femtometri*, greșit cu un factor de 10¹⁵. Orice simulare legată de el azi ar
părea înghețată. Testele furnizate îl prind; repararea lui face parte din
exercițiu.

## Specificație

### A. Repară convenția de unități (roșu → verde)

Leagă `tests/physics3d_tests.cpp` în `tests/CMakeLists.txt` ca prim pas. Nouă
din cele zece cazuri trec pe nucleul de azi; **„unit consistency — electron
falls the right DISTANCE" pică.** Alege o convenție (ex. poziții în fm,
viteze în fm/s, accelerații convertite la fm/s² la graniță — sau convertește
pozițiile în metri în integrator), implementeaz-o și **documenteaz-o în
comentarii în `Integrator.h` și `PhysicsObject.h`**. Testul definește
corectitudinea: un electron la 100 fm de un proton cade ~1,27×10⁻³ fm
într-un pas de 10⁻²³ s.

### B. Deține particulele corect (re-testul RAII)

`Sandbox3D` deține starea simulării:

```cpp
std::vector<std::unique_ptr<SubatomicParticle>> m_particles;  // proprietar
std::vector<PhysicsObject*>                     m_simView;    // observator
```

API-ul integratorului vrea `std::vector<PhysicsObject*>&` — construiește
vederea de pointeri bruți din vectorul proprietar (reconstruiește la
spawn/despawn). Fii pregătit să aperi la review: de ce `unique_ptr` și nu
valori într-un `vector`? (Indiciu: ce pățesc pointerii din `m_simView` când
un `std::vector<SubatomicParticle>` crește?) De ce pointeri bruți în vedere
și nu `shared_ptr`?

🦀 *Împărțirea asta e exact `Vec<Box<Particle>>` proprietar cu împrumuturi
`&mut [..]` din Rust — ownership într-un singur loc, acces peste tot.*

### C. Pas fix de fizică — derivează-l, nu-l ghici

`OnUpdate(Timestep ts)` îți dă timpul pe *cadru* (~16 ms). Nu poți să-l dai
integratorului. Adaugă un pas fix de fizică și un acumulator, și **derivează
mărimea pasului într-un comentariu**: cu `a = F/m` pentru un electron la
câțiva fm de un proton, cât de mare poate fi `dt` astfel încât un pas să-l
miște ≪ 1 fm? (Vei ajunge pe la 10⁻²²–10⁻²³ s.) Apoi decide câți pași de
fizică pe cadru îți permiți și ce înseamnă „viteza simulării" în UI.

### D. Randare și control

- Fiecare particulă → `Renderer3D::DrawSphere` cu `GetRadius()`/`GetColor()`,
  cu o constantă numită pentru maparea fm → unități de lume.
- Panou ImGui minimal: spawn proton/neutron/electron, pauză/reluare, reset,
  număr de particule live. (Un panou complet care oglindește 2D-ul *nu* e în
  scop — ăla e un exercițiu ulterior.)

## Fișierul de teste

`tests/physics3d_tests.cpp` — deja scris, **încă nelegat în ținta de teste**;
adăugarea la `target_sources` e pasul unu. Cazurile fixează: direcțiile și
magnitudinea Coulomb (unități!), neutralitatea neutronului, comportamentul de
rază Yukawa (strong bate Coulomb la 1,5 fm, neglijabil la 10 fm, zero pentru
electroni), legea a treia a lui Newton, conservarea impulsului și distanța de
cădere din testul de unități.

## Prag de acceptare

- [ ] Toate cazurile `physics3d` verzi — inclusiv cel de unități, și l-ai
      văzut picând mai întâi.
- [ ] Convenția de unități documentată în header-ele pe care le guvernează.
- [ ] `Sandbox3D` arată sfere în mișcare: spawnează un proton și un electron
      și privește electronul accelerând spre el (softening-ul îl împiedică să
      explodeze la contact).
- [ ] Review-ul de ownership curat: proprietar `unique_ptr` + vedere de
      observatori bruți, justificate cu voce tare.
- [ ] Panoul: spawn/pauză/reset/număr funcționează.

Commit-uri sugerate (două, în ordinea asta):
`fix: make Physics3D integrator unit-consistent` ·
`feat: wire Physics3D particles into Sandbox3D`