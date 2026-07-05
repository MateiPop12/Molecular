# Measuring progress

The goal is to measure **learning**, not activity. Three signals, in order of
how much they actually predict learning:

1. **Scaffolding trend** — how much help each exercise needed (the H-scale
   below). Learning shows as H going *down* while difficulty holds or rises.
2. **Review cycles** — how many review → fix rounds until acceptance.
3. **Retention** — whether a concept taught once comes back as a bug later.
   Each new exercise deliberately re-uses at least one concept from an earlier
   one, which is what makes this measurable.

Time spent is recorded as *context* (the denominator), not as a target —
optimizing for speed teaches shortcuts, not physics or C++.

## The help scale (H)

| Level | Meaning |
| --- | --- |
| **H0** | Spec only — solved solo |
| **H1** | Conceptual hints (tool/function names, "look at `lexically_normal`") |
| **H2** | Pseudocode / an ordered step recipe |
| **H3** | Guided step-by-step with exact instructions |
| **H4** | Working code was shown |

!!! warning "Goodhart's law"
    Asking for help is **correct behavior**, not a penalty. The scale exists
    to calibrate the *next* exercise, never to grade the asking. The moment
    "low H" becomes the goal, the number goes down and the learning stops.
    What matters is the trend across exercises — never a single data point.

## Adjustment rules

Applied when the next exercise is scaffolded (`/new-exercise` reads this
table first):

- **H0–H1 and ≤ 2 cycles** → raise difficulty: more new concepts, less
  prescriptive spec.
- **H2 and ≤ 3 cycles** → hold difficulty, introduce new concepts.
- **H3+ or > 3 cycles** → hold difficulty and **consolidate**: the next
  exercise re-tests the same weak concepts, with a target of ≤ H1.
- A concept that appears as a review-found bug **twice** gets its own
  dedicated exercise.

## Scorecard

One row per exercise, filled in by Claude at review acceptance. **Time** is
self-reported by Matei (0.5 h granularity is plenty).

| # | Exercise | Days | Sessions | Time | Cycles | Peak help | Bugs: self / review | Weak concepts to re-test |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 001 | [doctest target](exercises/001-doctest-target.md) | 1 | 1 | ~2 h | 4 | H1 | 0 / 3 | CMake link-vs-include · RAII |
| 002 | [asset path helper](exercises/002-asset-path-helper.md) | 3 | 2 | ~3 h | 4 | H3 | 1\* / 3 | `static` init semantics · short-circuit guard ordering · reading linker errors |

\* found with the debugger during a guided session — counts, but with an
asterisk.

## Current reading (2026-07-05)

Two exercises in: cycles are flat at 4, and 002 needed more scaffolding than
001 (H3 vs H1). That's expected — 002 was a real API with hostile edge cases,
not build wiring — but per the rules, **difficulty holds for 003 and it must
re-test at least one of:** function-local `static` semantics, short-circuit
guard ordering, or reading compiler/linker diagnostics. Target: ≤ H1 on the
re-tested concept.