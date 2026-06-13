from __future__ import annotations

from functools import lru_cache
from math import cos, radians, sin
from os import PathLike
from typing import Optional

from PIL import Image, ImageDraw, ImageFont


DEFAULT_TILE_SIZE = 30
BASE_LINE_WIDTH = 4
EMPTY_TILE = 0
CROSSING_TILES = {
    -1: "n1",  # vertical strand under, horizontal strand over
    -2: "n2",  # horizontal strand under, vertical strand over
}
DIRECTIONS = (
    (-1, 0, 1 << 0),  # top
    (0, 1, 1 << 1),   # right
    (1, 0, 1 << 2),   # bottom
    (0, -1, 1 << 3),  # left
)
LABEL_COLOR = (220, 0, 0)
LINE_TILE_SYMBOLS = {
    3: ("MR", "MT"),
    5: ("MB", "MT"),
    6: ("MB", "MR"),
    9: ("ML", "MT"),
    10: ("ML", "MR"),
    12: ("MB", "ML"),
}
CROSSING_TILE_SYMBOLS = {
    "n1": ("MT", "MB", "C", "ML", "MR"),
    "n2": ("ML", "MR", "C", "MT", "MB"),
}


def _validate_diagram(diagram: list[list[int]]) -> tuple[int, int]:
    if not isinstance(diagram, list) or len(diagram) == 0:
        raise ValueError("diagram must be a non-empty list of rows")

    col_cnt: Optional[int] = None
    for row in diagram:
        if not isinstance(row, list) or len(row) == 0:
            raise ValueError("diagram rows must be non-empty lists")
        if col_cnt is None:
            col_cnt = len(row)
        elif len(row) != col_cnt:
            raise ValueError("diagram must be rectangular")

        for val in row:
            if not isinstance(val, int):
                raise TypeError("diagram values must be int")

    return len(diagram), col_cnt


def _draw_segments(
    tile_size: int,
    symbols: tuple[str, ...],
    line_width: int,
    line_color: str = "black",
    scale: int = 4,
) -> Image.Image:
    n = tile_size
    width = max(1, line_width)
    high_n = n * scale
    high_width = width * scale

    image = Image.new("RGB", (high_n, high_n), "white")
    draw = ImageDraw.Draw(image)

    def scaled_point(point: tuple[float, float]) -> tuple[float, float]:
        x, y = point
        return (x * scale, y * scale)

    mid = n / 2
    points = {
        "M": (mid, mid),
        "L": (0, mid),
        "R": (n, mid),
        "T": (mid, 0),
        "B": (mid, n),
    }

    def endpoint_from_mid(symbol: str) -> Optional[str]:
        if len(symbol) != 2:
            return None

        a, b = symbol
        if a == "M" and b in "LRTB":
            return b
        if b == "M" and a in "LRTB":
            return a
        return None

    arc_keys = {
        frozenset(("L", "T")),
        frozenset(("T", "R")),
        frozenset(("R", "B")),
        frozenset(("B", "L")),
    }

    def draw_corner_arc(p1: str, p2: str) -> None:
        radius = n / 2
        arc_defs = {
            frozenset(("L", "T")): ((0, 0), 0, 90),
            frozenset(("T", "R")): ((n, 0), 90, 180),
            frozenset(("R", "B")): ((n, n), 180, 270),
            frozenset(("B", "L")): ((0, n), 270, 360),
        }

        (cx, cy), start, end = arc_defs[frozenset((p1, p2))]
        steps = max(32, int(radius * scale))
        arc_points = []

        for i in range(steps + 1):
            angle = radians(start + (end - start) * i / steps)
            x = cx + radius * cos(angle)
            y = cy + radius * sin(angle)
            arc_points.append(scaled_point((x, y)))

        draw.line(arc_points, fill=line_color, width=high_width, joint="curve")

    line_indices = [i for i, symbol in enumerate(symbols) if symbol != "C"]
    arc_replacement: Optional[tuple[int, int, str, str]] = None
    if len(line_indices) == 2:
        i1, i2 = line_indices
        p1 = endpoint_from_mid(symbols[i1])
        p2 = endpoint_from_mid(symbols[i2])

        if p1 and p2 and frozenset((p1, p2)) in arc_keys:
            arc_replacement = (i1, i2, p1, p2)

    for i, symbol in enumerate(symbols):
        if arc_replacement and i == arc_replacement[0]:
            _, _, p1, p2 = arc_replacement
            draw_corner_arc(p1, p2)
            continue

        if arc_replacement and i == arc_replacement[1]:
            continue

        if symbol == "C":
            radius = 1.5 * width * scale
            x, y = scaled_point(points["M"])
            draw.ellipse(
                [x - radius, y - radius, x + radius, y + radius],
                fill="white",
            )
            continue

        if len(symbol) != 2:
            raise ValueError(f"invalid tile symbol: {symbol}")

        a, b = symbol
        if a not in points or b not in points:
            raise ValueError(f"invalid tile segment: {symbol}")

        draw.line(
            [scaled_point(points[a]), scaled_point(points[b])],
            fill=line_color,
            width=high_width,
        )

    return image.resize((n, n), Image.Resampling.LANCZOS)


