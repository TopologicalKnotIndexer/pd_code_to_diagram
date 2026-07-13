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

The bundled C++ engine builds a crossing/socket tree, incrementally places crossings, and routes arcs through a grid path engine while avoiding occupied cells and preserving crossing over/under data. Python converts the matrix to cached antialiased Pillow tiles or traces a supported matrix back into crossing records. Matrix values use `0` for empty cells, positive integers for arcs, and negative values for the two crossing orientations.

## Input conventions

A PD code is represented as a list of four-entry crossings. Arc labels normally occur exactly twice. Public functions validate inputs and return new values rather than mutating caller-owned data unless their API explicitly says otherwise.

## External software

- A C++17 compiler such as GCC or Clang is required on first use to build the bundled layout executable.
- No GUI is required.
- Writing PNG images requires normal filesystem access.

## Development

Run examples and package checks before release. Python packages require Python 3.10 or newer. Build PyPI artifacts with:

```bash
poetry check
poetry build
```

## License

MIT. See `LICENSE`.
