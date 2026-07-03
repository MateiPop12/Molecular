# 04 — Fizică: subatomic 3D

Locație: `Molecular/src/Molecular/Physics3D`. Aceasta este jumătatea **mai nouă,
în lucru** a proiectului (adăugată în commit-ul `c8fef8a`, „Added 3D physics
package"). Modelează **particule subatomice** individuale — protoni, neutroni,
electroni — la scara femtometrilor, nu atomi întregi.

> Stadiu: modelul de date, forțele și integratorul există și compilează, dar
> fizica **nu este încă legată de o buclă de randare**. `Sandbox3D` desenează
> momentan doar cutii și este comentat în `Sandbox.cpp` (layerul 2D este activ).
> Vezi [06 — Stadiu și plan](06-status-and-roadmap.md).

## Componente

| Fișier                        | Rol                                                        |
|-------------------------------|------------------------------------------------------------|
| `ParticleTypeData.h`          | Enum-ul particulelor + constante fizice CODATA             |
| `PhysicsObject.{h,cpp}`       | Particula de bază: poziție, viteză, masă, sarcină (`dvec3`)|
| `SubatomicParticle.{h,cpp}`   | Adaugă rază de randare + culoare peste `PhysicsObject`     |
| `ForceCalculator3D.{h,cpp}`   | Coulomb + forța tare Yukawa                                |
| `Integrator.{h,cpp}`          | Integrare Velocity-Verlet                                  |

> Notă despre arborele de lucru: git status arată redenumirea
> `Physics3D/ForceCalculator.{h,cpp}` → `ForceCalculator3D.{h,cpp}`, pentru a o
> dezambiguiza de `ForceCalculator`-ul 2D. De reținut când consulți commit-uri
> mai vechi.

## Datele particulelor (`ParticleTypeData.h`)

```cpp
enum class ParticleType : uint8_t { PROTON, NEUTRON, ELECTRON };
```

Fiecare tip stochează constante CODATA 2018/2022: masă (kg și MeV/c²), sarcină
(C), spin (stocat ca spin×2), moment magnetic și timp de viață (∞ pentru
particulele stabile; neutronul are ~880.2 s).

| Particulă | Masă (kg)        | Masă (MeV/c²) | Sarcină (C)       | Timp de viață |
|-----------|------------------|---------------|-------------------|---------------|
| Proton    | 1.67262192369e-27| 938.272       | +1.602176634e-19  | stabil        |
| Neutron   | 1.67492749804e-27| 939.565       | 0                 | 880.2 s       |
| Electron  | 9.1093837015e-31 | 0.511         | −1.602176634e-19  | stabil        |

## Modelul de stare

- `PhysicsObject` ține poziția/viteza ca `glm::dvec3`, masa și un pointer către
  `ParticleTypeData`-ul său (sarcina vine de acolo). Pozițiile sunt exprimate în
  **femtometri** (fm = 1e-15 m).
- `SubatomicParticle` îl extinde cu o rază de randare și o culoare, alese per
  tip de particulă.

## Forțe (`ForceCalculator3D`)

Se construiește cu o **lungime de înmuiere** (softening) `epsilon_fm`
(implicit 0.1 fm), care menține forțele finite când două particule aproape
coincid.

### Coulomb (electrostatic)

`CalculateCoulombForce(a, b)` — forța electrostatică standard, invers
proporțională cu pătratul distanței, în Newtoni, cu pozițiile în fm și cu
lungimea de înmuiere aplicată.

### Forța nucleară tare (aproximația Yukawa)

`CalculateStrongForce(a, b)` — o aproximație de tip
[potențial Yukawa](https://en.wikipedia.org/wiki/Yukawa_potential) a forței
tari reziduale, semnificativă doar la distanțe de ~2–3 fm. Ea este cea care
permite protonilor și neutronilor să se lege în ciuda respingerii Coulomb.

`CalculateTotalForce` = Coulomb + tare.

## Integrare (`Integrator`)

`VelocityVerlet(objects, deltaTime, forceCalc)` avansează toate particulele cu
un pas (`deltaTime` în secunde). Accelerația se calculează însumând forțele de
pereche de la toate celelalte particule și împărțind la masă. `Reset()` curăță
starea integratorului. Velocity-Verlet este ales pentru proprietățile lui bune
de conservare a energiei, importante pentru sisteme legate (care orbitează /
oscilează).

## Scena 3D (`Sandbox3D`)

`Sandbox/src/Sandbox3D.cpp`:

- Randează într-un `Framebuffer` offscreen afișat ca viewport ImGui.
- Folosește o `EditorCamera` (orbit/fly) pentru navigare.
- Momentan apelează `Renderer3D` ca să deseneze cutii (`DrawSpace`) — o scenă
  substituent. Pasul natural următor este legarea particulelor + forțelor din
  `Physics3D` în `OnUpdate` și desenarea lor ca sfere.