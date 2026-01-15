# Advent of code

Code for adventofcode.com

## Usage

Create a repository for each year. In it, create a `src` and `input` subdirectories and copy the CMakeLists.txt in it (from another directory... let's fix this at some point soon).

Create a file `$XDG_CONFIG_HOME/aoc/session` and paste the AOC authentication token in it.

Everyday we need to update the CMake file to increase the day. I need to fix this. Maybe we can look at the system date as a heuristic. We'd also need to create the stub files automatically. And download the examples from the page.
See `BUILDING.md` for repository-root build instructions and target-name notes.
