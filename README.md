# pd-code-to-diagram

Convert PD codes to routed diagram matrices and render or parse diagrams.

## Installation

```bash
pip install pd-code-to-diagram
```

## Quick start

Use `get_diagram_from_pd_code`, `diagram_to_image`, or `diagram_to_pd_code` from `pd_code_to_diagram`.

PD codes are lists of four-entry crossings. Each arc label must occur exactly twice. Functions validate their inputs and do not mutate caller-owned PD-code lists unless explicitly documented.

## Development

Use Python 3.10 or newer for Python packages. Build distributions with `poetry build`. Run the package's tests or examples before publishing. C++ projects require a modern standards-compliant compiler.

## License

MIT. See `LICENSE`.
