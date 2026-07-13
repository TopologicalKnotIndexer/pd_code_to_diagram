# PD-code layout engine

This C++17 program converts a valid planar diagram (PD) code into either a
routed two-dimensional integer grid or a serialized three-dimensional graph.
It is bundled inside the Python `pd-code-to-diagram` repository and does not
use a nested Git checkout.

## Build

Run the following command from this directory:

```bash
g++ -std=c++17 -O2 main.cpp -o pd_code_to_diagram
```

On Windows, use `pd_code_to_diagram.exe` as the output name. The Python wrapper
performs this build automatically when the executable is absent or older than
one of the committed C++ sources. Set the `CXX` environment variable to select
a compiler explicitly.

## Input

The program reads one PD code from standard input. A valid code has `n`
four-entry crossings, uses exactly the labels `1` through `2n`, and contains
each label exactly twice.

```text
[[6, 1, 7, 2], [8, 3, 5, 4], [2, 5, 3, 6], [4, 7, 1, 8]]
```

When entering data interactively, terminate standard input with Ctrl+Z then
Enter on Windows, or Ctrl+D on Unix-like systems. Redirecting a file is less
error-prone:

```bash
pd_code_to_diagram --diagram --with_zero < pdcode.txt
```

## Modes

- `--diagram` or `-d` prints the routed two-dimensional matrix.
- `--with_zero` or `-z` prints zeroes for empty diagram cells. It has an effect
  only with `--diagram`.
- `--serial` or `-s` prints the three-dimensional graph representation.
- `--border` or `-b` prints arc labels exposed on the outer border.
- `--components` or `-c` prints connected-component information.
- `--N`, where `N` is an arc label, requests that label's component on the
  outer border.

In a diagram matrix, `0` is empty space, a positive value is an arc label,
`-1` is a crossing whose vertical strand passes underneath, and `-2` is a
crossing whose horizontal strand passes underneath.

## Layout algorithm

The engine constructs a crossing/socket forest, places its tree edges as
single grid segments, and routes remaining arcs through a bounded path-search
grid while avoiding occupied cells. Candidate placements use deterministic
random seeds and bounded retries. If no non-overlapping layout can be found,
the program exits with an explicit error instead of retrying indefinitely or
accessing an empty result.
