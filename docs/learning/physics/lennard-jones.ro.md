# 🔬 Potențialul Lennard-Jones

*Legea de forță din inima lui `Physics/` — ce modelează, de unde vin cei doi
exponenți și un loc în care implementarea ta se abate discret de la manual.*

---

## Întâi intuiția

Doi atomi neutri — fără sarcină netă, fără legătură — tot interacționează:

- **La distanță** (câteva diametre): se *atrag*, slab. Norii de electroni
  fluctuează; un dipol de moment pe un atom induce un dipol pe celălalt, iar
  cei doi dipoli se atrag. Aceasta este **forța de dispersie London**. Există
  între *toți* atomii, chiar și între gazele nobile — de aceea argonul poate fi
  lichid.
- **Striviți unul de altul**: se *resping*, violent. Norii de electroni încep
  să se suprapună, iar **principiul de excluziune al lui Pauli** interzice
  electronilor să ocupe aceleași stări — energia explodează. Atomii se comportă
  ca niște bile de biliard ușor moi.

Potențialul Lennard-Jones (LJ) este cea mai simplă funcție care surprinde
ambele regimuri cu doi parametri:

\[
V(r) \;=\; 4\varepsilon\!\left[\left(\frac{\sigma}{r}\right)^{12} - \left(\frac{\sigma}{r}\right)^{6}\right]
\]

| Simbol | Semnificație | Unități în acest proiect |
| --- | --- | --- |
| \( r \) | distanța centru-centru | nm |
| \( \varepsilon \) | adâncimea gropii — cât de puternic se leagă perechea | eV (`Atom.h:94`) |
| \( \sigma \) | distanța la care \( V = 0 \) — diametrul efectiv | nm (`Atom.h:95`) |

Trei repere de ținut minte:

- \( V(\sigma) = 0 \) — potențialul trece prin zero la \( r = \sigma \).
- Minimul e la \( r_\text{min} = 2^{1/6}\sigma \approx 1.122\,\sigma \), unde
  \( V(r_\text{min}) = -\varepsilon \). Aceasta e distanța de echilibru la care
  se așază o pereche.
- Pentru argon: \( \varepsilon \approx 0.0104 \) eV (echivalent
  \( \varepsilon/k_B \approx 120 \) K), \( \sigma \approx 0.34 \) nm. Observă
  cât de *mic* e ε pe lângă o legătură covalentă (~4 eV pentru C–C): dispersia
  e o șoaptă — de asta solidele de gaze nobile se topesc la temperaturi
  criogenice.

## De unde vin exponenții

**Termenul \( r^{-6} \) este fizică adevărată.** Teoria perturbațiilor de
ordinul doi pentru două distribuții de sarcină fluctuante dă o atracție care
scade exact ca \( r^{-6} \) (rezultatul van der Waals / London). Exponentul
acesta nu e un fit — e derivat.

**Termenul \( r^{-12} \) este o conveniență computațională.** Adevărata
respingere Pauli scade aproximativ *exponențial* cu suprapunerea,
\( \sim e^{-r/\rho} \). Lennard-Jones a ales 12 pentru că e suficient de abrupt
și pentru că \( (\sigma/r)^{12} = \left[(\sigma/r)^{6}\right]^2 \) — în 1924
(și tot așa, în bucla ta interioară) calculezi \( r^{-6} \) o dată și îl ridici
la pătrat. Uită-te la `ForceCalculator.cpp:25–26`: `r6`, apoi
`r12 = r6 * r6`. Folosești un truc de performanță vechi de 100 de ani.

## De la potențial la forță (ce calculează efectiv codul)

Forța este minus gradientul. Derivăm după \( r \):

\[
F(r) \;=\; -\frac{dV}{dr}
\;=\; \frac{48\,\varepsilon}{r}\left[\left(\frac{\sigma}{r}\right)^{12} - \frac{1}{2}\left(\frac{\sigma}{r}\right)^{6}\right]
\]

\( F \) pozitiv = respingere (împinge perechea în afară) pentru
\( r < r_\text{min} \); negativ = atracție pentru \( r > r_\text{min} \).

`ForceCalculator.cpp:27` calculează

```cpp
double forceMag = 48.0 * epsilon * (r12 - 0.5 * r6) / (r_len * r_len);
```

