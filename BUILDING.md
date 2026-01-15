Building

This repository supports two ways to build:

- Per-year (existing behavior): configure and build a single year directory (e.g. 2023) as before.
- Repository-root (new): configure and build from the repository root to build all years at once.

When building from the repository root we automatically set a per-year target prefix so targets from different years do not collide. For example, the target for `day1` in year `2023` becomes `2023_day1` when configured from the repo root. When configuring a single year directory directly, targets keep their original names (e.g. `day1`).

Examples:

```bash
# Configure & build all years from repository root
mkdir -p build && cmake -S . -B build
cmake --build build --parallel

# Configure & build a single year (unchanged behavior)
mkdir -p 2023/build && cmake -S 2023 -B 2023/build
cmake --build 2023/build --parallel
```

Targets and run targets naming:

- When built from the repo root the target names are prefixed with the year and an underscore, e.g. `2023_day1`, and the run targets are `run-2023_day1` and `run-2023_day1-example`.
- When built per-year the target names are unchanged, e.g. `day1`, and run targets are `run-day1` / `run-day1-example`.

Notes:

- Input fetching (`scripts/get_input.bash`) is invoked by CMake using the year derived from the year directory, so input downloads will request the correct year regardless of whether you configure per-year or from the repo root.
- If you rely on hardcoded target names in scripts or CI, update them to use the year-prefixed names when invoking the root build.
