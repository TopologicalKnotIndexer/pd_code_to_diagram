# pd-code-to-diagram

Convert PD codes to routed diagram matrices, render images, and parse supported diagram matrices back to PD codes.

## Installation

```bash
pip install pd-code-to-diagram
```

## Usage example

```python
from pd_code_to_diagram import get_diagram_from_pd_code, diagram_to_png

pd = [[1, 5, 2, 4], [3, 1, 4, 6], [5, 3, 6, 2]]
diagram = get_diagram_from_pd_code(pd)
diagram_to_png(diagram, "trefoil.png", tile_size=40, show_socket_labels=True)
print(len(diagram), len(diagram[0]))
```

## Algorithm

The bundled C++ engine builds a crossing/socket tree, incrementally places crossings, and routes arcs through a grid path engine while avoiding occupied cells and preserving crossing over/under data. Python converts the matrix to cached antialiased Pillow tiles or traces a supported matrix back into crossing records. Matrix values use `0` for empty cells, positive integers for arcs, and negative values for the two crossing orientations. Layout and orientation inference use bounded retries and fail explicitly when a matrix is ambiguous.

## Input conventions

A PD code is represented as a list of four-entry crossings. Arc labels normally occur exactly twice. Public functions validate inputs and return new values rather than mutating caller-owned data unless their API explicitly says otherwise.

## External software

- A C++17 compiler such as GCC or Clang is required on first use to build the bundled layout executable. The build uses only Python's standard library and recompiles when a bundled source changes.
- No GUI is required.
- Writing PNG images requires normal filesystem access.

## Development

Python 3.10 or newer is required. Run the regression and C++ round-trip tests with:

```bash
python -m unittest discover -s tests -v
```

No PyPI publication is performed as part of repository maintenance.

## License

MIT. See `LICENSE`.
