# <div align=center>kernzerfall's aoc24 solutions</div>

<div align=center>
  (Belated) Advent of Code 2024 Solutions in C11 using minimal parts of the standard library.
</div>
<p></p>
<div align=center>
  <img src="https://img.shields.io/badge/C11-00599C?logo=c&logoColor=white"></img>
  <img src="https://img.shields.io/badge/CMake-008FBA?logo=cmake&logoColor=white"></img>
</div>

## Building

* Recommended to use **LLVM** (Written/tested on 18.1.8)
* If it doesn't compile for you, you're probably using W\*ndows and I don't care.

### Commands
```bash
mkdir build/
cd build/
CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Benchmarks

| Day | Mean [µs] | Min [µs] | Max [µs] |
|:---|---:|---:|---:|
| `01` | 1715.6 ± 43.4 | 1649.6 | 2107.2 |
| `02` | 607.8 ± 43.5 | 529.9 | 813.1 |
| `03` | 356.4 ± 23.3 | 338.6 | 745.1 |
| `04` | 832.1 ± 24.7 | 806.8 | 1058.2 |
| `05` | 404.0 ± 27.8 | 385.2 | 676.7 |
