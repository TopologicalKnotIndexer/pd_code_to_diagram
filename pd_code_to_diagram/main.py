"""Build and call the bundled C++ PD-code layout engine."""

from collections import Counter
import json
import os
from pathlib import Path
import shutil
import subprocess
from typing import Optional

try:
    from .run_file import run_program_with_input
    from .from_diagram import diagram_to_pd_code
except ImportError:  # Direct execution from the package directory.
    from run_file import run_program_with_input
    from from_diagram import diagram_to_pd_code


PACKAGE_DIR = Path(__file__).resolve().parent
CPP_DIR = PACKAGE_DIR / "cpp_src"
CPP_MAIN = CPP_DIR / "main.cpp"
BIN_FOLDER = PACKAGE_DIR / "bin"
EXE_FILE = BIN_FOLDER / ("pd_code_to_diagram.exe" if os.name == "nt" else "pd_code_to_diagram")


def _validate_pd_code(pd_code: object) -> list[list[int]]:
    if not isinstance(pd_code, list):
        raise TypeError("PD code must be a list of crossings")
    if not pd_code:
        raise ValueError("the layout engine requires at least one crossing")
    normalized: list[list[int]] = []
    for index, crossing in enumerate(pd_code):
        if not isinstance(crossing, (list, tuple)) or len(crossing) != 4:
            raise ValueError(f"crossing {index} must contain exactly four labels")
        row: list[int] = []
        for label in crossing:
            if isinstance(label, bool) or not isinstance(label, int) or label <= 0:
                raise ValueError("arc labels must be positive integers")
            row.append(label)
        normalized.append(row)

    counts = Counter(label for crossing in normalized for label in crossing)
    expected = set(range(1, 2 * len(normalized) + 1))
    if set(counts) != expected or any(count != 2 for count in counts.values()):
        raise ValueError("arc labels must be exactly 1..2n and occur twice each")
    return normalized


def _find_compiler() -> str:
    configured = os.environ.get("CXX")
    candidates = [
        configured,
        shutil.which("g++"),
        shutil.which("clang++"),
        shutil.which("c++"),
    ]
    if os.name == "nt":
        candidates.append(r"C:\ProgramData\mingw64\mingw64\bin\g++.exe")
    for candidate in candidates:
        if candidate and Path(candidate).is_file():
            return str(Path(candidate))
    raise FileNotFoundError("no C++17 compiler found; set CXX to the compiler path")


def _needs_rebuild() -> bool:
    if not EXE_FILE.is_file():
        return True
    binary_time = EXE_FILE.stat().st_mtime_ns
    sources = [CPP_MAIN, *CPP_DIR.rglob("*.h")]
    return any(source.stat().st_mtime_ns > binary_time for source in sources)


def create_exe_file(force: bool = False) -> tuple[bool, str]:
    """Compile the bundled engine when missing or older than its sources."""

    BIN_FOLDER.mkdir(parents=True, exist_ok=True)
    if not force and not _needs_rebuild():
        return True, f"layout engine is current: {EXE_FILE}"

    try:
        compiler = _find_compiler()
    except FileNotFoundError as exc:
        return False, str(exc)
    temporary = EXE_FILE.with_name(EXE_FILE.stem + ".build" + EXE_FILE.suffix)
    command = [
        compiler,
        "-std=c++17",
        "-O2",
        str(CPP_MAIN),
        "-o",
        str(temporary),
    ]
    try:
        result = subprocess.run(
            command,
            cwd=CPP_DIR,
            text=True,
            encoding="utf-8",
            errors="replace",
            capture_output=True,
            timeout=180,
            check=False,
        )
    except (OSError, subprocess.TimeoutExpired) as exc:
        return False, f"C++ build failed: {exc}"
    if result.returncode != 0:
        temporary.unlink(missing_ok=True)
        details = (result.stderr or result.stdout).strip()
        return False, f"C++ build failed with exit {result.returncode}: {details}"
    temporary.replace(EXE_FILE)
    return True, f"compiled layout engine: {EXE_FILE}"


