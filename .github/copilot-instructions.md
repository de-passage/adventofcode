<!-- Repository-specific Copilot instructions for de-passage/adventofcode -->
# Copilot guidance — adventofcode (repo-specific)

Purpose: give an AI coding agent the minimal, actionable knowledge to be immediately productive in this repository.

Core reference
- Year 2024 is the canonical layout for the repo: every year folder follows the `src` + `input` layout. For example:
  - [2024/src/day12.cpp](2024/src/day12.cpp) — solution source for December 12, 2024.
  - [2024/input/day12.txt](2024/input/day12.txt) — the input used to run the program.
  - Example inputs: `2024/input/day12.example.txt` or `2024/input/day12_ex.txt` (patterns vary but live next to the main input).
- Note: in 2025 a CMake modification was introduced — check the year's `CMakeLists.txt` or the top-level `cmake/` helpers when building that year's folder.

Big picture
- Purpose: implement isolated algorithmic solutions for each Advent of Code puzzle day. Each `dayN.cpp` is intended to be a self-contained program that, given its input file, prints the puzzle answers.
- Scope: keep changes localized to one year/day. Only edit `common/` or `cmake/` when a true cross-year improvement is required.

Where to look first
- [README.md](README.md) — general notes (session token location, repo goals).
- Year folder `CMakeLists.txt` or `Makefile` — shows how targets are built for that year.
- `common/` and `cmake/` — shared helpers and build snippets.

Build & run (practical)
- Typical CMake flow (from a year folder):
```
cd 2024
mkdir -p build
cmake -S . -B build
cmake --build build --parallel
```
- After build, run the day's executable and point it at the input file (or let it open inputs internally):
```
./build/day12     # or the executable name shown by CMake
./build/day12 < input/day12.txt
```
- If a year uses a simple `Makefile` (older years), run `cd 2022 && make` and execute the produced binary.
- Always inspect the day's source to see whether it reads `argv` filenames, uses hardcoded filenames, or expects stdin.

Adding a new day (correct placement)
- Create `YEAR/src/dayN.cpp` and `YEAR/input/dayN.txt` (plus example inputs `*example*`).
- Update that year's `CMakeLists.txt` to add the new executable target, or use the year's helper script if present in `scripts/`.
- Keep names consistent: alternate solutions use `dayNbis.cpp` and live in the same `src` directory.

Repository conventions and patterns
- File naming: `day<N>.cpp`, `day<N>bis.cpp`, `day<N>.txt`, `day<N>.example.txt`.
- Minimal single-file solutions are preferred; only extract helpers into `*.hpp` when multiple days share code.
- Compiler flags per year live in `YEAR/compile_flags.txt` (e.g. `2023/compile_flags.txt`). Respect those flags when compiling single files manually.

Integration & credentials
- No external package manager: use the system `g++` toolchain and CMake.
- Session token for downloading inputs is expected at `$XDG_CONFIG_HOME/aoc/session` (do not hard-code tokens).

Examples to reference
- See [2023/compile_flags.txt](2023/compile_flags.txt) for preferred flags.
- See [2022/Makefile](2022/Makefile) for a simple per-year Makefile example.

If anything here is unclear or you want me to expand with concrete examples (e.g. show how to add `2024/src/day12.cpp` + `CMakeLists.txt` changes), tell me which year/day and I'll add the small code snippets.
