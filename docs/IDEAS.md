# Ideas & tooling backlog

Matei's ideas for improving the working experience and tooling around the
project — chat-session workflow, Claude skills, build tooling, measurement,
docs. Engine/physics feature ideas belong in the
[roadmap](06-status-and-roadmap.md) or GitHub issues, not here.

Add an idea the moment it comes up (say "add to ideas" in a session, or edit
directly). Review the list occasionally at kickoff; promote what's worth
doing, kill what isn't. Like the learning log, this page stays EN-only.

Legend: 💡 idea · 🚧 in progress · ✅ done · ❌ dropped

## Open

- 💡 **Version the Claude tooling** *(2026-07-05, Claude)* — `.claude/` is
  fully gitignored, so the three skills (`/kickoff`, `/new-exercise`,
  `/serve-docs`) live only on this machine. Un-ignore `.claude/skills/` (keep
  `settings.json` local) so the tooling survives a reinstall and travels with
  the repo.
- 💡 **Passive time tracking** *(2026-07-05, from the progress-scorecard
  discussion)* — ActivityWatch (local, private) or WakaTime CLion plugin, to
  feed the Time column in [Progress](learning/progress.md) automatically
  instead of self-reporting.
- 💡 **`/wrap-up` skill** *(2026-07-05)* — end-of-session mirror of
  `/kickoff`: draft the learning-log entry, update status tables/scorecard,
  propose the commit split. Today that was done by hand; it's a checklist, so
  it should be a skill.
- 💡 **CI for the test target** *(2026-07-05, Claude)* — GitHub Actions
  running `MolecularTests` on every push. Also the honest proof that fresh
  clones build — worth doing right after the imgui submodule fix lands.
- 💡 **`Shader::Create(const std::filesystem::path&)`** *(2026-07-05,
  deferred from exercise 002 review)* — renderer API takes `std::string`;
  call sites currently convert with `.string()`. A `path` overload would push
  the conversion to where encoding is decided. Small refactor, candidate for
  an exercise.
- 💡 **Exe-relative asset discovery** *(2026-07-05, from the `MOL_ASSETS_DIR`
  discussion)* — baked absolute path is right for dev builds, wrong for
  distribution. A runtime fallback (locate assets relative to the executable)
  would make the binary relocatable. Thesis-relevant as a build-design
  paragraph.

## Done / dropped

*(nothing yet — move entries here with a date and a one-line outcome)*