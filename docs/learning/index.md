# How this knowledge base works

This is the reading side of the learning system. The chat stays terse; anything
worth keeping — physics facts, graphics/architecture deep-dives, exercises,
reviews — lands **here**, versioned in git, searchable, with rendered math.

## Running the site

From the repo root:

```powershell
.\.venv\Scripts\mkdocs.exe serve
```

Then open <http://127.0.0.1:8000>. The site live-reloads whenever a page
changes — leave it open in a browser tab during sessions.

## What lives where

| Content | Location |
| --- | --- |
| 🔬 Physics notes | `docs/learning/physics/<topic>.md` |
| 🎨 Graphics / architecture notes | `docs/learning/graphics/<topic>.md` |
| 🦀 Rust/Zig seeds | `docs/learning/rust-zig/<topic>.md` |
| 🧩 Exercise specs | `docs/learning/exercises/NNN-<title>.md` |
| Exercise test code | `tests/` (real, compiled, run via CTest) |
| Session journal | [Learning log](../LEARNING_LOG.md) |

## The exercise loop

1. **Spec** — each exercise page states the goal, the concept it teaches, the
   exact signatures to implement, and the acceptance bar.
2. **Red → green** — every exercise ships with doctest unit tests. You start
   with failing (or non-compiling) tests and make them pass.
3. **Review** — once green, Claude reviews the solution critically:
   correctness, idiom, ownership, naming. Fixes are required, not optional.

An exercise counts as **done** only when the tests pass *and* the review is
clean. Status is tracked in the table below and chased at the start of every
session.

## Exercise status

| # | Exercise | Assigned | Status |
| --- | --- | --- | --- |
| 001 | [doctest test target](exercises/001-doctest-target.md) | 2026-07-03 | ✅ done (2026-07-03) |
| 002 | [asset path helper](exercises/002-asset-path-helper.md) | 2026-07-03 | 🔴 open |