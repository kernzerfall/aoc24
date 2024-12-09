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

| Day | Mean [ms] | Min [ms] | Max [ms] | Relative |
|:---|---:|---:|---:|---:|
| `01` | 2.0 ± 0.1 | 1.8 | 2.7 | 4.55 ± 0.75 |
| `02` | 0.7 ± 0.1 | 0.5 | 1.7 | 1.53 ± 0.31 |
| `03` | 0.4 ± 0.1 | 0.3 | 1.9 | 1.00 |
| `04` | 0.9 ± 0.1 | 0.8 | 1.4 | 2.18 ± 0.37 |