adică \( F(r)/r \) — împărțind la \( r^2 \) în loc de \( r \), pre-împarte cu
lungimea lui `r_vec`, așa că înmulțirea cu vectorul *nenormalizat* `r_vec` dă
vectorul de forță corect fără un apel la `normalize()`. Un `sqrt` evitat per
pereche. Verifică totuși că înmulțești cu `r_vec`, nu cu `r_hat`, altfel
mărimea e greșită cu un factor de \( r \).

Energia potențială de la `ForceCalculator.cpp:106` (`4.0 * epsilon * (r12 - r6)`)
este \( V(r) \) din manual — folosită la diagnosticele de energie; ea e mărimea
care ar trebui să rămână (aproximativ) conservată de integrator. Dacă energia
totală derivează, suspectează integratorul sau cutoff-ul, în ordinea asta.

## ⚠️ Regula ta de amestecare se abate de la manual

Când tipurile de atomi diferă (O–H, nu O–O), ai nevoie de parametri
încrucișați. Regulile standard **Lorentz–Berthelot** sunt:

\[
\sigma_{ab} = \frac{\sigma_a + \sigma_b}{2}
\qquad\text{(Lorentz — medie aritmetică)}
\]

\[
\varepsilon_{ab} = \sqrt{\varepsilon_a\,\varepsilon_b}
\qquad\text{(Berthelot — medie geometrică)}
\]

`ForceCalculator.cpp:14–15` folosește **media aritmetică pentru amândoi**.
Pentru σ e corect. Pentru ε nu e regula standard: cum media aritmetică e mereu
≥ media geometrică, simularea ta *supraestimează atracția dintre atomi
diferiți*, iar decalajul crește cu cât cele două valori ε diferă mai mult.
Pentru specii aproape egale eroarea e neglijabilă; pentru o pereche
grea/ușoară, nu.

**Acțiune:** ori treci ε pe media geometrică, ori documentezi deliberat de ce
ai ales media aritmetică. Fizică nestandard tăcută într-o licență e exact
genul de lucru pe care îl găsește un evaluator.

## Unde se strică modelul

- **Zero chimie.** Atomii LJ nu se leagă niciodată, nu reacționează, n-au
  direcționalitate. Structura tetraedrică a apei, de exemplu, cere sarcini
  explicite + geometrie.
- **Fără sarcini.** Ionii și moleculele polare au nevoie de un termen Coulomb
  deasupra (exact de asta `Physics3D/` adaugă Coulomb + Yukawa pentru domeniul
  subatomic).
- **\( r^{-12} \) e prea rigid.** Respingerea reală e exponențială; potențialul
  Buckingham \( A e^{-Br} - C/r^6 \) se potrivește mai bine, dar costă un
  `exp()` per pereche.
- **Aditivitate de pereche.** Însumarea termenilor de pereche ignoră efectele
  reale de trei corpuri (Axilrod–Teller). Eroare de ~5–10 % din energia de
  coeziune în sisteme dense de gaze nobile — acceptabil pentru un simulator de
  licență, merită o frază în redactare.
- **Cutoff-uri.** Trunchierea LJ la un \( r_c \) (uzual \( 2.5\sigma \))
  introduce o discontinuitate care injectează energie de fiecare dată când o
  pereche traversează cutoff-ul. Remediul standard: *deplasarea* potențialului
  astfel încât \( V(r_c) = 0 \). Merită verificat ce face bucla ta de forțe la
  distanțe mari.

## Mergi mai departe

- [Potențialul Lennard-Jones — Wikipedia (EN)](https://en.wikipedia.org/wiki/Lennard-Jones_potential) — temeinic, include formele derivate folosite mai sus.
- [Forța de dispersie London — Wikipedia (EN)](https://en.wikipedia.org/wiki/London_dispersion_force) — de unde vine \( r^{-6} \).
- [Lennard-Jones — LibreTexts (EN)](https://chem.libretexts.org/Bookshelves/Physical_and_Theoretical_Chemistry_Textbook_Maps/Supplemental_Modules_(Physical_and_Theoretical_Chemistry)/Physical_Properties_of_Matter/Atomic_and_Molecular_Properties/Intermolecular_Forces/Specific_Interactions/Lennard-Jones_Potential) — parcurgere mai blândă, cu grafice.
- [Căutare video: potențialul Lennard-Jones](https://www.youtube.com/results?search_query=lennard-jones+potential+molecular+dynamics) — mai multe explicații vizuale bune.
- Carte: Allen & Tildesley, *Computer Simulation of Liquids* — **referința**
  pentru tot ce e în `Physics/`, inclusiv corecții de cutoff și integratoare.