def get_diagram_from_pd_code(
    pd_code: list[list[int]], border_val: Optional[int] = None
) -> list[list[int]]:
    """Return the routed integer matrix for a validated PD code."""

    normalized = _validate_pd_code(pd_code)
    if border_val is not None:
        if (
            isinstance(border_val, bool)
            or not isinstance(border_val, int)
            or not 1 <= border_val <= 2 * len(normalized)
        ):
            raise ValueError("border_val must be an arc label in the PD code")

    success, message = create_exe_file()
    if not success:
        raise RuntimeError(message)

    arguments = ["--diagram", "--with_zero"]
    if border_val is not None:
        arguments.append("--" + str(border_val))
    stdout, stderr, return_code = run_program_with_input(
        str(EXE_FILE), arguments, json.dumps(normalized), timeout=120
    )
    if return_code != 0:
        raise RuntimeError(stderr.strip() or f"layout engine exited {return_code}")

    diagram: list[list[int]] = []
    try:
        for raw_line in stdout.splitlines():
            if raw_line.strip():
                diagram.append([int(term) for term in raw_line.split()])
    except ValueError as exc:
        raise RuntimeError("layout engine returned a non-integer matrix") from exc
    if not diagram or not diagram[0]:
        raise RuntimeError("layout engine returned an empty matrix")
    width = len(diagram[0])
    if any(len(row) != width for row in diagram):
        raise RuntimeError("layout engine returned a ragged matrix")
    return diagram


def get_diagram_str_from_pd_code(
    pd_code: list[list[int]], border_val: Optional[int] = None
) -> str:
    """Return a whitespace-formatted view of a routed matrix."""

    diagram = get_diagram_from_pd_code(pd_code, border_val)
    width = max(len(str(value)) for row in diagram for value in row) + 1
    return "".join(
        "".join(" " * width if value == 0 else f"{value:{width}d}" for value in row)
        + "\n"
        for row in diagram
    )


def pd_code_diagram_sanity(
    pd_code: list[list[int]], *args, **kwargs
) -> tuple[bool, list[list[int]]]:
    """Round-trip a canonical PD code through the layout matrix."""

    pd_code = sorted(json.loads(json.dumps(_validate_pd_code(pd_code))))

    def add_arc(graph: dict[int, set[int]], left: int, right: int):
        graph.setdefault(left, set()).add(right)

    def add_edge(graph: dict[int, set[int]], left: int, right: int):
        add_arc(graph, left, right)
        add_arc(graph, right, left)

    graph: dict[int, set[int]] = {}
    for crossing in pd_code:
        add_edge(graph, crossing[0], crossing[2])
        add_edge(graph, crossing[1], crossing[3])

    node_sets: dict[int, set[int]] = {}
    visited: dict[int, int] = {}

    def visit(node: int, root: int):
        visited[node] = root
        node_sets[root].add(node)
        for neighbor in sorted(graph[node]):
            if neighbor not in visited:
                visit(neighbor, root)

    for node in sorted(graph):
        if node not in visited:
            node_sets[node] = set()
            visit(node, node)

    real_next: dict[int, int] = {}
    for nodes in node_sets.values():
        ordered = sorted(nodes)
        for index, node in enumerate(ordered):
            real_next[node] = ordered[(index + 1) % len(ordered)]

    for crossing in pd_code:
        first, _, third, _ = crossing
        if real_next[first] != third and real_next[third] != first:
            return False, []
    for index, crossing in enumerate(pd_code):
        if real_next[crossing[0]] != crossing[2]:
            pd_code[index] = crossing[2:4] + crossing[0:2]
            if real_next[pd_code[index][0]] != pd_code[index][2]:
                raise ValueError("could not orient PD crossing")

    diagram = get_diagram_from_pd_code(pd_code, *args, **kwargs)
    recovered = sorted(diagram_to_pd_code(diagram))
    return pd_code == recovered, recovered


if __name__ == "__main__":
    example = [
        [2, 9, 3, 10],
        [4, 11, 5, 12],
        [6, 7, 1, 8],
        [8, 5, 7, 6],
        [10, 1, 11, 2],
        [12, 3, 9, 4],
    ]
    print(get_diagram_str_from_pd_code(example))