@lru_cache(maxsize=None)
def _load_tiles(tile_size: int) -> dict[int | str, Image.Image]:
    line_width = max(1, round(tile_size * BASE_LINE_WIDTH / DEFAULT_TILE_SIZE))
    tiles: dict[int | str, Image.Image] = {
        EMPTY_TILE: _draw_segments(tile_size, tuple(), line_width)
    }

    for mask, symbols in LINE_TILE_SYMBOLS.items():
        tiles[mask] = _draw_segments(tile_size, symbols, line_width)

    for name, symbols in CROSSING_TILE_SYMBOLS.items():
        tiles[name] = _draw_segments(tile_size, symbols, line_width)

    return tiles


def _load_label_font(tile_width: int, tile_height: int) -> ImageFont.ImageFont:
    font_size = max(7, min(tile_width, tile_height) // 3)
    for font_name in ("arial.ttf", "DejaVuSans.ttf"):
        try:
            return ImageFont.truetype(font_name, font_size)
        except OSError:
            pass
    return ImageFont.load_default()


def _tile_key_for_cell(
    diagram: list[list[int]],
    row_cnt: int,
    col_cnt: int,
    i: int,
    j: int,
) -> int | str:
    val = diagram[i][j]
    if val == 0:
        return EMPTY_TILE
    if val in CROSSING_TILES:
        return CROSSING_TILES[val]
    if val < 0:
        raise ValueError(f"unsupported crossing value {val} at ({i}, {j})")

    mask = 0
    for dx, dy, bit in DIRECTIONS:
        ni = i + dx
        nj = j + dy
        if 0 <= ni < row_cnt and 0 <= nj < col_cnt:
            neighbor = diagram[ni][nj]
            if neighbor == val or neighbor < 0:
                mask |= bit

    if mask == EMPTY_TILE:
        raise ValueError(f"line value {val} at ({i}, {j}) has no connections")
    if mask not in LINE_TILE_SYMBOLS:
        raise ValueError(
            f"line value {val} at ({i}, {j}) needs unsupported tile {mask:04b}"
        )
    return mask


def _draw_socket_labels(
    image: Image.Image,
    diagram: list[list[int]],
    row_cnt: int,
    col_cnt: int,
    tile_width: int,
    tile_height: int,
) -> None:
    draw = ImageDraw.Draw(image)
    font = _load_label_font(tile_width, tile_height)
    margin = max(2, min(tile_width, tile_height) // 10)

    label_positions = (
        (-1, 0, 1, 1, "rd"),  # top neighbor: bottom-right
        (0, 1, 0, 0, "la"),   # right neighbor: top-left
        (1, 0, 0, 0, "la"),   # bottom neighbor: top-left
        (0, -1, 1, 1, "rd"),  # left neighbor: bottom-right
    )

    for i in range(row_cnt):
        for j in range(col_cnt):
            if diagram[i][j] not in CROSSING_TILES:
                continue

            for dx, dy, tile_x, tile_y, anchor in label_positions:
                ni = i + dx
                nj = j + dy
                if not (0 <= ni < row_cnt and 0 <= nj < col_cnt):
                    continue

                val = diagram[ni][nj]
                if val <= 0:
                    continue

                x = (nj + tile_x) * tile_width
                y = (ni + tile_y) * tile_height
                if tile_x == 0:
                    x += margin
                else:
                    x -= margin
                if tile_y == 0:
                    y += margin
                else:
                    y -= margin

                draw.text((x, y), str(val), fill=LABEL_COLOR, font=font, anchor=anchor)


def diagram_to_image(
    diagram: list[list[int]],
    *,
    tile_size: Optional[int] = None,
    show_socket_labels: bool = False,
) -> Image.Image:
    row_cnt, col_cnt = _validate_diagram(diagram)
    if tile_size is not None and tile_size <= 0:
        raise ValueError("tile_size must be positive")
    if not isinstance(show_socket_labels, bool):
        raise TypeError("show_socket_labels must be bool")

    real_tile_size = DEFAULT_TILE_SIZE if tile_size is None else tile_size
    tiles = _load_tiles(real_tile_size)
    tile_width, tile_height = next(iter(tiles.values())).size
    image = Image.new("RGB", (col_cnt * tile_width, row_cnt * tile_height), "white")

    for i in range(row_cnt):
        for j in range(col_cnt):
            tile_key = _tile_key_for_cell(diagram, row_cnt, col_cnt, i, j)
            image.paste(tiles[tile_key], (j * tile_width, i * tile_height))

    if show_socket_labels:
        _draw_socket_labels(image, diagram, row_cnt, col_cnt, tile_width, tile_height)

    return image


def diagram_to_png(
    diagram: list[list[int]],
    output_path: str | PathLike[str],
    *,
    tile_size: Optional[int] = None,
    show_socket_labels: bool = False,
) -> Image.Image:
    image = diagram_to_image(
        diagram,
        tile_size=tile_size,
        show_socket_labels=show_socket_labels,
    )
    image.save(output_path, format="PNG")
    return image
