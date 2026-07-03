# 🔬 The Lennard-Jones potential

*The force law at the heart of `Physics/` — what it models, where its two
exponents come from, and one place where your implementation quietly deviates
from the textbook.*

---

## Intuition first

Two neutral atoms — no net charge, no bond — still interact:

- **Far apart** (a few diameters): they *attract*, weakly. Electron clouds
  fluctuate; a momentary dipole on one atom induces a dipole on the other, and
  the two dipoles pull together. This is the **London dispersion force**. It
  exists between *all* atoms, even noble gases — it's why argon can be a liquid.
- **Squeezed together**: they *repel*, violently. Electron clouds start to
  overlap, and the **Pauli exclusion principle** forbids the electrons from
  occupying the same states — energy skyrockets. Atoms behave like slightly
  squishy billiard balls.

The Lennard-Jones (LJ) potential is the simplest function that captures both
regimes with two parameters:

\[
V(r) \;=\; 4\varepsilon\!\left[\left(\frac{\sigma}{r}\right)^{12} - \left(\frac{\sigma}{r}\right)^{6}\right]
\]

| Symbol | Meaning | Units in this project |
| --- | --- | --- |
| \( r \) | centre-to-centre distance | nm |
| \( \varepsilon \) | well depth — how strongly the pair binds | eV (`Atom.h:94`) |
| \( \sigma \) | distance where \( V = 0 \) — effective diameter | nm (`Atom.h:95`) |

Three landmarks worth memorizing:

- \( V(\sigma) = 0 \) — the potential crosses zero at \( r = \sigma \).
- The minimum sits at \( r_\text{min} = 2^{1/6}\sigma \approx 1.122\,\sigma \),
  where \( V(r_\text{min}) = -\varepsilon \). This is the equilibrium bond-ish
  distance a pair settles into.
- For argon: \( \varepsilon \approx 0.0104 \) eV (equivalently
  \( \varepsilon/k_B \approx 120 \) K), \( \sigma \approx 0.34 \) nm. Note how
  *tiny* ε is next to a covalent bond (~4 eV for C–C): dispersion is a whisper,
  which is why noble-gas solids melt at cryogenic temperatures.

## Where the exponents come from

**The \( r^{-6} \) term is real physics.** Second-order quantum perturbation
theory of two fluctuating charge distributions gives an attraction that decays
exactly as \( r^{-6} \) (the van der Waals / London result). This exponent is
not a fit — it's derived.

**The \( r^{-12} \) term is a computational convenience.** True Pauli repulsion
decays roughly *exponentially* with overlap, \( \sim e^{-r/\rho} \). Lennard-Jones
chose 12 because it's steep enough and because
\( (\sigma/r)^{12} = \left[(\sigma/r)^{6}\right]^2 \) — in 1924 (and still, in
your inner loop) you compute \( r^{-6} \) once and square it. Look at
`ForceCalculator.cpp:25–26`: `r6` then `r12 = r6 * r6`. You are using a
100-year-old performance hack.

## From potential to force (what the code actually computes)

Force is the negative gradient. Differentiate along \( r \):

\[
F(r) \;=\; -\frac{dV}{dr}
\;=\; \frac{48\,\varepsilon}{r}\left[\left(\frac{\sigma}{r}\right)^{12} - \frac{1}{2}\left(\frac{\sigma}{r}\right)^{6}\right]
\]

Positive \( F \) = repulsion (pushes the pair apart) for \( r < r_\text{min} \);
negative = attraction for \( r > r_\text{min} \).

Your `ForceCalculator.cpp:27` computes

```cpp
double forceMag = 48.0 * epsilon * (r12 - 0.5 * r6) / (r_len * r_len);
```

which is \( F(r)/r \) — dividing by \( r^2 \) instead of \( r \) pre-divides by
the length of `r_vec`, so multiplying by the *unnormalized* vector `r_vec`
yields the correct force vector without a `normalize()` call. One `sqrt`
avoided per pair. Verify you multiply by `r_vec`, not `r_hat`, or the magnitude
is wrong by a factor of \( r \).

The potential energy at `ForceCalculator.cpp:106` (`4.0 * epsilon * (r12 - r6)`)
is the textbook \( V(r) \) — used for the energy diagnostics, and it's the
quantity that should stay (approximately) conserved by the integrator. If total
energy drifts, suspect the integrator or the cutoff, in that order.

## ⚠️ Your mixing rule deviates from the textbook

When atom types differ (O–H, not O–O), you need cross-parameters. The standard
**Lorentz–Berthelot rules** are:

\[
\sigma_{ab} = \frac{\sigma_a + \sigma_b}{2}
\qquad\text{(Lorentz — arithmetic mean)}
\]

\[
\varepsilon_{ab} = \sqrt{\varepsilon_a\,\varepsilon_b}
\qquad\text{(Berthelot — geometric mean)}
\]

`ForceCalculator.cpp:14–15` uses the **arithmetic mean for both**. For σ that's
correct. For ε it's not the standard rule: since the arithmetic mean is always
≥ the geometric mean, your simulation *overestimates the attraction between
dissimilar atoms*, and the gap grows the more the two ε values differ.
For near-equal species the error is negligible; for, say, a heavy/light pair it
is not.

**Action:** either switch ε to the geometric mean, or document deliberately why
the arithmetic mean was chosen. Silent nonstandard physics in a thesis is the
kind of thing a reviewer finds.

## Where the model breaks down

- **No chemistry.** LJ atoms never bond, never react, have no directionality.
  Water's tetrahedral structure, for instance, needs explicit charges + geometry.
- **No charges.** Ions and polar molecules need a Coulomb term on top
  (which is exactly why `Physics3D/` adds Coulomb + Yukawa for the subatomic
  domain).
- **\( r^{-12} \) is too stiff.** Real repulsion is exponential; the Buckingham
  potential \( A e^{-Br} - C/r^6 \) fits better but costs an `exp()` per pair.
- **Pairwise additivity.** Summing pair terms ignores genuine three-body
  effects (Axilrod–Teller). Error ~5–10 % of cohesive energy in dense noble-gas
  systems — acceptable for a thesis simulator, worth a sentence in the write-up.
- **Cutoffs.** Truncating LJ at some \( r_c \) (commonly \( 2.5\sigma \))
  introduces a discontinuity that injects energy each time a pair crosses the
  cutoff. Standard fix: *shift* the potential so \( V(r_c) = 0 \). Worth
  checking what your force loop does at long range.

## Go deeper

- [Lennard-Jones potential — Wikipedia](https://en.wikipedia.org/wiki/Lennard-Jones_potential) — thorough, includes the derivative forms used above.
- [London dispersion force — Wikipedia](https://en.wikipedia.org/wiki/London_dispersion_force) — where the \( r^{-6} \) comes from.
- [Lennard-Jones potential — LibreTexts](https://chem.libretexts.org/Bookshelves/Physical_and_Theoretical_Chemistry_Textbook_Maps/Supplemental_Modules_(Physical_and_Theoretical_Chemistry)/Physical_Properties_of_Matter/Atomic_and_Molecular_Properties/Intermolecular_Forces/Specific_Interactions/Lennard-Jones_Potential) — gentler walkthrough with plots.
- [Video search: Lennard-Jones potential](https://www.youtube.com/results?search_query=lennard-jones+potential+molecular+dynamics) — several good visual explainers.
- Book: Allen & Tildesley, *Computer Simulation of Liquids* — **the** reference
  for everything in `Physics/`, including cutoff corrections and integrators.